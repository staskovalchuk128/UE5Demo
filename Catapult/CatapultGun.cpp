#include "Catapult/CatapultGun.h"
#include "Catapult/CatapultTypes.hpp"
#include "Bullets/BulletsManager.h"
#include "Bullets/GameBulletActor.h"
#include "Kismet/GameplayStatics.h"
#include "PhysicsEngine/PhysicsSettings.h"
#include "Engine/StaticMeshSocket.h"
#include "Player/PlayerCatapult.h"
#include "Player/InventoryActorComponent.h"
#include "Character/Weapon_ActorComponent.h"

#include "Catapult/CatapultAiming.h"

#include "ResourceManager.h"

ACatapultGun::ACatapultGun()
{
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>("Root");

	gunMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>("catapultGun");
	gunMeshComponent->SetCollisionProfileName(FName("NoCollision"));
	
	actorWeaponData = CreateDefaultSubobject<UWeapon_ActorComponent>("Weapon");

}

void ACatapultGun::Init(APawn* parentActor, USkeletalMeshComponent* parentMC, const FString& gd, bool hasRotator)
{
	this->gunType = gd;
	this->gunData = CatapultGunsManager::GetGunData(gd);
	// setup gun prop
	{
		bulletsLeftBeforeReloading = gunData.bulletsNum;
	}
	
	SetActorRelativeLocation(gunData.relativeOffset);

	this->parentMeshComponent = parentMC;
	this->parentActorPtr = parentActor;


	aiming = NewObject<UCatapultAiming>(this);
	aiming->Init(this);
	aiming->RegisterComponent();

	FAttachmentTransformRules rules(EAttachmentRule::KeepRelative, true);
	AttachToComponent(parentMC, rules, "GunSocket");
	gunMeshComponent->AttachToComponent(RootComponent, rules);

	for (int i = 0; i < gunData.bulletsNum; i++)
	{
		UChildActorComponent* bc = NewObject<UChildActorComponent>(gunMeshComponent);
		bool r = bc->AttachToComponent(gunMeshComponent, rules, gunData.bulletsSockets[i]);
		bulletComponents.Add(bc);
	}

	constBulletStartPositions.resize(gunData.bulletsNum);
	bulletStartPositions.resize(gunData.bulletsNum);

	actorWeaponData->GetBulletStartPositionFromSocket(false);

	if (parentActor)
	{
		shootingComponent = NewObject<UProjectileShootingComponent>(this);
		shootingComponent->Init(parentActor, actorWeaponData);
		shootingComponent->RegisterComponent();

		if (!gunData.isProjectile)
		{
			shootingComponent->SetUseArcAim(true, 45.0f);
			shootingComponent->SetMinAndMaxAimTension(0.0f, 40.0f);
			shootingComponent->SetMinAndMaxVelocity(450.0f, 1500.0f);
		}
		else
		{
			shootingComponent->SetMinAndMaxAimTension(-10.0f, 45.0f);
			shootingComponent->SetAimPitch(0.0f);
		}
		shootingComponent->SetPullingGunSpeed(gunData.pullingGunSpeed);
		shootingComponent->SetReleasingGunSpeed(gunData.releasingGunSpeed);
		shootingComponent->SetBulletsByShot(gunData.bulletsNum);
		shootingComponent->SetIsCatapultGun(true, this);
		shootingComponent->SetHasRotator(hasRotator);

		if (gunData.launchBulletsByOne)
		{
			shootingComponent->SetLaunchBulletsByOne(true);
		}

		SetBulletType("CatapultRock");
	}
	
}

void ACatapultGun::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!shootingComponent) return;

	gunMeshComponent->SetRelativeRotation(FRotator(shootingComponent->GetAimPitch(), 0.0f, 0.0f));

	for (int i = 0; i < gunData.bulletsNum; i++)
	{
		bulletStartPositions[i] = bulletComponents[i]->GetSocketLocation(gunData.bulletsSockets[i]);
	}


	{
		FVector sum(0.0f);
		if (bulletStartPositions.size() == 0)
		{
			bulletStartPosition = sum;
		}
		else
		{
			for (auto& vec : bulletStartPositions)
			{
				sum += vec;
			}
			bulletStartPosition = sum / (float)bulletStartPositions.size();
		}

	}

	{
		FVector sum(0.0f);
		if (constBulletStartPositions.size() == 0)
		{
			constBulletStartPosition = sum;
		}
		else
		{
			for (auto& vec : constBulletStartPositions)
			{
				sum += vec;
			}
			constBulletStartPosition = sum / (float)constBulletStartPositions.size();
		}

	}

	actorWeaponData->SetBulletStartPosition(bulletStartPosition);
	actorWeaponData->SetBulletStartPositions(bulletStartPositions);
}

ProjectileGunState ACatapultGun::GetState() const
{
	return shootingComponent->GetState();
}

UProjectileWeaponAiming* ACatapultGun::GetAiming()
{
	return shootingComponent->GetAiming();
}

void ACatapultGun::SetGunTension(float t)
{
	return shootingComponent->SetAimPitch(t);
}

void ACatapultGun::PullGun()
{
	shootingComponent->PullGun();
}
void ACatapultGun::UnPullGun()
{
	shootingComponent->UnPullGun();
}

void ACatapultGun::StopPullingGun()
{
	shootingComponent->StopPullingGun();
}

const FVector ACatapultGun::GetBulletStartPosition()
{
	return shootingComponent->GetBulletStartPosition();
}

const FVector ACatapultGun::GetConstBulletStartPosition()
{
	return shootingComponent->GetBulletStartPosition();
}

float ACatapultGun::GetBulletMaxVelocity()
{
	return shootingComponent->GetMaxVelocity();
}

float ACatapultGun::GetBulletMinVelocity()
{
	return shootingComponent->GetMinVelocity();
}

float ACatapultGun::GetMaxGunTension()
{
	return shootingComponent->GetMaxAimTension();
}

float ACatapultGun::GetGunTension()
{
	return shootingComponent->GetAimPitch();
}

float ACatapultGun::GetTensionPrc()
{
	return shootingComponent->GetTensionPrc();
}

void ACatapultGun::BeginPlay()
{
	Super::BeginPlay();
}


bool ACatapultGun::Shot()
{
	if (gunData.releasingGunSpeed == -1.0f)
	{
		return shootingComponent->Shot();
	}
	else
	{
		return shootingComponent->StartArcShot();
	}
	return true;
}
bool ACatapultGun::IsLoaded()
{
	return shootingComponent->IsLoaded();
}

void ACatapultGun::SetBulletType(FString newType)
{
	actorWeaponData->SetBulletType(newType, false);

	const BulletData& bulletData = BulletsManager::GetBulletData(newType);

	UClass* bpClass = ResourceManager::Get<UClass>(bulletData.bpName);
	if (bpClass)
	{
		for (auto& it : bulletComponents)
		{
			it->SetChildActorClass(bpClass);
			it->CreateChildActor();
			AGameBulletActor* act = Cast<AGameBulletActor>(it->GetChildActor());
			if (act)
			{
				act->AttachToComponent(it, FAttachmentTransformRules::KeepRelativeTransform);
				act->GetBulletMeshComponent()->SetCollisionProfileName("NoCollision");
				act->DisablePhysics();
			}
		}


		FBoxSphereBounds bounds;
		for (int i = 0; i < gunData.bulletsNum; i++)
		{
			AGameBulletActor* gba = Cast<AGameBulletActor>(bulletComponents[0]->GetChildActor());
			if (!gba) continue;
			bounds.SphereRadius += gba->GetBulletMeshComponent()->GetStaticMesh()->GetBounds().SphereRadius;
			if (gunData.launchBulletsByOne) break; //Just get size of one bullet
		}
		actorWeaponData->SetBulletSize(bounds);
	}
}

FString ACatapultGun::GetBulletType()
{
	return actorWeaponData->GetBulletType();
}

void ACatapultGun::ShiftBullets(int bulletIndex)
{
	if (bulletIndex < 0 || bulletIndex > bulletComponents.Num() - 1) return;
	bulletComponents[bulletIndex]->SetVisibility(false);
}
void ACatapultGun::OnGunLoaded()
{
}
void ACatapultGun::ShowBullets(int count)
{
	int index = 1;
	for (auto& it : bulletComponents)
	{
		if (count == -1)
		{
			it->SetVisibility(true);
		}
		else if (index > count)
		{
			it->SetVisibility(false);
		}
		else
		{
			it->SetVisibility(true);
		}
		index++;
	}
}

void ACatapultGun::SetMinAndMaxVelocity(float minv, float maxv)
{
	shootingComponent->SetMinAndMaxVelocity(minv, maxv);
}
FVector ACatapultGun::GetRotatorForward()
{
	return parentMeshComponent->GetForwardVector();
}
bool ACatapultGun::IsProjectile()
{
	return gunData.isProjectile;
}
UProjectileShootingComponent* ACatapultGun::GetShootingComponent()
{
	return shootingComponent;
}