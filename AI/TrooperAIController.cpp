#include "AI/TrooperAIController.h"
#include "Kismet/GameplayStatics.h"

#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"

#include "ResourceManager.h"
#include "NPC/RoundCatapultPawn.h"

#include "AI/TrooperBTTask.h"

#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"

#include "Player/PlayerCatapult.h"

ATrooperAIController::ATrooperAIController(const FObjectInitializer& ObjectInitializer)
{
	SetupPerceptionSystem();
}

void ATrooperAIController::BeginPlay()
{
	Super::BeginPlay();
}

void ATrooperAIController::OnPossess(APawn* inPawn)
{
	Super::OnPossess(inPawn);
	
	if (ARoundCatapultPawn* const npc = Cast<ARoundCatapultPawn>(inPawn))
	{
		if (UBehaviorTree* const tree = npc->GetBehavorTree())
		{
			UBlackboardComponent* b;
			UseBlackboard(tree->BlackboardAsset, b);
			Blackboard = b;
			RunBehaviorTree(tree);
		}
	}
	
}

void ATrooperAIController::OnUnPossess()
{
	RunBehaviorTree(NULL);
}

void ATrooperAIController::SetupPerceptionSystem()
{
	sightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>("Sight cofig");
	if (sightConfig)
	{
		SetPerceptionComponent(*CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("Perception Component")));
		sightConfig->SightRadius = 1500.0f;
		sightConfig->LoseSightRadius = sightConfig->SightRadius + 25.0f;
		sightConfig->PeripheralVisionAngleDegrees = 360.0f;
		sightConfig->SetMaxAge(5.0f);
		sightConfig->AutoSuccessRangeFromLastSeenLocation = 520.0f;
		sightConfig->DetectionByAffiliation.bDetectEnemies = true;
		sightConfig->DetectionByAffiliation.bDetectFriendlies = true;
		sightConfig->DetectionByAffiliation.bDetectNeutrals = true;

		GetPerceptionComponent()->SetDominantSense(*sightConfig->GetSenseImplementation());
		GetPerceptionComponent()->ConfigureSense(*sightConfig);
		GetPerceptionComponent()->OnTargetPerceptionUpdated.AddDynamic(this, &ATrooperAIController::OnTargetDetected);
	}

}

void ATrooperAIController::OnTargetDetected(AActor* actor, FAIStimulus const stimulus)
{
	if (auto* const character = Cast<APlayerCatapult>(actor))
	{
		GetBlackboardComponent()->SetValueAsBool("CanSeePlayer", stimulus.WasSuccessfullySensed());
	}
}


void ATrooperAIController::Activate()
{
	if (ARoundCatapultPawn* const npc = Cast<ARoundCatapultPawn>(GetPawn()))
	{
		if (UBehaviorTree* const tree = npc->GetBehavorTree())
		{
			Cast<UBehaviorTreeComponent>(BrainComponent)->StartTree(*tree);
		}
	}

}

void ATrooperAIController::Deactivate()
{
	Cast<UBehaviorTreeComponent>(BrainComponent)->StopTree();
}