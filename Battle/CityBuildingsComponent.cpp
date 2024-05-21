#include "Battle/CityBuildingsComponent.h"

// Sets default values for this component's properties
UCityBuildingsComponent::UCityBuildingsComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}


void UCityBuildingsComponent::BeginPlay()
{
	Super::BeginPlay();
	
}

void UCityBuildingsComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

bool UCityBuildingsComponent::IsBuilt(CityBuildingEnum type)
{
	for (auto &it: builtBuildings)
	{
		if (it.DataTable->FindRow<FCityBuilding>(it.RowName, "")->type == type) return true;
	}
	return false;
}

