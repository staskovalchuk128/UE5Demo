/*
Copyright 2024 Stanislav Kovalchuk
*/

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "GeneralBlueprintFunctionLibrary.generated.h"

class APlayerCharacter;

enum class UserMessageType : uint8;

UCLASS()
class MYPROJECT_API UGeneralBlueprintFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	UGeneralBlueprintFunctionLibrary(const FObjectInitializer& ObjectInitializer);

	UFUNCTION(BlueprintCallable, Category = "Custom kit functions")
	static void CreateUserMessage(UGameInstance* GameInstance, FText text, float duration, UserMessageType messageType);

	UFUNCTION(BlueprintCallable, Category = "Custom kit functions")
	static void CreateUserMessageRemovedMoney(UGameInstance* GameInstance, int num);

	UFUNCTION(BlueprintCallable, Category = "Custom kit functions")
	static void CreateUserMessageAddMoney(UGameInstance* GameInstance, int num);

	UFUNCTION(BlueprintCallable, Category = "Custom kit functions")
	static void CreateUserMessageNotEnoughMoney(UGameInstance* GameInstance);

	UFUNCTION(BlueprintCallable, Category = "Custom kit functions")
	static APlayerCharacter* GetPlayerChar(UGameInstance* GameInstance);
};
