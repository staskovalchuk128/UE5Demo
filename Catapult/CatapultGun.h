/*
Copyright 2024 Stanislav Kovalchuk
*/

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "..//Aiming/ProjectileShootingComponent.h"
#include "CatapultGunsManager.h"
#include "CatapultGun.generated.h"

struct CatapultData;
class UProjectileWeaponAiming;
class UCatapultAiming;
class UWeapon_ActorComponent;


UCLASS()
class MYPROJECT_API ACatapultGun : public AActor
{
	GENERATED_BODY()
	
public:	
	ACatapultGun();

	UFUNCTION(BlueprintCallable, Category = "Init")
	void Init(APawn* parentActorPtr, USkeletalMeshComponent* parentMC, const FString& gd, bool hasRotator = false);

	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable, Category = "catapultGun")
	ProjectileGunState GetState() const;


	UProjectileWeaponAiming* GetAiming();

	void SetGunTension(float t);

	void PullGun();
	void UnPullGun();
	void StopPullingGun();


	const FVector GetBulletStartPosition();
	const FVector GetConstBulletStartPosition();

	float GetBulletMaxVelocity();
	float GetBulletMinVelocity();
	float GetMaxGunTension();
	float GetGunTension();

	float GetTensionPrc();

	bool Shot();
	bool IsLoaded();

	void SetBulletType(FString newType);
	FString GetBulletType();
	void ShiftBullets(int bulletIndex);

	void OnGunLoaded();

	//if -1 then show all
	void ShowBullets(int count = -1);
	void SetMinAndMaxVelocity(float minv, float maxv);

	FVector GetRotatorForward();

	bool IsProjectile();

	UProjectileShootingComponent* GetShootingComponent();
protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shooting", meta = (AllowPrivateAccess = "true"))
	UProjectileShootingComponent* shootingComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Enum")
	ProjectileGunState state;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gun Mesh", meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* gunMeshComponent;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bullet", meta = (AllowPrivateAccess = "true"))
	TArray<UChildActorComponent*> bulletComponents;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gun type", meta = (AllowPrivateAccess = "true"))
	FString gunType;
private:
	UWeapon_ActorComponent* actorWeaponData;
	CatapultGunData gunData;
	APawn* parentActorPtr;
	USkeletalMeshComponent* parentMeshComponent;
	FString bulletType;
	UCatapultAiming* aiming;
	std::vector<FVector> bulletStartPositions, constBulletStartPositions;
	FVector bulletStartPosition, constBulletStartPosition;
	bool catapultHasVehicle;
	FVector gunPosition, gunLocalOffset, bulletLocalOffset, bulletSize;
	FVector gunRotation;
	bool pullingGunBack;
	float pullingGunBackTo;

	int bulletsLeftBeforeReloading;
};
