/*
Copyright 2024 Stanislav Kovalchuk
*/

#pragma once
#include "CoreMinimal.h"
#include "NPCCommon.h"
#include "GameFramework/Character.h"
#include "..//Character/CharacterCommon.h"
#include "Components/WidgetComponent.h"

#include "ManTrooperChar.generated.h"

class UBehaviorTree;
class UBehaviorTreeComponent;
class UProjectileShootingComponent;
class AGameBulletActor;
class APlayerCatapult;
class UWeapon_ActorComponent;
class ACastleLevelInstance;
class UWidgetComponent;

UCLASS()
class MYPROJECT_API AManTrooperChar : public ACharacter, public NPCCommon, public CharacterCommon
{
	GENERATED_BODY()
public:
	AManTrooperChar();
	void Destroyed();

	bool IsDoneAiming();
	UProjectileShootingComponent* GetShootingComponent();

	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UBehaviorTree* GetBehavorTree() const;

	bool IsReadyToShot();
	bool IsAimOnPlayer();
	bool IsJustDummy();

	void ShotWithDelay(float delayFrom, float delayTo);
	void CancelShot();
	void Shot();
	void StartAim();
	void StopAim();

	void SetWorldState(CharacterWorldState newState);
	void SetShootingState(CharacterShootingState newState);

	CharacterWorldState GetWorldState();
	CharacterShootingState GetShootingState();

	void OnTowerUnderFeetExploded();

	void ThrowNpc(const FVector& normalImpulse);

	void DamageActor(int units);

	void SwitchToMeleeWeapon();
	void SwitchToRangeWeapon();
	UWeapon_ActorComponent* GetWeaponComponent();

	void ActivateAI();
	void DeactivateAI();


protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Enum")
	CharacterWorldState worldState;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Enum")
	CharacterShootingState shootingState;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI", meta = (AllowPrivateAccess = "true"))
	UBehaviorTree* behavorTree;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Castle", meta = (AllowPrivateAccess = "true"))
	ACastleLevelInstance* castleInstance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NPC INFO", meta = (AllowPrivateAccess = "true"))
	bool showHealthBar;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NPC INFO", meta = (AllowPrivateAccess = "true"))
	bool justDummy;


	virtual void BeginPlay() override;

	UFUNCTION()
	void OnCharDamaged(int newHealth);

	UFUNCTION()
	void HandleOnMontageEnded(UAnimMontage* Montage, bool Interrupted);

	UFUNCTION()
	void OnHit(class UPrimitiveComponent* HitComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	UWidgetComponent* GetHealthBarWidget();

	UFUNCTION(BlueprintCallable)
	void OnCharacterGotUpAfterThrow();

	UFUNCTION(BlueprintCallable)
	void OnCharacterIsDoneAiming();

	UFUNCTION(BlueprintCallable)
	float GetAimPitch();

	UFUNCTION(BlueprintCallable)
	void OnLandedDeadFromTower();

	void DestroyCharacter();


	void OnCatapultHit(APlayerCatapult* catapultPtr, const FVector& normalImpulse);
	void OnBulletHit(AGameBulletActor* bulletPtr, const FVector& normalImpulse);



	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Weapon")
	UWeapon_ActorComponent* weaponComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
	UHealthActorComponent* healthComponent;


private:
	UWidgetComponent* healthBarWidget;
	USkeletalMeshSocket* weaponSocket;
	UProjectileShootingComponent* shootingComponent;
	bool waitingToShot;
	float shotDelay, shotAwaitTime;
};
