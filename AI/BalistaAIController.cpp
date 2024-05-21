#include "AI/BalistaAIController.h"
#include "Kismet/GameplayStatics.h"

#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"

#include "NPC/BalistaPawn.h"


ABalistaAIController::ABalistaAIController(const FObjectInitializer& ObjectInitializer)
{
}

void ABalistaAIController::BeginPlay()
{
	Super::BeginPlay();
}

void ABalistaAIController::OnPossess(APawn* inPawn)
{
	Super::OnPossess(inPawn);

	if (ABalistaPawn* const npc = Cast<ABalistaPawn>(inPawn))
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

void ABalistaAIController::OnUnPossess()
{
	Super::OnUnPossess();
	RunBehaviorTree(NULL);
}

void ABalistaAIController::Activate()
{
	if (ABalistaPawn* const npc = Cast<ABalistaPawn>(GetPawn()))
	{
		if (UBehaviorTree* const tree = npc->GetBehavorTree())
		{
			Cast<UBehaviorTreeComponent>(BrainComponent)->StartTree(*tree);
		}
	}
}
void ABalistaAIController::Deactivate()
{
	Cast<UBehaviorTreeComponent>(BrainComponent)->StopTree();
}