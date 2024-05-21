/*
Copyright 2024 Stanislav Kovalchuk
*/

#pragma once
#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BTTask_Shot.generated.h"

UCLASS()
class MYPROJECT_API UBTTask_Shot : public UBTTask_BlackboardBase
{
	GENERATED_BODY()
public:
	explicit UBTTask_Shot(const FObjectInitializer& ObjectInitializer);
private:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& ownerComp, uint8* nodeMemory) override;
};
