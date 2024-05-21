/*
Copyright 2024 Stanislav Kovalchuk
*/

#pragma once
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "..//UI/InventorySlotItem.h"
#include "CityBuildingsComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MYPROJECT_API UCityBuildingsComponent : public UActorComponent
{
	GENERATED_BODY()
public:	
	UCityBuildingsComponent();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable)
	bool IsBuilt(CityBuildingEnum type);
protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Built Buildings", meta = (AllowPrivateAccess = "true"))
	TMap<CityBuildingEnum, FVector> buildingPositions;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Built Buildings", meta = (AllowPrivateAccess = "true"))
	TMap<CityBuildingEnum, FRotator> buildingRotations;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Built Buildings", meta = (AllowPrivateAccess = "true"))
	TArray<FDataTableRowHandle> builtBuildings;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Avalable Buildings", meta = (AllowPrivateAccess = "true"))
	TArray<FDataTableRowHandle> avalableBuildings;
};
