#include "AI/BTTask_Shot.h"
#include "AIController.h"
#include "NPC/ManTrooperChar.h"
#include "NPC/RoundCatapultPawn.h"
#include "Aiming/ProjectileShootingComponent.h"
#include "Aiming/ProjectileWeaponAiming.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTTask_Shot::UBTTask_Shot(const FObjectInitializer& ObjectInitializer)
{
	NodeName = TEXT("Shot");
}

EBTNodeResult::Type UBTTask_Shot::ExecuteTask(UBehaviorTreeComponent& ownerComp, uint8* nodeMemory)
{
	if (ownerComp.GetAIOwner()->GetPawn()->IsA(ARoundCatapultPawn::StaticClass()))
	{
		ARoundCatapultPawn* pawn = Cast<ARoundCatapultPawn>(ownerComp.GetAIOwner()->GetPawn());
		if (pawn->IsReadyToShot())
		{
			pawn->Shot();
			return EBTNodeResult::Succeeded;
		}

	}
	else if (ownerComp.GetAIOwner()->GetPawn()->IsA(AManTrooperChar::StaticClass()))
	{
		AManTrooperChar* characterPtr = Cast<AManTrooperChar>(ownerComp.GetAIOwner()->GetPawn());
		characterPtr->ShotWithDelay(1.5f, 2.5f);
		return EBTNodeResult::Succeeded;
	}

	return EBTNodeResult::Failed;
}
