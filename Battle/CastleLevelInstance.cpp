#include "Battle/CastleLevelInstance.h"
#include "WorldGameMode.h"
#include "NPC/NPCCommon.h"
#include "Kismet/GameplayStatics.h"
#include "Components/SphereComponent.h"
#include "Player/PlayerCharacter.h"
#include "Player/PlayerCatapult.h"

#include "NPC/BalistaPawn.h"
#include "NPC/ManTrooperChar.h"
#include "NPC/RoundCatapultPawn.h"

#include "Player/PlayerCatapult.h"
#include "Player/PlayerCharacter.h"

#include "LevelSequence.h"
#include "LevelSequencePlayer.h"

#include "GameFramework/CharacterMovementComponent.h"

#include "Player/InventoryActorComponent.h"

#include "Battle/CityBuildingsComponent.h"
#include "BPLibs/GeneralBlueprintFunctionLibrary.h"
#include "NPC/CitizenNPC.h"
#include "UI/CaptureCityWidget.h"
#include "UI/UserMessageWidget.h"

#define DISTANCE_TO_DEACTIVATE_BATTLE_WHEN_PLAYER_STARTED	2000.0f

ACastleLevelInstance::ACastleLevelInstance()
{
	PrimaryActorTick.bCanEverTick = true;

	inBattle = false;
	playerStartedBattle = false;
	currentlyShownCaputedCastleAnim = false;
	removePlayerFromCatapultWhenCapture = true;
	merchantInventoryCatapults = CreateDefaultSubobject<UInventoryActorComponent>("Merchant Inventory Catapults");
	merchantInventoryBullets = CreateDefaultSubobject<UInventoryActorComponent>("Merchant Inventory Bullets");
	merchantInventoryArmor = CreateDefaultSubobject<UInventoryActorComponent>("Merchant Inventory Man Armor");
	merchantInventoryManWeapons = CreateDefaultSubobject<UInventoryActorComponent>("Merchant Inventory Man Weapons");
	buildings = CreateDefaultSubobject<UCityBuildingsComponent>("Buildings");

	enterLeaveDetector = CreateDefaultSubobject<USphereComponent>("enterLeaveDetector");
}

void ACastleLevelInstance::BeginPlay()
{
	enterLeaveDetector->AttachToComponent(GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);
	enterLeaveDetector->OnComponentBeginOverlap.AddUniqueDynamic(this, &ACastleLevelInstance::OnOverlapBegin);
	enterLeaveDetector->OnComponentEndOverlap.AddUniqueDynamic(this, &ACastleLevelInstance::OnOverlapEnd);
	Super::BeginPlay();
}

void ACastleLevelInstance::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (playerStartedBattle && !playerInCastleArea)
	{
		APlayerCharacter* playerChar = UGeneralBlueprintFunctionLibrary::GetPlayerChar(GetGameInstance());
		if (FVector::Dist(playerChar->GetActorLocation(), positionWhenPlayerStartedBattle) > DISTANCE_TO_DEACTIVATE_BATTLE_WHEN_PLAYER_STARTED)
		{
			playerChar->OnPlayerLeaveCastle();
		}
	}
}

int ACastleLevelInstance::GetNumOfTroopers()
{
	return troopers.Num();
}

int ACastleLevelInstance::GetNumOfCatapults()
{
	return catapults.Num();
}


void ACastleLevelInstance::ChangeTroopersAIStatus(bool enabled)
{
	for (auto &it: catapults)
	{
		if (ARoundCatapultPawn* catapultPawn = Cast<ARoundCatapultPawn>(it))
		{
			enabled ? catapultPawn->ActivateAI() : catapultPawn->DeactivateAI();
		}

		if (ABalistaPawn* balistaPawn = Cast<ABalistaPawn>(it))
		{
			enabled ? balistaPawn->ActivateAI() : balistaPawn->DeactivateAI();
		}
		
	}

	for (auto& it : troopers)
	{
		if (AManTrooperChar* man = Cast<AManTrooperChar>(it))
		{
			enabled ? man->ActivateAI() : man->DeactivateAI();
		}
	}
}

void ACastleLevelInstance::SetShowTroopersHealthBars(bool res)
{
	for (auto& it : catapults)
	{
		if (ARoundCatapultPawn* catapultPawn = Cast<ARoundCatapultPawn>(it))
		{
			catapultPawn->SetShowHeathBar(res);
		}

		if (ABalistaPawn* balistaPawn = Cast<ABalistaPawn>(it))
		{
			balistaPawn->SetShowHeathBar(res);
		}
	}
	for (auto& it : troopers)
	{
		if (AManTrooperChar* man = Cast<AManTrooperChar>(it))
		{
			man->SetShowHeathBar(res);
		}
	}
}
void ACastleLevelInstance::RestoreTroopersHealth()
{
	for (auto& it : catapults)
	{
		if (ARoundCatapultPawn* catapultPawn = Cast<ARoundCatapultPawn>(it))
		{
			catapultPawn->RestoreHealth();
		}

		if (ABalistaPawn* balistaPawn = Cast<ABalistaPawn>(it))
		{
			balistaPawn->RestoreHealth();
		}
	}
	for (auto& it : troopers)
	{
		if (AManTrooperChar* man = Cast<AManTrooperChar>(it))
		{
			man->RestoreHealth();
		}
	}
}

void ACastleLevelInstance::OnBattleStart()
{
	ChangeTroopersAIStatus(true);
	inBattle = true;
	playerStartedBattle = false;
	SetShowTroopersHealthBars(true);
}

void ACastleLevelInstance::OnBattleEnd()
{
	ChangeTroopersAIStatus(false);
	inBattle = false;
	playerStartedBattle = false;
	SetShowTroopersHealthBars(false);
	RestoreTroopersHealth();
}

void ACastleLevelInstance::SetPlayerPtr(APlayerCharacter* playerP)
{
	playerPtr = playerP;
}

bool ACastleLevelInstance::IsInBattle()
{
	return inBattle;
}

void ACastleLevelInstance::OnPlayerAttackedCastle()
{
	APlayerCharacter* playerChar = UGeneralBlueprintFunctionLibrary::GetPlayerChar(GetGameInstance());
	positionWhenPlayerStartedBattle = playerChar->GetActorLocation();
	playerChar->OnStartBattle(true, this);
	playerStartedBattle = true;
}

void ACastleLevelInstance::OnPlayerCapturedCastle()
{
	AWorldGameMode* gameMode = Cast<AWorldGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
	if (gameMode)
	{
		gameMode->AddPlayersCastle(this);
	}
	
	RemoveAllTroopersFromCastle();

	ownedByPlayer = true;

	APlayerCatapult* catapultPtr = playerPtr->GetCatapultPtr();

	if (removePlayerFromCatapultWhenCapture)
	{
		catapultPtr->OnPlayerExitCatapult();
		playerPtr->TeleportTo(playerPositionWhenCapture, playerRotationWhenCapture);
		catapultPtr->TeleportTo(catapultPositionWhenCapture, catapultRotationWhenCapture);

	}
	else
	{
		playerPtr->EnterCatapult(false);
		catapultPtr->TeleportTo(catapultPositionWhenCapture, catapultRotationWhenCapture);

	}

	
	SpawnCitizents();


	//Start capured castle widget
	FSoftClassPath widgetClassRef(TEXT("/Game/UI/WP_CaptureCityWidget.WP_CaptureCityWidget_C"));
	if (UClass* widgetClass = widgetClassRef.TryLoadClass<UCaptureCityWidget>())
	{
		UCaptureCityWidget* mcWidget = CreateWidget<UCaptureCityWidget>(GetWorld(), widgetClass);
		mcWidget->SetCastlePtr(this);
		Cast<APlayerController>(playerPtr->GetController())->SetInputMode(FInputModeUIOnly());
		mcWidget->AddToViewport(102);
	}
}


void ACastleLevelInstance::RemoveAllTroopersFromCastle()
{
	for (auto& it : catapults)
	{
		if (it->IsValidLowLevel()) it->Destroy();
	}

	for (auto& it : troopers)
	{
		if (it->IsValidLowLevel()) it->Destroy();
	}

	catapults.Empty();
	troopers.Empty();
}

void ACastleLevelInstance::OnNPCDied(NPCCommon* npcPtr)
{
	FString npcType = npcPtr->GetType();

	if (npcType == NPC_TYPE_CATAPULT)
	{
		catapults.Remove(Cast<APawn>(npcPtr->GetActorPtr()));
	}
	else if (npcType == NPC_TYPE_MAN)
	{
		troopers.Remove(Cast<ACharacter>(npcPtr->GetActorPtr()));
	}


	AWorldGameMode* gameMode = Cast<AWorldGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
	if (gameMode) gameMode->OnNpcDied(npcType);

	if (GetNumOfTroopers() + GetNumOfCatapults() <= 0)
	{
		if (!playerPtr)
		{
			playerPtr = UGeneralBlueprintFunctionLibrary::GetPlayerChar(GetGameInstance());
		}
		currentlyShownCaputedCastleAnim = true; // Prevent player from managing city
		playerPtr->OnPlayerLeaveCastle();
		OnPlayerCapturedCastle();
	}
}

void ACastleLevelInstance::OnCapturedCastleAnimationStoped()
{
	StopCityCamera();
	currentlyShownCaputedCastleAnim = false;
	catapultInCastleArea = false;
	playerPtr->OnEndBattle();
	playerPtr->OnPlayerEntersCastle(this); // to turn on manage city icon
}

bool ACastleLevelInstance::IsPlayerInCastle()
{
	return playerInCastle;
}

bool ACastleLevelInstance::IsOwnedByPlayer()
{
	return ownedByPlayer;
}

void ACastleLevelInstance::StartCityCamera_Implementation(float timeLimit)
{

}

void ACastleLevelInstance::StopCityCamera_Implementation()
{

}

void ACastleLevelInstance::CreateBuildings_Implementation()
{
	if (ownedByPlayer)
	{
		SpawnCitizents();
		RemoveAllTroopersFromCastle();
	}
}

const TArray<FInventorySlotItem>& ACastleLevelInstance::GetMerchantCatapultItems()
{
	return merchantInventoryCatapults->GetAllItems();
}

const TArray<FInventorySlotItem>& ACastleLevelInstance::GetMerchantBulletsItems()
{
	return merchantInventoryBullets->GetAllItems();
}

const TArray<FInventorySlotItem>& ACastleLevelInstance::GetMerchantArmorItems()
{
	return merchantInventoryArmor->GetAllItems();
}

const TArray<FInventorySlotItem>& ACastleLevelInstance::GetMerchantManArmorItems()
{
	return merchantInventoryArmor->GetAllItems();
}

TArray<FInventorySlotItem> ACastleLevelInstance::GetMerchantCatapultItemsByType(InventorySlotItemType type)
{
	return merchantInventoryCatapults->GetItemsByType(type);
}

TArray<FInventorySlotItem> ACastleLevelInstance::GetMerchantBulletsItemsByType(InventorySlotItemType type)
{
	return merchantInventoryBullets->GetItemsByType(type);
}

TArray<FInventorySlotItem> ACastleLevelInstance::GetMerchantArmorItemsByType(InventorySlotItemType type)
{
	return merchantInventoryArmor->GetItemsByType(type);
}

TArray<FInventorySlotItem> ACastleLevelInstance::GetMerchantManWeaponsItemsByType(InventorySlotItemType type)
{
	return merchantInventoryManWeapons->GetItemsByType(type);
}

void ACastleLevelInstance::SpawnCitizents()
{
	for (auto &it: citizens)
	{
		FTransform SpawnTransform(it.rotation, it.location);
		ACitizenNPC* citizen = GetWorld()->SpawnActorDeferred<ACitizenNPC>(it.type, SpawnTransform);
		if (citizen != nullptr)
		{
			citizen->SpawnDefaultController();
			UGameplayStatics::FinishSpawningActor(citizen, SpawnTransform);
		}
	}
}

void ACastleLevelInstance::OnPlayerEnterHisOwnCastle()
{
	playerInCastle = true;
}
void ACastleLevelInstance::OnPlayerLeaveHisOwnCastle()
{
	playerInCastle = false;
}


void ACastleLevelInstance::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (currentlyShownCaputedCastleAnim) return; // Prevent player from managing city

	if (OtherActor->IsA(APlayerCharacter::StaticClass()) || OtherActor->IsA(APlayerCatapult::StaticClass()))
	{
		if (OtherActor->IsA(APlayerCatapult::StaticClass()))
		{
			catapultInCastleArea = true;
		}
		

		playerInCastleArea = true;
		APlayerCharacter* player = UGeneralBlueprintFunctionLibrary::GetPlayerChar(GetGameInstance());
		if (player) player->OnPlayerEntersCastle(this);
	}
}

void ACastleLevelInstance::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (currentlyShownCaputedCastleAnim) return; // Prevent player from managing city
	
	if (OtherActor->IsA(APlayerCharacter::StaticClass()) || OtherActor->IsA(APlayerCatapult::StaticClass()))
	{
		APlayerCharacter* player = UGeneralBlueprintFunctionLibrary::GetPlayerChar(GetGameInstance());
		bool playerInCatapult = player->IsInCatapult();


		if (OtherActor->IsA(APlayerCatapult::StaticClass()))
		{
			catapultInCastleArea = false;
		}
		else
		{
			if (catapultInCastleArea && !playerInCatapult)
			{
				UGeneralBlueprintFunctionLibrary::CreateUserMessage(
					GetGameInstance(),
					FText::FromString("Your catapult is still in the castle area, move it out to end the battle"),
					3.0f, UserMessageType::INFO);
			}
		}

		if (catapultInCastleArea) return;
		
		playerInCastleArea = false;
		if(player) player->OnPlayerLeaveCastle();
	}
}