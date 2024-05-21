#include "Player/InventoryActorComponent.h"
#include "Bullets/BulletsManager.h"
#include "Bullets/GameBulletActor.h"
#include "MeleeWeapons/MeleeWeaponsManager.h"
#include "Player/PlayerCharacter.h"
#include "Player/PlayerCatapult.h"

UInventoryActorComponent::UInventoryActorComponent()
{
	capacity = 20;
	inventoryLoaded = false;
	PrimaryComponentTick.bCanEverTick = true;
	inventory.Init(FInventorySlotItem(), capacity);
}

void UInventoryActorComponent::ChanageCapacity(int num)
{
	for (int i = capacity; i < num; i++)
	{
		inventory.Add(FInventorySlotItem());
	}
	capacity = num;
}

void UInventoryActorComponent::ResetCapacity(int num)
{
	capacity = num;
	inventory.Init(FInventorySlotItem(), num);
}

int UInventoryActorComponent::GetNextEmptySlot()
{
	for (int i = 0; i < capacity; i++)
	{
		if (IsSlotEmpty(i)) return i;
	}
	return -1;
}

int UInventoryActorComponent::GetNumOfEmptySlots()
{
	int num = 0;
	for (int i = 0; i < capacity; i++)
	{
		if (IsSlotEmpty(i)) num++;
	}
	return num;
}

int UInventoryActorComponent::GetFirstSlotOfType(InventorySlotItemType type)
{
	for (int i = 0; i < capacity; i++)
	{
		if (inventory[i].item.slotType == type) return i;
	}
	return -1;
}

int UInventoryActorComponent::GetFirstItemByType(FString type)
{
	for (int i = 0; i < capacity; i++)
	{
		if (inventory[i].item.objectType == type) return i;
	}
	return -1;
}

bool UInventoryActorComponent::IsSlotEmpty(int slotIndex)
{
	if (slotIndex > capacity - 1 || slotIndex < 0) return false;
	return inventory[slotIndex].dtData.IsNull();
}

void UInventoryActorComponent::AddItemToSlot(int slot, FInventorySlotItem item)
{
	inventory[slot] = item;
}

void UInventoryActorComponent::SwapItems(int from, int to)
{
	inventory.Swap(from, to);
}

int UInventoryActorComponent::SwapItemsWithDiffrentInventory(UInventoryActorComponent* otherInv, int from, int to, int quantity)
{
	FInventorySlotItem tempFrom = otherInv->GetItemAtIndex(to);
	FInventorySlotItem tempTo = GetItemAtIndex(from);
	
	int outIndex = from;

	if (tempFrom.item.multicount)
	{
		FInventorySlotItem* slotToMoveIn = FindItem(tempFrom.item.objectType);
		
		if (quantity >= tempFrom.quantity)
		{
			if (slotToMoveIn)
			{
				slotToMoveIn->quantity += quantity;
				outIndex = GetIndexByItem(*slotToMoveIn);
				otherInv->EmptySlot(to);
			}
			else
			{
				ReplaceItemWith(from, otherInv->GetItemAtIndex(to));
				otherInv->ReplaceItemWith(to, tempTo);
			}

		}
		else
		{
			if (slotToMoveIn)
			{
				slotToMoveIn->quantity += quantity;
				outIndex = GetIndexByItem(*slotToMoveIn);
			}
			else
			{
				FInventorySlotItem item = tempFrom;
				item.quantity = quantity;
				AddItemToSlot(from, item);
			}
		}

	}
	else
	{
		ReplaceItemWith(from, otherInv->GetItemAtIndex(to));
		otherInv->ReplaceItemWith(to, tempTo);
	}

	return outIndex;
}

void UInventoryActorComponent::ReplaceItemWith(int index, FInventorySlotItem items)
{
	inventory[index] = items;
}

FInventorySlotItem UInventoryActorComponent::GetItemAtIndex(int index)
{
	return inventory[index];
}

int UInventoryActorComponent::GetIndexByItem(FInventorySlotItem& item)
{
	for (size_t i = 0; i < inventory.Num(); i++)
	{
		if (&inventory[i] == &item) return i;
	}
	return -1;
}

void UInventoryActorComponent::ChangeQuantityForSlot(int slotIndex, int units)
{
	if (inventory[slotIndex].item.multicount)
	{
		inventory[slotIndex].quantity = units;
	}
}

void UInventoryActorComponent::EmptySlot(int slotIndex)
{
	if (slotIndex > capacity - 1 || slotIndex < 0) return;
	inventory[slotIndex] = FInventorySlotItem();
}

void UInventoryActorComponent::EquipSlot(InventorySlotItemType type, int index)

{
	inventory[index].equipedType = type;
}

void UInventoryActorComponent::UnequipSlot(InventorySlotItemType type)
{
	for (auto& it : inventory)
	{
		if (it.equipedType == type)
		{
			it.equipedType = InventorySlotItemType::NONE;
		}
	}
}

int UInventoryActorComponent::GetEquipedSlotIndexByType(InventorySlotItemType type)
{
	for (int i = 0; i < capacity; i++)
	{
		if (inventory[i].equipedType == type) return i;
	}
	return -1;
}

bool UInventoryActorComponent::IsSlotEquiped(int index)
{
	return inventory[index].equipedType != InventorySlotItemType::NONE;
}

bool UInventoryActorComponent::IsSlotEquipedFor(InventorySlotItemType type)
{
	bool res = false;
	for (auto& it : inventory)
	{
		if (it.equipedType == type)
		{
			res = true;
			break;
		}
	}
	return res;
}

void UInventoryActorComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UInventoryActorComponent::InitItems()
{
	if (inventory.Num() == 0)
	{
		inventory.Init(FInventorySlotItem(), capacity);
	}
	//Load items from data table
	for (auto& it: inventory )
	{
		if (it.dtData.IsNull()) continue;
		FGameTradableItem* ptr = it.dtData.GetRow<FGameTradableItem>(it.dtData.RowName.ToString());
		it.item = ptr ? *ptr : FGameTradableItem();
	}
	inventoryLoaded = true;
}


int UInventoryActorComponent::GetBulletsCount(FString type)
{
	return FindItem(type)->quantity;
}

FString UInventoryActorComponent::GetBulletsCountAsStr(FString type)
{
	FInventorySlotItem* invItem = FindItem(type);
	if (!invItem) return L"∞";
	if (IsMeleeWeapon(invItem->item.objectType)) return L"";
	int count = FindItem(type)->quantity;
	return count == -5 ? L"∞" : FString::FromInt(count);
}

FInventorySlotItem* UInventoryActorComponent::FindItem(FString type)
{
	for (auto&it: inventory)
	{
		if (it.item.objectType == type) return &it;
	}
	return NULL;
	/*
	TArray<FInventorySlotItem>::iterator it = std::find_if(inventory.begin(), inventory.end(), [type](const FInventorySlotItem& item)
		{
			return item.bulletType == type;
		}
	);
	if (it == inventory.end()) return NULL;
	return &*it;
	*/
}

void UInventoryActorComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

void UInventoryActorComponent::SetNewCount(FString type, int units)
{
	// -5 for unlimited bullets
	if (units <= -5) units = -5;
	else
	{
		units = units < 0 ? 0 : units;
	}

	FInventorySlotItem* item = FindItem(type);
	if(item) item->quantity = units;
}
void UInventoryActorComponent::RemoveCount(FString type, int units)
{
	SetNewCount(type, GetBulletsCount(type) - units);
}
void UInventoryActorComponent::AddCount(FString type, int units)
{
	SetNewCount(type, GetBulletsCount(type) + units);
}

const TArray<FInventorySlotItem>& UInventoryActorComponent::GetAllItems()
{
	if (!inventoryLoaded) InitItems();
	return inventory;
}

TArray<FInventorySlotItem> UInventoryActorComponent::GetItemsByType(InventorySlotItemType type)
{
	if (!inventoryLoaded) InitItems();
	if (type == InventorySlotItemType::NONE) return inventory;
	TArray<FInventorySlotItem> items;
	for (auto& it : inventory)
	{
		if (it.item.slotType == type) items.Push(it);
	}
	return items;
}


TArray<FInventorySlotItem> UInventoryActorComponent::GetItemsByTypes(TArray<InventorySlotItemType> types)
{
	if (!inventoryLoaded) InitItems();
	TArray<FInventorySlotItem> items;
	for (auto& it : inventory)
	{
		if(types.FindByKey(it.item.slotType)) items.Push(it);
	}
	return items;
}

void UInventoryActorComponent::AddMoney(int num)
{
	SetMoney(money + num);
}
void UInventoryActorComponent::RemoveMoney(int num)
{
	SetMoney(money - num);
}
void UInventoryActorComponent::SetMoney(int num)
{
	money = num;
	money = money < 0 ? 0 : money;
}

int UInventoryActorComponent::GetMoney()
{
	return money;
}