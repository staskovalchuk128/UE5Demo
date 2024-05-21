/*
Copyright 2024 Stanislav Kovalchuk
*/

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ManageCityWidget.generated.h"

class ACastleLevelInstance;

UCLASS()
class MYPROJECT_API UManageCityWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	UManageCityWidget(const FObjectInitializer& ObjectInitializer);
	void SetCastlePtr(ACastleLevelInstance* ptr);
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	ACastleLevelInstance* castlePtr;

};
