#include "Misc/PickableObject.h"
#include "Components/BillboardComponent.h"
#include "ResourceManager.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/ArrowComponent.h"

#include "Player/PlayerCharacter.h"
#include "Kismet/GameplayStatics.h"

#include "Misc/OutputDeviceNull.h"

#include "string"

APickableObject::APickableObject()
{
	needToRemove = false;
	respawnAfterDestory = true;
	enablePhysicsAfterPlaced = false;
	removeTime = 1.5f; // 1.5 sec
	collidedTimer = 0.0f;


	mainMesh = CreateDefaultSubobject<UStaticMeshComponent>("mainMesh");
	mainMesh->SetNotifyRigidBodyCollision(true);
	SetRootComponent(mainMesh);

	axisToSwitchWhenRotate = FVector(1.0f, 1.0f, 1.0f);

	PrimaryActorTick.bCanEverTick = true;
	pickIconBillboard = CreateDefaultSubobject<UBillboardComponent>("pickIconBillboard");
	UTexture2D* pickTexture = ResourceManager::Get<UTexture2D>("texture_InteractWithObject");
	pickIconBillboard->SetSprite(pickTexture);

	type = PickableObjectType::THROWABLE;
	pickAnimation = ResourceManager::Get<UAnimMontage>("animPickUpObject");
	carryAnimation = ResourceManager::Get<UBlendSpace>("anim_BS_RunDefault");

	useNormalWhenPlace = true;
}

void APickableObject::BeginPlay()
{
	pickedRelativeScale = FVector(1.0f);

	Super::BeginPlay();

	
	FAttachmentTransformRules rules(EAttachmentRule::KeepRelative, true);
	pickIconBillboard->AttachToComponent(mainMesh, rules);

	mainMesh->SetMassOverrideInKg(NAME_None, 20.0f, true);

	pickIconBillboard->SetHiddenInGame(false);


	//Save for respawning
	{
		beginLocation = GetActorLocation();
		beginRotation = GetActorRotation();
		beginScale = GetActorScale();
	}

}

void APickableObject::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (needToRemove)
	{
		collidedTimer += DeltaTime;

		if (collidedTimer >= removeTime) // Destory actor
		{
			// Respawn 
			if (respawnAfterDestory)
			{
				FTransform spawnTransform(beginRotation, beginLocation, beginScale);
				AActor* sameActor = GetWorld()->SpawnActorDeferred<AActor>(GetClass(), spawnTransform);
				if (sameActor != nullptr)
				{
					UGameplayStatics::FinishSpawningActor(sameActor, spawnTransform);
				}
			}

			Destroy();
		}
		
	}

}

bool APickableObject::CanPick()
{
	return playerCanPick;
}

PickableObjectType APickableObject::GetType()
{
	return type;
}
UBlendSpace* APickableObject::GetCarryAnimation()
{
	return carryAnimation;
}
UAnimMontage* APickableObject::GetPickAnimataion()
{
	return pickAnimation;
}

UStaticMeshComponent* APickableObject::GetMainMesh()
{
	return mainMesh;
}

FVector APickableObject::GetPlacedRelativeLocation()
{
	return placedRelativeLocation;
}

FVector APickableObject::GetPlacedOnGroundRelativeLocation()
{
	return placedOnGroundRelativeLocation;
}

FVector APickableObject::GetAxisToSwitchWhenRotate()
{
	return axisToSwitchWhenRotate;
}

FRotator APickableObject::GetPlacedRelativeRotation()
{
	return placedRelativeRotation;
}

FRotator APickableObject::GetPlacedOnGroundRelativeRotation()
{
	return placedOnGroundRelativeRotation;
}

bool APickableObject::ShouldUseNormalWhenPlace()
{
	return useNormalWhenPlace;
}
bool APickableObject::ShouldEnablePhysicsAfterPlaced()
{
	return enablePhysicsAfterPlaced;
}


void APickableObject::OnObjectPicked(UChildActorComponent* obj, APawn* ownerPawn)
{
	if (!IsValid(this)) return;


	objectHolderPtr = obj;
	ownerPawnPtr = ownerPawn;
	picked = true;
	playerCanPick = false;
	pickIconBillboard->SetVisibility(false);

	FOutputDeviceNull nullDev;
	CallFunctionByNameWithArguments(TEXT("OnPickedup"), nullDev, nullptr, true);

	ResetPositionToPlayer();

	// ignore self actor collision
	{
		ownerPawn->MoveIgnoreActorAdd(this);
		mainMesh->MoveIgnoreActors.Add(ownerPawn);
	}

	mainMesh->OnComponentHit.AddUniqueDynamic(this, &APickableObject::OnHit);


	prevCollisionProfile = mainMesh->GetCollisionProfileName();
	prevCollisionEnabled = mainMesh->GetCollisionEnabled();

	mainMesh->SetCollisionProfileName("NoCollision");
}

void APickableObject::ResetPositionToPlayer()
{
	mainMesh->SetRelativeLocation(FVector(0.0f));
	mainMesh->SetRelativeRotation(FRotator(0.0f, 0.0f, 0.0f));

	objectHolderPtr->SetRelativeLocation(pickedRelativeLocation);
	objectHolderPtr->SetRelativeRotation(pickedRelativeRotation);
	objectHolderPtr->SetRelativeScale3D(pickedRelativeScale);
}

void APickableObject::OnObjectLost()
{

	mainMesh->SetCollisionProfileName(prevCollisionProfile);
	mainMesh->SetCollisionEnabled(prevCollisionEnabled);

	picked = false;
	if (ownerPawnPtr)
	{
		ownerPawnPtr->MoveIgnoreActorRemove(this);
		mainMesh->MoveIgnoreActors.Remove(ownerPawnPtr);
	}
}

void APickableObject::OnThrow(FVector velocity, APawn* ownerPtr, const FVector& forwardVec)
{
	FOutputDeviceNull nullDev;
	CallFunctionByNameWithArguments(TEXT("OnBeginThrow"), nullDev, nullptr, true);

	DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);

	projMoveComp = NewObject<UProjectileMovementComponent>(this);
	projMoveComp->bRotationFollowsVelocity = true;

	projMoveComp->InitialSpeed = velocity.Size();
	projMoveComp->MaxSpeed = projMoveComp->InitialSpeed;
	projMoveComp->Velocity = forwardVec;
	mainMesh->SetRelativeRotation(FRotator(0.0f, 0.0f, 0.0f));
	projMoveComp->RegisterComponent();

}



void APickableObject::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (type == PickableObjectType::THROWABLE && projMoveComp)
	{

		projMoveComp->StopMovementImmediately();
		projMoveComp->bSweepCollision = false;
		projMoveComp->DestroyComponent();

		mainMesh->SetSimulatePhysics(true);

		needToRemove = true;
	}
}

void APickableObject::SetPlayerCanPick(bool v)
{
	playerCanPick = v;

	if (playerCanPick)
	{
		pickIconBillboard->SetVisibility(true);
	}
	else
	{
		pickIconBillboard->SetVisibility(false);
	}
}

