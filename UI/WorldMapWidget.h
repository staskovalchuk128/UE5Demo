/*
Copyright 2024 Stanislav Kovalchuk
*/

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "WorldMapWidget.generated.h"


UCLASS()
class MYPROJECT_API UWorldMapWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	UWorldMapWidget(const FObjectInitializer& ObjectInitializer);
};
