#include "AI/BTTask_AimOnPlayer.h"
#include "AIController.h"
#include "NPC/RoundCatapultPawn.h"
#include "Catapult/CatapultGun.h"
#include "Aiming/ProjectileShootingComponent.h"
#include "Aiming/ProjectileWeaponAiming.h"
#include "Kismet/GameplayStatics.h"

UBTTask_AimOnPlayer::UBTTask_AimOnPlayer(const FObjectInitializer& ObjectInitializer)
{
	NodeName = TEXT("Aim on Player");
}

EBTNodeResult::Type UBTTask_AimOnPlayer::ExecuteTask(UBehaviorTreeComponent& ownerComp, uint8* nodeMemory)
{
	
	if (ownerComp.GetAIOwner()->GetPawn()->IsA(ARoundCatapultPawn::StaticClass()))
	{
		ARoundCatapultPawn* pawn = Cast<ARoundCatapultPawn>(ownerComp.GetAIOwner()->GetPawn());
		ACatapultGun* catapultGunPtr = pawn->GetCatapultGun();
		
		
		if (catapultGunPtr->GetState() == ProjectileGunState::RELEASING_GUN) return EBTNodeResult::Failed; // shoting right now, no need for any actions


		if (catapultGunPtr->GetAiming()->IsAimOnPlayer()) {
			catapultGunPtr->StopPullingGun();
			if (catapultGunPtr->GetState() == ProjectileGunState::READY) {
				pawn->Shot();
			}
			return EBTNodeResult::Succeeded;
		}
	
		APawn* player = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);

		FVector bulletStartPos = catapultGunPtr->GetConstBulletStartPosition();
		FVector playersPos = player->GetActorLocation();


		float neededInitalSpeed = 0.0f;
		if (catapultGunPtr->IsProjectile())
		{
			UProjectileShootingComponent* shootingPtr = catapultGunPtr->GetShootingComponent();

			
			float initialVelocity = shootingPtr->GetBulletInitialVelocity();

			TArray<AActor*> igoreActors;
			igoreActors.Add(ownerComp.GetAIOwner()->GetPawn());
			FVector vel;
			bool r = UGameplayStatics::SuggestProjectileVelocity(ownerComp.GetAIOwner()->GetPawn(), vel, bulletStartPos, playersPos, initialVelocity, false, 0.1f, 0.0f, ESuggestProjVelocityTraceOption::DoNotTrace, FCollisionResponseParams::DefaultResponseParam, igoreActors);
			neededInitalSpeed = vel.Size();
			FRotator angle = vel.Rotation();


			const float& minTen = shootingPtr->GetMinAimTension();
			const float& maxTen = shootingPtr->GetMaxAimTension();


			float prc = abs((angle.Pitch + abs(minTen)) / (minTen - maxTen));

			float finalTen = prc * (abs(maxTen) + abs(minTen));


			shootingPtr->SetAimPitch(finalTen);

			return EBTNodeResult::Succeeded;
		}
		else
		{
			FVector vel;
			bool r = UGameplayStatics::SuggestProjectileVelocity_CustomArc(this, vel, bulletStartPos, playersPos, 0.0f, 0.5f);
			neededInitalSpeed = vel.Size();
		}



		const float& minConstVel = catapultGunPtr->GetBulletMinVelocity();
		const float& maxConstVel = catapultGunPtr->GetBulletMaxVelocity();

		if (neededInitalSpeed < minConstVel || neededInitalSpeed > maxConstVel)
		{
			return EBTNodeResult::Failed;
		}
		float velPrc = (neededInitalSpeed - minConstVel) / (maxConstVel - minConstVel);
		float tenPrc = catapultGunPtr->GetTensionPrc();

		float prcDiff = abs(velPrc - tenPrc);

		float finalTen = velPrc * catapultGunPtr->GetMaxGunTension();
		
		if (prcDiff <= 1.0f) {
			catapultGunPtr->SetGunTension(finalTen);
			return EBTNodeResult::Failed;
		}

		if (finalTen > catapultGunPtr->GetGunTension())
		{
			catapultGunPtr->PullGun();
		}
		else
		{
			catapultGunPtr->UnPullGun();
		}
		

	}

	return EBTNodeResult::Failed;
}
