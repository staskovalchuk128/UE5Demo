/*
Copyright 2024 Stanislav Kovalchuk
*/

#pragma once
#include "CoreMinimal.h"
#include "NPCCommon.h"
#include "GameFramework/Pawn.h"
#include "..//Character/CharacterCommon.h"
#include "BalistaPawn.generated.h"


class UBehaviorTree;
class ACatapultGun;
class UParticleSystemComponent;
class UProjectileShootingComponent;
class UWeapon_ActorComponent;
class ACastleLevelInstance;
class UWidgetComponent;

UCLASS()
class MYPROJECT_API ABalistaPawn : public APawn, public NPCCommon
{
	GENERATED_BODY()
public:
	ABalistaPawn();

	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UBehaviorTree* GetBehavorTree() const;

	bool IsReadyToShot();
	bool IsAimOnPlayer();

	void Shot();
	void StartAim();
	void StopAim();

	void ActivateAI();
	void DeactivateAI();

	void SetShootingState(CharacterShootingState newState);
	CharacterShootingState GetShootingState();

	UProjectileShootingComponent* GetShootingComponent();
protected:
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	UWidgetComponent* GetHealthBarWidget();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI", meta = (AllowPrivateAccess = "true"))
	UBehaviorTree* behavorTree;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Particle", meta = (AllowPrivateAccess = "true"))
	UParticleSystemComponent* explosionParticle;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Enum")
	CharacterShootingState shootingState;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Castle", meta = (AllowPrivateAccess = "true"))
	ACastleLevelInstance* castleInstance;

	UFUNCTION(BlueprintCallable)
	float GetAimPitch();

	UFUNCTION()
	void OnExplosionOver(class UParticleSystemComponent* PSystem);

	UFUNCTION()
	void OnCatapultDamaged(int newHealth);

	UFUNCTION()
	void OnHit(class UPrimitiveComponent* HitComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	void OnCatapultDied();

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Weapon")
	UWeapon_ActorComponent* weaponComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
	UHealthActorComponent* healthComponent;
private:
	bool catapultDown;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* catapultMeshComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* gunMeshComponent;

	UProjectileShootingComponent* shootingComponent;

	USkeletalMeshSocket* bulletSocket;
	UWidgetComponent* healthBarWidget;


};
