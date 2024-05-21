#include "Aiming/ProjectileShootingComponent.h"
#include "Aiming/ProjectileWeaponAiming.h"
#include "Bullets/GameBulletActor.h"
#include "Kismet/GameplayStatics.h"
#include "Bullets/BulletsManager.h"
#include "GameFramework/Character.h"
#include "Character/Weapon_ActorComponent.h"
#include "Catapult/CatapultGun.h"

#include "Player/PlayerCatapult.h"
#include "Player/PlayerCharacter.h"
#include "Player/InventoryActorComponent.h"
#include "Misc/PickableObject.h"

#include "ResourceManager.h"

#define AIM_VELOCITY_SPEED	50.0f

UProjectileShootingComponent::UProjectileShootingComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	bulletsByShot = 1;
	isCatapultGun = false;
	pullingGunBack = false;
	launchBulletsByOne = false;
	loaded = true;
	reloadingTime = 1.5f; // time in s 1 sec
	aiming = NULL;

	maxVelocity = minVelocity = 0.0f;

	minAimTension = -60.0f;
	maxAimTension = 30.0f;
	currentVelocity = 1600.0f;
	pullingGunSpeed = 0.3f;
	releasingGunSpeed = 10.0f;

	aimTensionValue = (abs(minAimTension) + abs(maxAimTension)) / 2.0f;

	playerInventory = NULL;
	playerCatapultPtr = NULL;
	playerCharacterPtr = NULL;
}

void UProjectileShootingComponent::Init(APawn* parentActor, UWeapon_ActorComponent* weaponComponentPtr)
{
	this->weaponComponent = weaponComponentPtr;
	this->parentActorPtr = parentActor;
	aiming = NewObject<UProjectileWeaponAiming>(this);
	aiming->Init(this);
	aiming->RegisterComponent();

	isNPC = !(parentActorPtr->IsA(APlayerCatapult::StaticClass()) || parentActorPtr->IsA(APlayerCharacter::StaticClass()));



	if (!isNPC)
	{
		if (parentActorPtr->IsA(APlayerCatapult::StaticClass()))
		{
			playerCatapultPtr = Cast<APlayerCatapult>(parentActorPtr);
			playerInventory = playerCatapultPtr->GetInventory();
		}
		else if (parentActorPtr->IsA(APlayerCharacter::StaticClass()))
		{
			playerCharacterPtr = Cast<APlayerCharacter>(parentActorPtr);
			playerInventory = playerCharacterPtr->GetInventory();
		}
	}
}

void UProjectileShootingComponent::OnComponentDestroyed(bool bDestroyingHierarchy)
{
	aiming->DestroyComponent();
}

void UProjectileShootingComponent::SetVelocity(float val)
{
	currentVelocity = val;
}


void UProjectileShootingComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UProjectileShootingComponent::OnMouseMoveY(const float Value)
{

	FRotator currentControllerRotation = parentActorPtr->GetController()->GetControlRotation();
	currentControllerRotation.Normalize();

	UE_LOG(LogTemp, Warning, TEXT("The float value is: %f"), currentControllerRotation.Pitch);

	currentControllerRotation.Pitch += Value;

	if (currentControllerRotation.Pitch < minAimTension)
	{
		parentActorPtr->GetController()->SetControlRotation(FRotator(minAimTension, currentControllerRotation.Yaw, currentControllerRotation.Roll));
	}
	else if (currentControllerRotation.Pitch > maxAimTension)
	{
		parentActorPtr->GetController()->SetControlRotation(FRotator(maxAimTension, currentControllerRotation.Yaw, currentControllerRotation.Roll));
	}
	else
	{
		parentActorPtr->AddControllerPitchInput(-Value);
	}

	if(!useArcAim) aimTensionValue = currentControllerRotation.Pitch + abs(minAimTension);

}

void UProjectileShootingComponent::SetMinAndMaxAimTension(float minT, float maxT)
{
	minAimTension = minT;
	maxAimTension = maxT;
	if (!useArcAim) aimTensionValue = (abs(minAimTension) + abs(maxAimTension)) / 2.0f;
	else aimTensionValue = minAimTension;
}

void UProjectileShootingComponent::SetMinAndMaxVelocity(float minT, float maxT)
{
	minVelocity = minT;
	maxVelocity = maxT;
}

float UProjectileShootingComponent::GetAimPitch()
{
	return aimTensionValue;
}

void UProjectileShootingComponent::SetAimPitch(float newValue)
{
	aimTensionValue = newValue;
}

void UProjectileShootingComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);


	if (!loaded)
	{
		shotTime += DeltaTime;
		loaded = shotTime >= reloadingTime;
		if (loaded)
		{
			bulletsLeftBeforeReloading = bulletsByShot;
			if (isCatapultGun)
			{
				catapultActorPtr->OnGunLoaded();
				if(!isNPC) ResetBulletsOnGun();
			}
		}
	}


	if (state == ProjectileGunState::RELEASING_GUN)
	{
		RelasingGun(DeltaTime);
	}
	else if (pullingGunBack)
	{
		OnPullingGunBackTo(DeltaTime);
	}
	else if (state == ProjectileGunState::PULLING_GUN)
	{
		OnPullingGun(DeltaTime);
	}
	else if (state == ProjectileGunState::UNPULLING_GUN)
	{
		OnUnpullingGun(DeltaTime);
	}

}

void UProjectileShootingComponent::PullGun()
{
	if (state == ProjectileGunState::RELEASING_GUN) return;
	SetState(ProjectileGunState::PULLING_GUN);
}

void UProjectileShootingComponent::UnPullGun()
{
	if (state == ProjectileGunState::RELEASING_GUN) return;
	SetState(ProjectileGunState::UNPULLING_GUN);
}

void UProjectileShootingComponent::SetUseArcAim(bool v, float ang)
{
	useArcAim = v;
	arcDegrees = ang;
}

void UProjectileShootingComponent::OnPullingGun(float dt)
{
	aimTensionValue += pullingGunSpeed * dt;
	aimTensionValue = aimTensionValue > maxAimTension ? maxAimTension : aimTensionValue;
}

void UProjectileShootingComponent::OnUnpullingGun(float dt)
{
	aimTensionValue -= pullingGunSpeed * dt;
	aimTensionValue = aimTensionValue <= 0.0f ? 0.0f : aimTensionValue;
}

void UProjectileShootingComponent::StopPullingGunBack()
{
	pullingGunBack = false;
	pullingGunBackTo = 0.0f;
}

void UProjectileShootingComponent::OnPullingGunBackTo(float dt)
{
	aimTensionValue += pullingGunSpeed * dt;
	aimTensionValue = aimTensionValue > pullingGunBackTo ? pullingGunBackTo : aimTensionValue;
	if (aimTensionValue == pullingGunBackTo)
	{
		StopPullingGunBack();
	}
}

void UProjectileShootingComponent::RelasingGun(float dt)
{
	aimTensionValue -= releasingGunSpeed * dt;
	if (aimTensionValue <= 0)
	{
		Shot();
		aimTensionValue = 0;
	}
}

bool UProjectileShootingComponent::IsLoaded()
{
	return loaded;
}


bool UProjectileShootingComponent::Shot()
{
	if (!IsLoaded()) return false;

	int bulletsCount = -5;

	//Check if player has enought bullets
	if (!isNPC)
	{
		bulletsCount = playerInventory->GetBulletsCount(weaponComponent->GetBulletType());
		if (bulletsCount != -5.0f && bulletsCount <= 0) return false;
	}

	float velocity = GetBulletInitialVelocity();
	FVector forwardVec = GetForward();

	const BulletData& bd = BulletsManager::GetBulletData(weaponComponent->GetBulletType());

	bool readyToReload = false;
	int bulletsToRemove = 1;

	UClass* bpClass = ResourceManager::Get<UClass>(bd.bpName);
	if (bpClass)
	{


		if (isNPC) // give some random num
		{
			velocity += FMath::RandRange(-50.0f, 500.0f);
		}

		FVector finalVel = velocity * forwardVec;


		if (launchBulletsByOne)
		{
			FVector bulletStartPos = weaponComponent->GetBulletStartPositions()[0];

			SpawnBullet(bd, bpClass, finalVel, bulletStartPos, forwardVec);

			bulletsLeftBeforeReloading -= 1;
			bulletsLeftBeforeReloading = bulletsLeftBeforeReloading < 0 ? 0 : bulletsLeftBeforeReloading;

			if (isCatapultGun)
			{
				catapultActorPtr->ShiftBullets(bulletsLeftBeforeReloading);
			}

			if (bulletsLeftBeforeReloading <= 0)
			{
				readyToReload = true;
			}
		}
		else if (bulletsByShot > 1)
		{
			const std::vector<FVector>& bulletStartPos = weaponComponent->GetBulletStartPositions();

			int bulletIndex = 1;
			for (auto& bst : bulletStartPos)
			{
				if (bulletsCount != -5 && bulletIndex > bulletsCount) break;
				SpawnBullet(bd, bpClass, finalVel, bst, forwardVec);
				bulletIndex++;
			}
			readyToReload = true;
			bulletsToRemove = bulletsByShot;
		}
		else
		{
			if (weaponComponent->IsPickableObject())
			{
				ThrowPickableObject(finalVel, GetBulletStartPosition(), forwardVec);
			}
			else
			{
				SpawnBullet(bd, bpClass, finalVel, GetBulletStartPosition(), forwardVec);
			}
			readyToReload = true;
		}

	}


	//Remove bullets from inventory
	if (!isNPC)
	{
		playerInventory->RemoveCount(bd.type, bulletsToRemove);
		if(playerCatapultPtr) playerCatapultPtr->OnBulletCountChanged();
	}

	if (readyToReload)
	{
		loaded = false;
		shotTime = 0.0f;
		if (useArcAim)
		{
			if(!isNPC) StartPullingGunBack();
		}
		SetState(ProjectileGunState::RELOADING);
	}
	else
	{
		SetState(ProjectileGunState::READY);
	}

	return true;
}

void UProjectileShootingComponent::ThrowPickableObject(const FVector& vel, const FVector& startPos, const FVector& forward)
{
	APickableObject* obj = weaponComponent->GetHoldingPickableObject();
	obj->OnThrow(vel, parentActorPtr, forward);
}

void UProjectileShootingComponent::SpawnBullet(const BulletData& bd, UClass* bpClass, const FVector& vel, const FVector& startPos, const FVector& forward)
{
	FTransform SpawnTransform(FRotator(0, 0, 0), startPos);
	AGameBulletActor* bulletActor = GetWorld()->SpawnActorDeferred<AGameBulletActor>(bpClass, SpawnTransform);
	if (bulletActor != nullptr)
	{
		bulletActor->Init(bd, vel, parentActorPtr, forward);
		UGameplayStatics::FinishSpawningActor(bulletActor, SpawnTransform);
	
		if (!isNPC)
		{
			aiming->AddActorToIgnore(bulletActor);
			bulletActor->OnBulletRemove.AddDynamic(this, &UProjectileShootingComponent::OnBulletRemoved);
		}
	}
}

void UProjectileShootingComponent::SetState(ProjectileGunState newState)
{
	state = newState;
}

ProjectileGunState UProjectileShootingComponent::GetState()
{
	return state;
}

const float& UProjectileShootingComponent::GetMaxAimTension()
{
	return maxAimTension;
}
const float& UProjectileShootingComponent::GetMinAimTension()
{
	return minAimTension;
}
const float& UProjectileShootingComponent::GetMaxVelocity()
{
	return maxVelocity;
}
const float& UProjectileShootingComponent::GetMinVelocity()
{
	return minVelocity;
}
const FVector UProjectileShootingComponent::GetBulletStartPosition()
{
	if (launchBulletsByOne)
	{
		return weaponComponent->GetBulletStartPositions()[0];
	}
	return weaponComponent->GetBulletStartPosition();
}
const FBoxSphereBounds UProjectileShootingComponent::GetBulletSize()
{
	return weaponComponent->GetBulletSize();
}
FVector UProjectileShootingComponent::GetFront()
{
	return parentActorPtr->GetActorRotation().Vector();
}

FVector UProjectileShootingComponent::GetForward()
{
	FRotator v = parentActorPtr->GetController() ? parentActorPtr->GetController()->GetControlRotation() : FRotator();
	v.Pitch = minAimTension + aimTensionValue;

	FVector forward = v.Vector();

	if (isCatapultGun)
	{
		if (hasRotator)
		{
			forward = catapultActorPtr->GetRotatorForward();
		}
		else
		{
			forward = parentActorPtr->GetActorForwardVector();
		}

		if (useArcAim)
		{
			forward = forward.RotateAngleAxis(-arcDegrees, FVector(-forward.Y, forward.X, 0.0f));
		}
		else
		{
			v = catapultActorPtr->GetActorRotation();
			v.Pitch = minAimTension + aimTensionValue;
			forward = v.Vector();
		}
	}

	return forward;
}

float UProjectileShootingComponent::GetBulletInitialVelocity()
{
	if (useArcAim)
	{
		return (maxVelocity - minVelocity) * GetTensionPrc() + minVelocity;
	}
	return currentVelocity;
}

UProjectileWeaponAiming* UProjectileShootingComponent::GetAiming()
{
	return aiming;
}
APawn* UProjectileShootingComponent::GetParentActorPtr()
{
	return parentActorPtr;
}


void UProjectileShootingComponent::StopPullingGun()
{
	if (state == ProjectileGunState::RELEASING_GUN) return;
	SetState(loaded ? ProjectileGunState::READY : ProjectileGunState::RELOADING);
}

float UProjectileShootingComponent::GetTensionPrc()
{
	return (aimTensionValue > 0 ? aimTensionValue : shotingTension) / maxAimTension;
}


void UProjectileShootingComponent::SetPullingGunSpeed(float v)
{
	pullingGunSpeed = v;
}
void UProjectileShootingComponent::SetReleasingGunSpeed(float v)
{
	releasingGunSpeed = v;
}

bool UProjectileShootingComponent::StartArcShot()
{
	if (!IsLoaded()) return false;

	//Check if player has enought bullets
	if (!isNPC)
	{
		int bulletsCount = playerInventory->GetBulletsCount(weaponComponent->GetBulletType());
		if (bulletsCount != -5.0f && bulletsCount <= 0) return false;
	}

	if (GetState() == ProjectileGunState::RELEASING_GUN) return false;
	shotingTension = aimTensionValue;
	SetState(ProjectileGunState::RELEASING_GUN);
	return true;
}

void UProjectileShootingComponent::StartPullingGunBack()
{
	pullingGunBack = true;
	pullingGunBackTo = shotingTension;
	shotingTension = 0.0f;
}

void UProjectileShootingComponent::SetLaunchBulletsByOne(bool v)
{
	launchBulletsByOne = v;
	bulletsLeftBeforeReloading = bulletsByShot;
}
void UProjectileShootingComponent::SetBulletsByShot(int n)
{
	bulletsByShot = n;
}
void UProjectileShootingComponent::SetIsCatapultGun(bool v, ACatapultGun* catapultActor)
{
	catapultActorPtr = catapultActor;
	isCatapultGun = v;
}

void UProjectileShootingComponent::SetHasRotator(bool v)
{
	hasRotator = v;
}

void UProjectileShootingComponent::OnBulletRemoved(AActor* actor)
{
	aiming->RemoveActorToIgnore(actor);
}
float UProjectileShootingComponent::GetReloadingTime()
{
	return reloadingTime;
}

void UProjectileShootingComponent::ResetBulletsOnGun()
{
	int bulletsCount = playerInventory->GetBulletsCount(weaponComponent->GetBulletType());
	bulletsLeftBeforeReloading = bulletsByShot;
	int showCount = -1;
	if (bulletsCount != -5 && bulletsCount < bulletsByShot)
	{
		showCount = bulletsCount;
		bulletsLeftBeforeReloading = bulletsCount;
	}

	catapultActorPtr->ShowBullets(showCount);
}