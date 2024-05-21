/*
Copyright 2024 Stanislav Kovalchuk
*/

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ExplosionActor.generated.h"

class UParticleSystemComponent;
class USphereComponent;

UCLASS()
class MYPROJECT_API AExplosionActor : public AActor
{
	GENERATED_BODY()
	
public:	
	AExplosionActor();
	virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mesh", meta = (AllowPrivateAccess = "true"))
	USphereComponent* sphereComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Particle", meta = (AllowPrivateAccess = "true"))
	UParticleSystemComponent* explosionParticle;

private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Damaged actors", meta = (AllowPrivateAccess = "true"))
	TArray<AActor*> alreadyDamagedActors;
};
