/*
Copyright 2024 Stanislav Kovalchuk
*/

#pragma once
#include "CoreMinimal.h"
#include "AIController.h"
#include "Perception/AIPerceptionSystem.h"
#include "BalistaAIController.generated.h"

UCLASS()
class MYPROJECT_API ABalistaAIController : public AAIController
{
	GENERATED_BODY()
public:
	explicit ABalistaAIController(const FObjectInitializer& ObjectInitializer);
	void Activate();
	void Deactivate();
protected:
	void BeginPlay();
	virtual void OnPossess(APawn* inPawn) override;
	virtual void OnUnPossess() override;
};
