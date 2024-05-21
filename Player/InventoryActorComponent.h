/*
Copyright 2024 Stanislav Kovalchuk
*/

#pragma once
#include <vector>
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CoreFwd.h"
#include "..//UI/InventorySlotItem.h"
#include "InventoryActorComponent.generated.h"

enum struct BulletUsageType;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MYPROJECT_API UInventoryActorComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UInventoryActorComponent();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;


	int GetBulletsCount(FString type);
	FString GetBulletsCountAsStr(FString type);
	void SetNewCount(FString type, int units);
	void RemoveCount(FString type, int units);
	void AddCount(FString type, int units);

	const TArray<FInventorySlotItem>& GetAllItems();
	
	UFUNCTION(BlueprintCallable)
	TArray<FInventorySlotItem> GetItemsByType(InventorySlotItemType type);

	UFUNCTION(BlueprintCallable)
	TArray<FInventorySlotItem> GetItemsByTypes(TArray<InventorySlotItemType> types);

	UFUNCTION(BlueprintCallable)
	void AddMoney(int num);
	
	UFUNCTION(BlueprintCallable)
	void RemoveMoney(int num);

	UFUNCTION(BlueprintCallable)
	int GetMoney();

	UFUNCTION(BlueprintCallable)
	void ChanageCapacity(int num);

	UFUNCTION(BlueprintCallable)
	void ResetCapacity(int num);

	UFUNCTION(BlueprintCallable)
	int GetNextEmptySlot();

	UFUNCTION(BlueprintCallable)
	int GetNumOfEmptySlots();

	UFUNCTION(BlueprintCallable)
	int GetFirstSlotOfType(InventorySlotItemType type);

	UFUNCTION(BlueprintCallable)
	int GetFirstItemByType(FString type);

	UFUNCTION(BlueprintCallable)
	bool IsSlotEmpty(int slotIndex);

	UFUNCTION(BlueprintCallable)
	void SwapItems(int from, int to);

	UFUNCTION(BlueprintCallable)
	void AddItemToSlot(int slot, FInventorySlotItem item);

	UFUNCTION(BlueprintCallable)
	int SwapItemsWithDiffrentInventory(UInventoryActorComponent* otherInv, int from, int to, int quantity);

	UFUNCTION(BlueprintCallable)
	void EmptySlot(int index);

	UFUNCTION(BlueprintCallable)
	void ReplaceItemWith(int index, FInventorySlotItem item);

	UFUNCTION(BlueprintCallable)
	void EquipSlot(InventorySlotItemType type, int index);

	UFUNCTION(BlueprintCallable)
	void UnequipSlot(InventorySlotItemType type);

	UFUNCTION(BlueprintCallable)
	int GetEquipedSlotIndexByType(InventorySlotItemType type);

	UFUNCTION(BlueprintCallable)
	bool IsSlotEquiped(int index);

	UFUNCTION(BlueprintCallable)
	bool IsSlotEquipedFor(InventorySlotItemType type);

	UFUNCTION(BlueprintCallable)
	FInventorySlotItem GetItemAtIndex(int index);

	UFUNCTION(BlueprintCallable)
	int GetIndexByItem(FInventorySlotItem& item);

	UFUNCTION(BlueprintCallable)
	void ChangeQuantityForSlot(int slotIndex, int units);
protected:
	virtual void BeginPlay() override;
	FInventorySlotItem* FindItem(FString type);

	UFUNCTION(BlueprintCallable)
	void SetMoney(int num);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory", meta = (AllowPrivateAccess = "true"))
	TArray<FInventorySlotItem> inventory;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory", meta = (AllowPrivateAccess = "true"))
	int capacity;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory", meta = (AllowPrivateAccess = "true"))
	int money;

	UFUNCTION(BlueprintCallable)
	void InitItems();

private:
	bool inventoryLoaded;
};
