#include "BPLibs/GeneralBlueprintFunctionLibrary.h"
#include "Misc/OutputDeviceNull.h"
#include "ResourceManager.h"
#include "Kismet/GameplayStatics.h"

#include "Blueprint/UserWidget.h"

#include "UI/UserMessageWidget.h"

#include "Blueprint/WidgetBlueprintLibrary.h"

#include "Components/CanvasPanelSlot.h"

#include "Player/PlayerCatapult.h"
#include "Player/PlayerCharacter.h"

UGeneralBlueprintFunctionLibrary::UGeneralBlueprintFunctionLibrary(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
}

void UGeneralBlueprintFunctionLibrary::CreateUserMessage(UGameInstance* GameInstance, FText text, float duration, UserMessageType messageType)
{
	UClass* test = ResourceManager::Get<UClass>("BP_UserMessage");

	TArray<UUserWidget*> items;
	UWidgetBlueprintLibrary::GetAllWidgetsOfClass(GameInstance->GetWorld(), items, UUserMessageWidget::StaticClass(), true);
	if (items.Num() > 10) return;


	UUserMessageWidget* wp = CreateWidget<UUserMessageWidget>(GameInstance, ResourceManager::Get<UClass>("BP_UserMessage"));
	

	if (wp)
	{
		wp->AddToViewport(10);
		wp->ShowMessage(text, duration, messageType);
	}

}


void UGeneralBlueprintFunctionLibrary::CreateUserMessageRemovedMoney(UGameInstance* GameInstance, int num)
{
	FString txt = "Removed " + FString(std::to_string(num).c_str()) + " coins";
	CreateUserMessage(GameInstance, FText::FromString(txt), 2.0f, UserMessageType::REMOVE_MONEY);
}

void UGeneralBlueprintFunctionLibrary::CreateUserMessageAddMoney(UGameInstance* GameInstance, int num)
{
	FString txt = "Added " + FString(std::to_string(num).c_str()) + " coins";
	CreateUserMessage(GameInstance, FText::FromString(txt), 2.0f, UserMessageType::ADD_MONEY);
}

void UGeneralBlueprintFunctionLibrary::CreateUserMessageNotEnoughMoney(UGameInstance* GameInstance)
{
	FString txt = "You don't have enough money";
	CreateUserMessage(GameInstance, FText::FromString(txt), 2.0f, UserMessageType::ERROR);
}

APlayerCharacter* UGeneralBlueprintFunctionLibrary::GetPlayerChar(UGameInstance* GameInstance)
{
	APawn* pawn = UGameplayStatics::GetPlayerPawn(GameInstance->GetWorld(), 0);
	if (!pawn) return NULL;
	if (pawn->IsA(APlayerCatapult::StaticClass()))
	{
		return Cast<APlayerCatapult>(pawn)->GetPlayerCharacterInCatapult();
	}
	return Cast<APlayerCharacter>(pawn);
}