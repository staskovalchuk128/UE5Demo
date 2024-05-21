/*
Copyright 2024 Stanislav Kovalchuk
*/

#pragma once
#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BTTask_AimOnPlayer.generated.h"

UCLASS()
class MYPROJECT_API UBTTask_AimOnPlayer : public UBTTask_BlackboardBase
{
	GENERATED_BODY()
public:
	explicit UBTTask_AimOnPlayer(const FObjectInitializer& ObjectInitializer);
private:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& ownerComp, uint8* nodeMemory) override;
};
