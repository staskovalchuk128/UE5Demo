#include "Bullets/GameBulletActor.h"
#include "Bullets/BulletsManager.h"
#include "ResourceManager.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/CapsuleComponent.h"

#include "Player/PlayerCharacter.h"
#include "Player/PlayerCatapult.h"

#include "NPC/ManTrooperChar.h"

#include "CommonFunctions.h"

// Sets default values
AGameBulletActor::AGameBulletActor()
{

	lifeTimeAfterCollision = 1.5f;
	lifeTimeAfterCollisionPast = 0.0f;
	collided = false;
	needRemoveImmediately = false;

 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bulletMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>("bulletMeshComp");
	SetRootComponent(bulletMeshComponent);

	bulletMeshComponent->SetNotifyRigidBodyCollision(true);
	//SetActorLabel("Bullet");


	projMoveComp = CreateDefaultSubobject<UProjectileMovementComponent>("projMove");
	projMoveComp->bRotationFollowsVelocity = true;

}


void AGameBulletActor::Init(const BulletData& bulletData, FVector bf, APawn* ownerPtr, const FVector& forwardVec)
{
	this->ownerActor = ownerPtr;
	useProjectveMove = bulletData.useProjectiveMove;


	// ignore self actor collision
	{
		ownerPtr->MoveIgnoreActorAdd(this);
		bulletMeshComponent->MoveIgnoreActors.Add(ownerPtr);
	}

	if (useProjectveMove)
	{
		projMoveComp->InitialSpeed = bf.Size();
		projMoveComp->MaxSpeed = projMoveComp->InitialSpeed;
		projMoveComp->Velocity = forwardVec;
	}
	else
	{
		projMoveComp->DestroyComponent();
		bulletMeshComponent->SetSimulatePhysics(true);
	}


	this->bulletForce = bf * bulletData.mass;
	FRotator rot = forwardVec.Rotation();
	bulletMeshComponent->SetMassOverrideInKg(NAME_None, bulletData.mass, true);
	damage = bulletData.damage;
	heavyBullet = bulletData.mass > 100.0f;

}

// Called when the game starts or when spawned
void AGameBulletActor::BeginPlay()
{
	Super::BeginPlay();

	if (!physicsDisabled && !useProjectveMove && bulletForce.Size() > 0)
	{
		bulletMeshComponent->AddImpulse(bulletForce);
	}
	bulletMeshComponent->OnComponentHit.AddUniqueDynamic(this, &AGameBulletActor::OnHit);
	bulletMeshComponent->OnComponentBeginOverlap.AddUniqueDynamic(this, &AGameBulletActor::OnOverlapBegin);
}

void AGameBulletActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (collided || needRemoveImmediately)
	{
		lifeTimeAfterCollisionPast += DeltaTime;
		if (lifeTimeAfterCollisionPast >= lifeTimeAfterCollision || needRemoveImmediately)
		{
			//ownerActor->MoveIgnoreActorRemove(this);
			OnBulletRemove.Broadcast(this);
			Destroy();
		}
	}

}

int AGameBulletActor::GetDamage()
{
	return damage;
}

bool AGameBulletActor::CanDamage()
{
	return damage > 0;
}

void AGameBulletActor::RemoveDamage()
{
	damage = 0;
}

bool AGameBulletActor::IsHeavyBullet()
{
	return heavyBullet;
}

void AGameBulletActor::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (useProjectveMove)
	{

		if (OtherActor->IsA(APlayerCharacter::StaticClass()) || OtherActor->IsA(AManTrooperChar::StaticClass()))
		{
			USkeletalMeshComponent* characterMesh = NULL;
			UCapsuleComponent* capsuleComponent = NULL;
			if (OtherActor->IsA(APlayerCharacter::StaticClass()))
			{
				characterMesh = Cast<APlayerCharacter>(OtherActor)->GetMesh();
				capsuleComponent = Cast<APlayerCharacter>(OtherActor)->GetCapsuleComponent();
			}
			else
			{
				characterMesh = Cast<AManTrooperChar>(OtherActor)->GetMesh();
				capsuleComponent = Cast<AManTrooperChar>(OtherActor)->GetCapsuleComponent();
			}

			FName closestSocket = CommonFunctions::GetClosestSocket(characterMesh, SweepResult.Location);
			
			FVector tranceNorm = (SweepResult.TraceStart - SweepResult.TraceEnd);
			tranceNorm.Normalize();
			FVector exTrace = SweepResult.Location + (tranceNorm * -250.0f);

			FCollisionQueryParams CollisionParams;
			FName prevMeshProfile = characterMesh->GetCollisionProfileName();
			characterMesh->SetCollisionProfileName("Pawn");

		//	DrawDebugLine(GetWorld(), SweepResult.Location, exTrace, FColor::Green, false, 1.0, 5);

			FHitResult OutHit;
			bool meshCollided = characterMesh->LineTraceComponent(OutHit, SweepResult.Location, exTrace, CollisionParams);

			characterMesh->SetCollisionProfileName(prevMeshProfile);

			if (!meshCollided) return;

			AddActorWorldOffset(tranceNorm * -50.0f);
			AttachToComponent(characterMesh, FAttachmentTransformRules::KeepWorldTransform, closestSocket);

			
			capsuleComponent->OnComponentHit.Broadcast(OtherComp, this, OverlappedComp, FVector(), SweepResult);
			
		}
		else
		{
			bulletMeshComponent->AttachToComponent(OtherActor->GetRootComponent(), FAttachmentTransformRules::KeepWorldTransform);
		}
		

		projMoveComp->StopMovementImmediately();

		projMoveComp->bSweepCollision = false;
		bulletMeshComponent->SetCollisionProfileName("NoCollision");

		bulletMeshComponent->SetSimulatePhysics(false);

		projMoveComp->Deactivate();
	}

	collided = true;
}

void AGameBulletActor::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (useProjectveMove)
	{
		AttachToActor(OtherActor, FAttachmentTransformRules::KeepWorldTransform);

		projMoveComp->StopMovementImmediately();

		projMoveComp->bSweepCollision = false;
		bulletMeshComponent->SetCollisionProfileName("NoCollision");
		bulletMeshComponent->AttachToComponent(OtherActor->GetRootComponent(), FAttachmentTransformRules::KeepWorldTransform);


		bulletMeshComponent->SetSimulatePhysics(true);
	}
	collided = true;
}


UStaticMeshComponent* AGameBulletActor::GetBulletMeshComponent()
{
	return bulletMeshComponent;
}
void AGameBulletActor::EnablePhysics()
{
	physicsDisabled = false;
	projMoveComp->bSimulationEnabled = true;
	bulletMeshComponent->SetSimulatePhysics(true);
}
void AGameBulletActor::DisablePhysics()
{
	physicsDisabled = true;
	projMoveComp->bSimulationEnabled = false;
	bulletMeshComponent->SetSimulatePhysics(false);
}

FVector AGameBulletActor::GetApplyedForce()
{
	return bulletForce;
}

bool AGameBulletActor::IsCollided()
{
	return collided;
}

void AGameBulletActor::RemoveImmediately()
{
	needRemoveImmediately = true;
}