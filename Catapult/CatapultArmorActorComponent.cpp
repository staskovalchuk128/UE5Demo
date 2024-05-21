#include "Catapult/CatapultArmorActorComponent.h"
#include "ResourceManager.h"

UCatapultArmorActorComponent::UCatapultArmorActorComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	meshComponent = CreateDefaultSubobject<UStaticMeshComponent>("meshComponent");
}

void UCatapultArmorActorComponent::BeginPlay()
{
	Super::BeginPlay();

}

void UCatapultArmorActorComponent::Init(USkeletalMeshComponent* parentMC, FString type)
{
	armorData = CatapultArmorsManager::GetArmorData(type);
	UStaticMesh* mesh = ResourceManager::Get<UStaticMesh>(armorData.modelName);
	meshComponent->SetStaticMesh(mesh);
	meshComponent->AttachToComponent(parentMC, FAttachmentTransformRules::KeepRelativeTransform, "ArmorSocket");
	meshComponent->RegisterComponent();
}

void UCatapultArmorActorComponent::RemoveArmor()
{
	UnregisterComponent();
	meshComponent->UnregisterComponent();
}

int UCatapultArmorActorComponent::GetHealthIncreaseValue()
{
	return armorData.healthIncreaseValue;
}


void UCatapultArmorActorComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

