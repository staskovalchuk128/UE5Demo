#include "NPC/ManTrooperChar.h"
#include "Kismet/GameplayStatics.h"
#include "ResourceManager.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Bullets/GameBulletActor.h"
#include "Engine/TextureRenderTarget2D.h"

#include "AI/TrooperManAIController.h"
#include "AI/WalkingTrooperManAIController.h"
#include "Components/CapsuleComponent.h"

#include "Aiming/ProjectileShootingComponent.h"
#include "Aiming/ProjectileWeaponAiming.h"
#include "Character/Weapon_ActorComponent.h"
#include "Character/HealthActorComponent.h"


#include "Player/PlayerCatapult.h"

#include "GameFramework/PawnMovementComponent.h"

#include "MeleeWeapons/MeleeWeaponsManager.h"

#include "BehaviorTree/BehaviorTree.h"
#include "Math/UnrealMathUtility.h"
#include "Blueprint/UserWidget.h"
#include "UI/NPCHealthWidget.h"
#include "Components/WidgetComponent.h"

// Sets default values
AManTrooperChar::AManTrooperChar() : CharacterCommon()
{
	shootingState = CharacterShootingState::NOT_AIMING;
	worldState = CharacterWorldState::ON_FEET;

	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	USkeletalMesh* manMesh = ConstructorHelpers::FObjectFinder<USkeletalMesh>(TEXT("/Script/Engine.SkeletalMesh'/Game/Meshes/Men/man.man'")).Object;
	GetMesh()->SetSkeletalMesh(manMesh);

	GetMesh()->SetNotifyRigidBodyCollision(true);
	GetCapsuleComponent()->SetNotifyRigidBodyCollision(true);


	weaponComponent = CreateDefaultSubobject<UWeapon_ActorComponent>("Weapon");
	healthComponent = CreateDefaultSubobject<UHealthActorComponent>("Health");

	justDummy = false;
}

void AManTrooperChar::Destroyed()
{
	Super::Destroyed();
	if (shootingComponent && shootingComponent->IsValidLowLevel()) shootingComponent->DestroyComponent();
	if (weaponComponent && weaponComponent->IsValidLowLevel()) weaponComponent->DestroyComponent();
	OnDestroyedNPC();
}

UWidgetComponent* AManTrooperChar::GetHealthBarWidget_Implementation()
{
	return NULL;
}

// Called when the game starts or when spawned
void AManTrooperChar::BeginPlay()
{
	
	Super::BeginPlay();

	if (!justDummy)
	{
		healthComponent->Init(15, 15);
		healthComponent->OnHealthChanged.AddDynamic(this, &AManTrooperChar::OnCharDamaged);
		healthBarWidget = GetHealthBarWidget();
		
		InitNPC(NPC_TYPE_MAN, this, healthComponent, showHealthBar, healthBarWidget, castleInstance);

		weaponComponent->Init(GetMesh());
		weaponComponent->SetBulletType("ManSimpleSpear");



		shootingComponent = NewObject<UProjectileShootingComponent>(this);
		shootingComponent->Init(this, weaponComponent);
		shootingComponent->RegisterComponent();


		GetCapsuleComponent()->GetBodyInstance()->bLockXRotation = true;
		GetCapsuleComponent()->GetBodyInstance()->bLockYRotation = true;
		GetCapsuleComponent()->GetBodyInstance()->SetDOFLock(EDOFMode::SixDOF);


		GetCapsuleComponent()->OnComponentHit.AddUniqueDynamic(this, &AManTrooperChar::OnHit);
		GetCapsuleComponent()->OnComponentBeginOverlap.AddUniqueDynamic(this, &AManTrooperChar::OnOverlapBegin);
		GetMesh()->GetAnimInstance()->OnMontageEnded.AddDynamic(this, &AManTrooperChar::HandleOnMontageEnded);


	}


	// by default troopers not moving
	DeactivateAI();
}

void AManTrooperChar::ActivateAI()
{
	if (ATrooperManAIController* aiController = Cast<ATrooperManAIController>(GetController()))
	{
		aiController->Activate();
	}
	else if (AWalkingTrooperManAIController* aiControllerWalking = Cast<AWalkingTrooperManAIController>(GetController()))
	{
		aiControllerWalking->Activate();
	}
}

void AManTrooperChar::DeactivateAI()
{
	if (ATrooperManAIController* aiController = Cast<ATrooperManAIController>(GetController()))
	{
		aiController->Deactivate();
	}
	else if (AWalkingTrooperManAIController* aiControllerWalking = Cast<AWalkingTrooperManAIController>(GetController()))
	{
		aiControllerWalking->Deactivate();
	}
}


void AManTrooperChar::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (OtherActor->IsA(AGameBulletActor::StaticClass()))
	{
		//Check if actor not ignored
		AGameBulletActor* bulletPtr = Cast<AGameBulletActor>(OtherActor);
		if (bulletPtr->GetBulletMeshComponent()->GetMoveIgnoreActors().Find(this) == -1)
		{
			OnBulletHit(Cast<AGameBulletActor>(OtherActor), NormalImpulse);
		}
	}
	else if (OtherActor->IsA(APlayerCatapult::StaticClass()))
	{
		OnCatapultHit(Cast<APlayerCatapult>(OtherActor), NormalImpulse);
	}
	
}

void AManTrooperChar::OnCharacterIsDoneAiming()
{
	SetShootingState(CharacterShootingState::AIMING);
}

void AManTrooperChar::OnCharacterGotUpAfterThrow()
{
	GetCapsuleComponent()->SetSimulatePhysics(false);
	GetCapsuleComponent()->GetBodyInstance()->bLockZRotation = false;
	GetCapsuleComponent()->GetBodyInstance()->SetDOFLock(EDOFMode::Default);
}

void AManTrooperChar::ThrowNpc(const FVector& normalImpulse)
{
	if (worldState == CharacterWorldState::DYING) return;
	GetCapsuleComponent()->SetSimulatePhysics(true);
	GetCapsuleComponent()->GetBodyInstance()->bLockZRotation = true;
	GetCapsuleComponent()->GetBodyInstance()->SetDOFLock(EDOFMode::Default);
	GetCapsuleComponent()->AddImpulse(normalImpulse / 10.0f);
	SetWorldState(CharacterWorldState::THROWN_BACK);

	PlayAnimMontage(ResourceManager::Get<UAnimMontage>("animThrownBack"));
}

void AManTrooperChar::OnCatapultHit(APlayerCatapult* catapultPtr, const FVector& normalImpulse)
{
	float velocity = catapultPtr->GetVelocity().Size();
	if (velocity < 250.0f) return;
	StopAim();
	ThrowNpc(normalImpulse);
	DamageActor(static_cast<int>(velocity / 300.0f));
}


void AManTrooperChar::OnBulletHit(AGameBulletActor* bulletPtr, const FVector& normalImpulse)
{

	if (!bulletPtr->CanDamage()) return;

	if (bulletPtr->IsHeavyBullet())
	{
		ThrowNpc(normalImpulse);
	}

	DamageActor(bulletPtr->GetDamage());

	bulletPtr->RemoveDamage();
	
}

// Called every frame
void AManTrooperChar::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	Update(DeltaTime);

	if (waitingToShot)
	{
		shotAwaitTime += DeltaTime;
		if (shotAwaitTime >= shotDelay)
		{
			Shot();
		}
	}

	if (worldState == CharacterWorldState::DEAD)
	{
		DestroyCharacter();
	}
}

// Called to bind functionality to input
void AManTrooperChar::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}


UBehaviorTree* AManTrooperChar::GetBehavorTree() const
{
	return behavorTree;
}

bool AManTrooperChar::IsJustDummy()
{
	return justDummy;
}

bool AManTrooperChar::IsReadyToShot()
{
	return shootingComponent ? shootingComponent->IsLoaded() : false;
}
bool AManTrooperChar::IsAimOnPlayer()
{
	return shootingComponent ? shootingComponent->GetAiming()->IsAimOnPlayer() : false;
}

CharacterWorldState AManTrooperChar::GetWorldState()
{
	return worldState;
}
CharacterShootingState AManTrooperChar::GetShootingState()
{
	return shootingState;
}

void AManTrooperChar::SetWorldState(CharacterWorldState newState)
{
	worldState = newState;
}

void AManTrooperChar::SetShootingState(CharacterShootingState newState)
{
	shootingState = newState;
}

void AManTrooperChar::ShotWithDelay(float delayFrom, float delayTo)
{
	if (waitingToShot) return;
	waitingToShot = true;
	shotDelay = FMath::RandRange(delayFrom, delayTo);
	shotAwaitTime = 0.0f;
}

void AManTrooperChar::CancelShot()
{
	waitingToShot = false;
	shotAwaitTime = 0.0f;
}

void AManTrooperChar::Shot()
{
	CancelShot();

	if (shootingComponent && shootingComponent->IsRegistered()) {
		if (shootingComponent->IsLoaded())
		{
			PlayAnimMontage(ResourceManager::Get<UAnimMontage>("animThrowSpear"));
			SetShootingState(CharacterShootingState::SHOT);
			shootingComponent->Shot();
			shootingComponent->GetAiming()->Deactivate();
		}
	}
}
UProjectileShootingComponent* AManTrooperChar::GetShootingComponent()
{
	return shootingComponent;
}

void AManTrooperChar::StartAim()
{
	if (IsDoneAiming() || shootingState == CharacterShootingState::START_AIMING) return;
	SetShootingState(CharacterShootingState::START_AIMING);
	PlayAnimMontage(ResourceManager::Get<UAnimMontage>("animGettingReadyToAim"));
	weaponComponent->SetRelativeRotation(FRotator(10.0f, -180.0f, 0.0f));
}
void AManTrooperChar::StopAim()
{
	if (shootingState == CharacterShootingState::NOT_AIMING) return;

	SetShootingState(CharacterShootingState::NOT_AIMING);
	weaponComponent->SetRelativeRotation(FRotator(0.0f, 0.0f, 0.0f));
}
bool AManTrooperChar::IsDoneAiming()
{
	return shootingState == CharacterShootingState::AIMING;
}

float AManTrooperChar::GetAimPitch()
{
	return shootingComponent ? shootingComponent->GetAimPitch() : 0.0f;
}

void AManTrooperChar::OnTowerUnderFeetExploded()
{
	if (shootingComponent)
	{
		FVector front = shootingComponent->GetFront();

		GetCapsuleComponent()->GetBodyInstance()->bLockZRotation = true;
		GetCapsuleComponent()->GetBodyInstance()->SetDOFLock(EDOFMode::Default);

		GetCapsuleComponent()->AddImpulse(front * 500.0f);
	}

	SetWorldState(CharacterWorldState::JUMPING_DEAD_FROM_TOWER);

}

void AManTrooperChar::OnLandedDeadFromTower()
{
	SetWorldState(CharacterWorldState::DEAD);
}

void AManTrooperChar::DestroyCharacter()
{
	if(GetController()) GetController()->Destroy();
	Destroy();
}

void AManTrooperChar::DamageActor(int units)
{
	healthComponent->RemoveActorHealth(units);
}


void AManTrooperChar::OnCharDamaged(int newHealth)
{
	OnDamaged();
	healthBarWidget->RequestRenderUpdate();

	if (worldState != CharacterWorldState::DYING && worldState != CharacterWorldState::DEAD
		&& newHealth <= 0)
	{
		healthBarWidget->DestroyComponent();
		DeactivateAI();
		SetWorldState(CharacterWorldState::DYING);
		PlayAnimMontage(ResourceManager::Get<UAnimMontage>("animFallingDead"));
	}
}

void AManTrooperChar::HandleOnMontageEnded(UAnimMontage* Montage, bool Interrupted)
{
	if (!Interrupted && Montage->GetName() == "man_Anim_ThrownBack_Montage")
	{
		PlayAnimMontage(ResourceManager::Get<UAnimMontage>("animGettingUp"));
	}
	else if (!Interrupted && Montage->GetName() == "man_Anim_FallingDead_Montage")
	{
		SetWorldState(CharacterWorldState::DEAD);
	}
	else if (Montage->GetName() == "man_Anim_GettingReadyToThrow_Montage")
	{
		if (shootingState == CharacterShootingState::START_AIMING)
		{
			OnCharacterIsDoneAiming();
		}
	}
}

UWeapon_ActorComponent* AManTrooperChar::GetWeaponComponent()
{
	return weaponComponent;
}

void AManTrooperChar::SwitchToMeleeWeapon()
{
	if (weaponComponent->IsMelee())
	{
		return;
	}
	SetShootingState(CharacterShootingState::NOT_AIMING);
	weaponComponent->SetVisibility(true);

	shootingComponent->DestroyComponent();
	weaponComponent->SetMeleeWeaponType("MeleeSimpleSword");
	weaponComponent->SetRelativeRotation(FRotator(0.0f, 0.0f, 0.0f));
}
void AManTrooperChar::SwitchToRangeWeapon()
{
	if (!weaponComponent->IsMelee())
	{
		return;
	}
	shootingComponent = NewObject<UProjectileShootingComponent>(this);
	shootingComponent->Init(this, weaponComponent);
	shootingComponent->RegisterComponent();

	weaponComponent->SetBulletType("ManSimpleSpear");
}