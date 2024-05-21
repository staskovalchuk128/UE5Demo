/*
Copyright 2024 Stanislav Kovalchuk
*/

#pragma once
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ProjectileShootingComponent.generated.h"


struct BulletData;
class UProjectileWeaponAiming;
class UWeapon_ActorComponent;
class ACatapultGun;
class UInventoryActorComponent;
class APlayerCatapult;
class APlayerCharacter;


UENUM(BlueprintType)
enum class ProjectileGunState : uint8
{
	READY UMETA(DisplayName = "Gun Ready"),
	PULLING_GUN UMETA(DisplayName = "Pulling gun"),
	UNPULLING_GUN UMETA(DisplayName = "Unpulling gun"),
	RELEASING_GUN UMETA(DisplayName = "Releasing gun"),
	RELOADING UMETA(DisplayName = "Reloading gun")
};

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class MYPROJECT_API UProjectileShootingComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UProjectileShootingComponent();

	void Init(APawn* parentActorPtr, UWeapon_ActorComponent* weaponComponentPtr);

	bool Shot();

	bool IsLoaded();

	const float& GetMaxAimTension();
	const float& GetMinAimTension();

	const float& GetMaxVelocity();
	const float& GetMinVelocity();

	const FVector GetBulletStartPosition();
	const FBoxSphereBounds GetBulletSize();

	void SetState(ProjectileGunState newState);
	ProjectileGunState GetState();

	float GetBulletInitialVelocity();
	FVector GetFront();
	FVector GetForward();

	UProjectileWeaponAiming* GetAiming();

	APawn* GetParentActorPtr();
	void OnComponentDestroyed(bool bDestroyingHierarchy);


	void SetVelocity(float val);

	UFUNCTION()
	void OnMouseMoveY(const float Value);

	float GetAimPitch();
	void SetAimPitch(float newValue);


	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;


	void SetMinAndMaxAimTension(float minT, float maxT);
	void SetMinAndMaxVelocity(float minT, float maxT);
	void StopPullingGun();
	void PullGun();
	void UnPullGun();


	void SetUseArcAim(bool v, float arcDegrees);

	float GetTensionPrc();

	void SetPullingGunSpeed(float v);
	void SetReleasingGunSpeed(float v);

	bool StartArcShot();

	void SetLaunchBulletsByOne(bool v);
	void SetBulletsByShot(int n);
	void SetIsCatapultGun(bool v, ACatapultGun* catapultActor);
	void SetHasRotator(bool v);

	float GetReloadingTime();
	void ResetBulletsOnGun();
protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Enum")
	ProjectileGunState state;

private:
	void RelasingGun(float dt);

	void OnPullingGun(float dt);
	void OnUnpullingGun(float dt);
	void OnPullingGunBackTo(float dt);

	void StartPullingGunBack();
	void StopPullingGunBack();

	UFUNCTION()
	void OnBulletRemoved(AActor* actor);

	void ThrowPickableObject(const FVector& vel, const FVector& startPos, const FVector& forward);
	void SpawnBullet(const BulletData& bd, UClass* bpClass, const FVector& vel, const FVector& startPos, const FVector& forward);

private:
	UInventoryActorComponent* playerInventory;
	APlayerCatapult* playerCatapultPtr;
	APlayerCharacter* playerCharacterPtr;
	ACatapultGun* catapultActorPtr;
	APawn* parentActorPtr;
	UWeapon_ActorComponent* weaponComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Aiming", meta = (AllowPrivateAccess = "true"))
	UProjectileWeaponAiming* aiming;

	FVector bulletStartPosition;
	bool hasRotator;
	bool isNPC;
	bool isCatapultGun;
	int bulletsByShot;
	bool launchBulletsByOne;
	bool pullingGunBack;
	float pullingGunBackTo;
	bool useArcAim;
	float arcDegrees;
	float reloadingTime, shotTime;
	float aimTensionValue, shotingTension;
	bool loaded;
	float currentVelocity, maxAimTension, minAimTension;
	float maxVelocity, minVelocity;
	float pullingGunSpeed, releasingGunSpeed;
	int bulletsLeftBeforeReloading;
};
