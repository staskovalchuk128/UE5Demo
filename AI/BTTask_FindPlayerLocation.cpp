#include "AI/BTTask_FindPlayerLocation.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"
#include "NavigationSystem.h"
#include "BehaviorTree/BlackboardComponent.h"

#include "Blueprint/AIBlueprintHelperLibrary.h"

UBTTask_FindPlayerLocation::UBTTask_FindPlayerLocation(const FObjectInitializer& ObjectInitializer)
{
	NodeName = TEXT("Find Player Location");
}

EBTNodeResult::Type UBTTask_FindPlayerLocation::ExecuteTask(UBehaviorTreeComponent& ownerComp, uint8* nodeMemory)
{
	if (auto* const player = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0))
	{
		auto const playerLocation = player->GetActorLocation();

		if (searchRandom)
		{
			FNavLocation loc;

			if (auto* const navSys = UNavigationSystemV1::GetCurrent(GetWorld()))
			{
				if (navSys->GetRandomPointInNavigableRadius(playerLocation, searchRadius, loc))
				{
					ownerComp.GetBlackboardComponent()->SetValueAsVector(GetSelectedBlackboardKey(), loc.Location);
					FinishLatentTask(ownerComp, EBTNodeResult::Succeeded);
					return EBTNodeResult::Succeeded;
				}
			}
		}
		else
		{
			ownerComp.GetBlackboardComponent()->SetValueAsVector(GetSelectedBlackboardKey(), playerLocation);
			FinishLatentTask(ownerComp, EBTNodeResult::Succeeded);
			return EBTNodeResult::Succeeded;
		}
	}
	return EBTNodeResult::Failed;
}
