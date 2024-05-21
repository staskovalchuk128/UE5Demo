/*
Copyright 2024 Stanislav Kovalchuk
*/

#pragma once
#include "CoreMinimal.h"


class UNPCHealthWidget;
class APawn;
class UHealthActorComponent;
class ACastleLevelInstance;
class UWidgetComponent;


#define NPC_TYPE_CATAPULT	"catapult"
#define NPC_TYPE_MAN		"man"

class MYPROJECT_API NPCCommon
{
public:
	NPCCommon();
	~NPCCommon();

	void InitNPC(FString npcTypeStr, APawn* actor, UHealthActorComponent* healthComp, bool showHealthBar, UWidgetComponent* healthBarWidgetRootPtr, ACastleLevelInstance* castleInstancePtr = NULL);
	
	void OnDamaged();

	void OnDestroyedNPC();
	AActor* GetActorPtr();
	FString GetType();
	void RestoreHealth();
	void SetShowHeathBar(bool show);
private:
	ACastleLevelInstance* castleInstancePtr;
	UWidgetComponent* healthBarWidgetRoot;
	UNPCHealthWidget* healthBarWidget;
	UHealthActorComponent* healthComponentPtr;
	APawn* actorPtr;
	UObject* worldPtr;
	FString npcType;
	FVector actorSize;
	float actorHealthOffsetZ;
};
