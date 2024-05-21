#include "NPC/NPCCommon.h"
#include "NPC/NPCHealthActor.h"
#include "Battle/CastleLevelInstance.h"
#include "Character/HealthActorComponent.h"
#include "UI/NPCHealthWidget.h"
#include "Components/WidgetComponent.h"

NPCCommon::NPCCommon()
{
}

NPCCommon::~NPCCommon()
{
}

void NPCCommon::OnDestroyedNPC()
{
	if (castleInstancePtr) castleInstancePtr->OnNPCDied(this);
}

void NPCCommon::RestoreHealth()
{
	healthComponentPtr->RestoreHealth();
}


void NPCCommon::SetShowHeathBar(bool show)
{
	if (!healthBarWidget || !healthBarWidgetRoot) return;
	healthBarWidgetRoot->SetVisibility(show, true);
}

void NPCCommon::InitNPC(FString npcTypeStr, APawn* actor, UHealthActorComponent* healthComp, bool showHealthBar, UWidgetComponent* healthBarWidgetRootPtr, ACastleLevelInstance* castleInstance)
{
	this->healthBarWidgetRoot = healthBarWidgetRootPtr;
	this->healthBarWidget = Cast<UNPCHealthWidget>(healthBarWidgetRoot->GetWidget());
	this->castleInstancePtr = castleInstance;
	this->npcType = npcTypeStr;
	this->actorPtr = actor;
	this->healthComponentPtr = healthComp;
	this->worldPtr = actor->GetWorld();

	healthBarWidgetRoot->RequestRenderUpdate();
	healthBarWidget->UpdateHealth(healthComponentPtr->GetActorHealthPrc()); // set health

	if (showHealthBar)
	{
		SetShowHeathBar(showHealthBar);
	}
	FVector origin;
	actor->GetActorBounds(false, origin, actorSize);
	actorHealthOffsetZ = actorSize.Z / 2.0f;
}

AActor* NPCCommon::GetActorPtr()
{
	return actorPtr;
}

void NPCCommon::OnDamaged()
{
	if (healthBarWidget) healthBarWidget->UpdateHealth(healthComponentPtr->GetActorHealthPrc());

	if (castleInstancePtr && !castleInstancePtr->IsInBattle())
	{
		castleInstancePtr->OnPlayerAttackedCastle();
	}
}
FString NPCCommon::GetType()
{
	return npcType;
}

