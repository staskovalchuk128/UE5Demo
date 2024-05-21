/*
Copyright 2024 Stanislav Kovalchuk
*/

#pragma once
#include "CoreMinimal.h"
#include "AIController.h"
#include "Perception/AIPerceptionSystem.h"
#include "TrooperAIController.generated.h"

class ACatapultPawn;
class UBehaviorTreeComponent;
class UAISenseConfig_Sight;


UCLASS()
class MYPROJECT_API ATrooperAIController : public AAIController
{
	GENERATED_BODY()
public:
	explicit ATrooperAIController(const FObjectInitializer& ObjectInitializer);

	void Activate();
	void Deactivate();
protected:
	void BeginPlay();
	virtual void OnPossess(APawn* inPawn) override;
	virtual void OnUnPossess() override;
private:
	UAISenseConfig_Sight* sightConfig;

	void SetupPerceptionSystem();

	UFUNCTION()
	void OnTargetDetected(AActor* actor, FAIStimulus const stimulus);
};
