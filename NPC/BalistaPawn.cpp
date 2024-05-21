#include "NPC/BalistaPawn.h"
#include "Catapult/CatapultGun.h"
#include "Kismet/GameplayStatics.h"
#include "ResourceManager.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Particles/ParticleSystemComponent.h"
#include "Bullets/GameBulletActor.h"
#include "UI/NPCHealthWidget.h"
#include "Components/WidgetComponent.h"

#include "Aiming/ProjectileShootingComponent.h"
#include "Aiming/ProjectileWeaponAiming.h"

#include "Character/Weapon_ActorComponent.h"
#include "Character/HealthActorComponent.h"

#include "AI/BalistaAIController.h"

#include "UI/InventorySlotItem.h"

ABalistaPawn::ABalistaPawn()
{
	
	PrimaryActorTick.bCanEverTick = true;

	catapultDown = false;

	USkeletalMesh* baseMesh = ResourceManager::Get<USkeletalMesh>("balistaBase");

	catapultMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>("balistaMeshComponent");
	catapultMeshComponent->SetSkeletalMesh(baseMesh);
	catapultMeshComponent->SetCollisionProfileName("Pawn");


	bulletSocket = NewObject<USkeletalMeshSocket>(baseMesh);
	bulletSocket->SocketName = "WeaponSocket";
	bulletSocket->BoneName = "Root";
	bulletSocket->RelativeLocation = FVector(-140.0f, -180.0f, 0.0f);
	bulletSocket->RelativeRotation = FRotator(0.0f, 180.0f, 0.0f);

	catapultMeshComponent->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	catapultMeshComponent->SetEnableGravity(true);
	catapultMeshComponent->SetNotifyRigidBodyCollision(true);

	SetRootComponent(catapultMeshComponent);

	explosionParticle = CreateDefaultSubobject<UParticleSystemComponent>("explosionParticle");
	explosionParticle->SetTemplate(ResourceManager::Get<UParticleSystem>("explosionParticle"));
	explosionParticle->CustomTimeDilation = 0.8f;
	explosionParticle->SetWorldScale3D(FVector(4.0f, 4.0f, 4.0f));
	explosionParticle->SetAutoActivate(false);
	explosionParticle->Deactivate();


	weaponComponent = CreateDefaultSubobject<UWeapon_ActorComponent>("Weapon");
	healthComponent = CreateDefaultSubobject<UHealthActorComponent>("Health");
}

// Called when the game starts or when spawned
void ABalistaPawn::BeginPlay()
{
	Super::BeginPlay();
	
	healthComponent->Init(100, 100);
	healthComponent->OnHealthChanged.AddDynamic(this, &ABalistaPawn::OnCatapultDamaged);
	healthBarWidget = GetHealthBarWidget();

	InitNPC(NPC_TYPE_CATAPULT, this, healthComponent, true, healthBarWidget, castleInstance);


	if (AIControllerClass)
	{
		weaponComponent->Init(catapultMeshComponent);
		weaponComponent->SetBulletType("BalistaSpear");


		shootingComponent = NewObject<UProjectileShootingComponent>(this);
		shootingComponent->Init(this, weaponComponent);
		shootingComponent->RegisterComponent();

		shootingComponent->SetMinAndMaxAimTension(-20.0f, 30.0f);
		shootingComponent->SetAimPitch(50.0f);
		shootingComponent->SetVelocity(2000.0f);
	}

	FAttachmentTransformRules rules(EAttachmentRule::KeepRelative, true);
	FAttachmentTransformRules rules2(EAttachmentRule::KeepRelative, true);

	catapultMeshComponent->OnComponentHit.AddUniqueDynamic(this, &ABalistaPawn::OnHit);

	DeactivateAI();

}

UWidgetComponent* ABalistaPawn::GetHealthBarWidget_Implementation()
{
	return NULL;
}

void ABalistaPawn::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (!OtherActor->IsA(AGameBulletActor::StaticClass())) return;
	AGameBulletActor* bullet = Cast<AGameBulletActor>(OtherActor);
	if (!bullet->CanDamage()) return;


	healthComponent->RemoveActorHealth(bullet->GetDamage());

	bullet->RemoveDamage();

	

}

void ABalistaPawn::OnCatapultDamaged(int newHealth)
{
	if (catapultDown) return;

	OnDamaged();

	healthBarWidget->RequestRenderUpdate();

	if (newHealth <= 0)
	{
		OnCatapultDied();
	}
}

void ABalistaPawn::OnCatapultDied()
{
	if (catapultDown) return;
	catapultDown = true;

	OnDestroyedNPC();

	explosionParticle->SetWorldLocation(GetActorLocation());
	explosionParticle->Activate();
	explosionParticle->ActivateSystem();
	explosionParticle->OnSystemFinished.AddUniqueDynamic(this, &ABalistaPawn::OnExplosionOver);

	healthBarWidget->DestroyComponent();
	healthComponent->DestroyComponent();
	shootingComponent->DestroyComponent();

	DeactivateAI();
	
}

// Called every frame
void ABalistaPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	Update(DeltaTime);



	if (shootingComponent)
	{
		weaponComponent->SetRelativeLocation(FVector(shootingComponent->GetAimPitch(), 0.0f, 0.0f));
	}

}

// Called to bind functionality to input
void ABalistaPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

UBehaviorTree* ABalistaPawn::GetBehavorTree() const
{
	return behavorTree;
}


bool ABalistaPawn::IsReadyToShot()
{
	return shootingComponent->IsLoaded();
}
bool ABalistaPawn::IsAimOnPlayer()
{
	return shootingComponent ? shootingComponent->GetAiming()->IsAimOnPlayer() : false;
}
void ABalistaPawn::Shot()
{
	if (shootingComponent && shootingComponent->IsRegistered()) {
		if (shootingComponent->IsLoaded())
		{
			shootingComponent->Shot();
		}
	}
}

void ABalistaPawn::SetShootingState(CharacterShootingState newState)
{
	shootingState = newState;
}

CharacterShootingState ABalistaPawn::GetShootingState()
{
	return shootingState;
}

void ABalistaPawn::StartAim()
{
	if (shootingState == CharacterShootingState::START_AIMING) return;
	SetShootingState(CharacterShootingState::START_AIMING);
}
void ABalistaPawn::StopAim()
{
	if (shootingState == CharacterShootingState::NOT_AIMING) return;
	SetShootingState(CharacterShootingState::NOT_AIMING);
}

float ABalistaPawn::GetAimPitch()
{
	return shootingComponent ? shootingComponent->GetAimPitch() : 0.0f;
}

void ABalistaPawn::OnExplosionOver(class UParticleSystemComponent* PSystem)
{
	explosionParticle->DestroyComponent();
	Destroy();
}

UProjectileShootingComponent* ABalistaPawn::GetShootingComponent()
{
	return shootingComponent;
}

void ABalistaPawn::ActivateAI()
{
	ABalistaAIController* aiController = Cast<ABalistaAIController>(GetController());
	if (aiController)
	{
		aiController->Activate();
	}
}
void ABalistaPawn::DeactivateAI()
{
	ABalistaAIController* aiController = Cast<ABalistaAIController>(GetController());
	if (aiController)
	{
		aiController->Deactivate();
	}
}