/*
Copyright 2024 Stanislav Kovalchuk
*/

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HealthActorComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(
	FOnHealthChanged,
	int, newHealth
);


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MYPROJECT_API UHealthActorComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UHealthActorComponent();
	void Init(int maxHealth, int health);
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void SetActorHealth(int newHealth);
	void AddActorHealth(int units, bool notifyActor = true);
	void RemoveActorHealth(int units, bool notifyActor = true);

	UFUNCTION(BlueprintCallable)
	float GetActorHealthPrc();

	UFUNCTION(BlueprintCallable)
	int GetActorHealth();

	UFUNCTION(BlueprintCallable)
	int GetActorMaxHealth();

	UFUNCTION(BlueprintCallable)
	void RestoreHealth();

	UPROPERTY(BlueprintAssignable)
	FOnHealthChanged OnHealthChanged;
protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "INFO", meta = (AllowPrivateAccess = "true"))
	int actorMaxHealth;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "INFO", meta = (AllowPrivateAccess = "true"))
	int actorHealth;
private:
	void NotifyActorHealthChanged();
};
