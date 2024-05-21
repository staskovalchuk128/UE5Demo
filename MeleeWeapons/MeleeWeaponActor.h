/*
Copyright 2024 Stanislav Kovalchuk
*/

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MeleeWeaponActor.generated.h"

struct MeleeWeaponData;
class UParticleSystemComponent;

UCLASS()
class MYPROJECT_API AMeleeWeaponActor : public AActor
{
	GENERATED_BODY()
	
public:
	AMeleeWeaponActor();

	void Init(const MeleeWeaponData& weaponData, APawn* ownerPtr);

	virtual void Tick(float DeltaTime) override;

	UStaticMeshComponent* GetWeaponMeshComponent();

	void SetCanDamage(bool v);
	bool CanDamage();
protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnParticleExpired(class UParticleSystemComponent* PSystem);


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mesh", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* weaponMeshComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage", meta = (AllowPrivateAccess = "true"))
	bool canDamage;

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Particle", meta = (AllowPrivateAccess = "true"))
	TArray<UParticleSystemComponent*> hitParticles;

	int damage;
	APawn* ownerActor;

};
