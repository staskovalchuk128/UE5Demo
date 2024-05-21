#include "Character/HealthActorComponent.h"

UHealthActorComponent::UHealthActorComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UHealthActorComponent::Init(int maxHealth, int health)
{
	actorMaxHealth = maxHealth;
	actorHealth = health;
}


void UHealthActorComponent::BeginPlay()
{
	Super::BeginPlay();
	
}


void UHealthActorComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UHealthActorComponent::SetActorHealth(int newHealth)
{
	actorHealth = newHealth;
	actorHealth = actorHealth > actorMaxHealth ? actorMaxHealth : actorHealth;
	actorHealth = actorHealth < 0 ? 0 : actorHealth;
}

void UHealthActorComponent::AddActorHealth(int units, bool notifyActor)
{
	SetActorHealth(actorHealth + units);
	if (notifyActor)
	{
		NotifyActorHealthChanged();
	}
}

void UHealthActorComponent::RemoveActorHealth(int units, bool notifyActor)
{
	SetActorHealth(actorHealth - units);
	if (notifyActor)
	{
		NotifyActorHealthChanged();
	}
}

void UHealthActorComponent::RestoreHealth()
{
	actorHealth = actorMaxHealth;
}

int UHealthActorComponent::GetActorMaxHealth()
{
	return actorMaxHealth;
}

int UHealthActorComponent::GetActorHealth()
{
	return actorHealth;
}

float UHealthActorComponent::GetActorHealthPrc()
{
	return ((float)actorHealth / (float)actorMaxHealth);
}

void UHealthActorComponent::NotifyActorHealthChanged()
{
	OnHealthChanged.Broadcast(actorHealth);
}