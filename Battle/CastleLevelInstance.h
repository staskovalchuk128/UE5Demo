/*
Copyright 2024 Stanislav Kovalchuk
*/

#pragma once

#include "CoreMinimal.h"
#include "Containers/List.h"
#include "UI/InventorySlotItem.h"
#include "LevelInstance/LevelInstanceActor.h"
#include "CastleLevelInstance.generated.h"

class NPCCommon;
class APlayerCharacter;
class ULevelSequence;
class UCityBuildingsComponent;
class USphereComponent;
class UInventoryActorComponent;

UCLASS()
class MYPROJECT_API ACastleLevelInstance : public ALevelInstance
{
	GENERATED_BODY()
public:
	ACastleLevelInstance();

	virtual void Tick(float DeltaTime) override;


	UFUNCTION(BlueprintCallable)
	int GetNumOfTroopers();

	UFUNCTION(BlueprintCallable)
	int GetNumOfCatapults();

	UFUNCTION(BlueprintCallable)
	void OnBattleStart();

	UFUNCTION(BlueprintCallable)
	void OnBattleEnd();

	void OnNPCDied(NPCCommon* npcPtr);

	void SetPlayerPtr(APlayerCharacter* playerP);

	bool IsInBattle();

	UFUNCTION(BlueprintCallable)
	void OnPlayerAttackedCastle();

	UFUNCTION(BlueprintCallable)
	void OnPlayerCapturedCastle();

	UFUNCTION(BlueprintCallable)
	bool IsPlayerInCastle();

	UFUNCTION(BlueprintCallable)
	bool IsOwnedByPlayer();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void StartCityCamera(float timeLimit = 0.0f);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void StopCityCamera();
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void CreateBuildings();

	UFUNCTION(BlueprintCallable)
	const TArray<FInventorySlotItem>& GetMerchantCatapultItems();

	UFUNCTION(BlueprintCallable)
	const TArray<FInventorySlotItem>& GetMerchantBulletsItems();

	UFUNCTION(BlueprintCallable)
	const TArray<FInventorySlotItem>& GetMerchantArmorItems();

	UFUNCTION(BlueprintCallable)
	const TArray<FInventorySlotItem>& GetMerchantManArmorItems();

	UFUNCTION(BlueprintCallable)
	TArray<FInventorySlotItem> GetMerchantCatapultItemsByType(InventorySlotItemType type);

	UFUNCTION(BlueprintCallable)
	TArray<FInventorySlotItem> GetMerchantBulletsItemsByType(InventorySlotItemType type);

	UFUNCTION(BlueprintCallable)
	TArray<FInventorySlotItem> GetMerchantArmorItemsByType(InventorySlotItemType type);

	UFUNCTION(BlueprintCallable)
	TArray<FInventorySlotItem> GetMerchantManWeaponsItemsByType(InventorySlotItemType type);

	UFUNCTION(BlueprintCallable)
	void SpawnCitizents();

	UFUNCTION(BlueprintCallable)
	void OnCapturedCastleAnimationStoped();

	void OnPlayerEnterHisOwnCastle();
	void OnPlayerLeaveHisOwnCastle();

	void BeginPlay();
protected:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats", meta = (AllowPrivateAccess = "true"))
	TArray<ACharacter*> troopers;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats", meta = (AllowPrivateAccess = "true"))
	TArray<APawn*> catapults;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats", meta = (AllowPrivateAccess = "true"))
	TArray<FCitizenData> citizens;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Info", meta = (AllowPrivateAccess = "true"))
	bool ownedByPlayer;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Info", meta = (AllowPrivateAccess = "true"))
	FVector playerPositionWhenCapture;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Info", meta = (AllowPrivateAccess = "true"))
	FRotator playerRotationWhenCapture;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Info", meta = (AllowPrivateAccess = "true"))
	FVector catapultPositionWhenCapture;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Info", meta = (AllowPrivateAccess = "true"))
	FRotator catapultRotationWhenCapture;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Info", meta = (AllowPrivateAccess = "true"))
	bool removePlayerFromCatapultWhenCapture;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Info", meta = (AllowPrivateAccess = "true"))
	USphereComponent* enterLeaveDetector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Merchant", meta = (AllowPrivateAccess = "true"))
	UInventoryActorComponent* merchantInventoryCatapults;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Merchant", meta = (AllowPrivateAccess = "true"))
	UInventoryActorComponent* merchantInventoryBullets;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Merchant", meta = (AllowPrivateAccess = "true"))
	UInventoryActorComponent* merchantInventoryArmor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Merchant", meta = (AllowPrivateAccess = "true"))
	UInventoryActorComponent* merchantInventoryManWeapons;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Merchant", meta = (AllowPrivateAccess = "true"))
	UCityBuildingsComponent* buildings;

	UFUNCTION(BlueprintCallable)
	void RemoveAllTroopersFromCastle();

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnOverlapEnd(UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

private:
	void ChangeTroopersAIStatus(bool enabled);
	bool inBattle;
	bool playerStartedBattle;
	bool playerInCastleArea;
	bool catapultInCastleArea;
	FVector positionWhenPlayerStartedBattle;
	bool currentlyShownCaputedCastleAnim;
	bool playerInCastle;
	APlayerCharacter* playerPtr;


	void SetShowTroopersHealthBars(bool res);
	void RestoreTroopersHealth();
};
