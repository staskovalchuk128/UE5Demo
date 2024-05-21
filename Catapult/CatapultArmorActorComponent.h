/*
Copyright 2024 Stanislav Kovalchuk
*/

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Catapult/CatapultArmorsManager.h"
#include "..//UI/InventorySlotItem.h"
#include "CatapultArmorActorComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MYPROJECT_API UCatapultArmorActorComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UCatapultArmorActorComponent();

	UFUNCTION(BlueprintCallable)
	void Init(USkeletalMeshComponent* parentMC, FString type);

	UFUNCTION(BlueprintCallable)
	void RemoveArmor();

	UFUNCTION(BlueprintCallable)
	int GetHealthIncreaseValue();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Catapult Armor", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* meshComponent;
	
private:
	CatapultArmorData armorData;
};
