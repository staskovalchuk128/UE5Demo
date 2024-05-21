/*
Copyright 2024 Stanislav Kovalchuk
*/

#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameBulletActor.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(
	FOnBulletRemove,
	AActor*, actor
);

struct BulletData;
class UProjectileMovementComponent;
class UNiagaraSystem;

UCLASS()
class MYPROJECT_API AGameBulletActor : public AActor
{
	GENERATED_BODY()
public:	
	AGameBulletActor();

	void Init(const BulletData& bulletData, FVector bulletForce, APawn* ownerPtr, const FVector& forwardVec);
	virtual void Tick(float DeltaTime) override;
	
	bool CanDamage();
	int GetDamage();

	void RemoveDamage();

	UStaticMeshComponent* GetBulletMeshComponent();

	UFUNCTION()
	void OnHit(class UPrimitiveComponent* HitComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION(BlueprintCallable)
	bool IsHeavyBullet();

	void EnablePhysics();
	void DisablePhysics();

	UFUNCTION(BlueprintCallable)
	FVector GetApplyedForce();

	UFUNCTION(BlueprintCallable)
	bool IsCollided();

	UFUNCTION(BlueprintCallable)
	void RemoveImmediately();

	UPROPERTY(BlueprintAssignable)
	FOnBulletRemove OnBulletRemove;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mesh", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* bulletMeshComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mesh", meta = (AllowPrivateAccess = "true"))
	bool physicsDisabled;

private:
	bool needRemoveImmediately;
	bool heavyBullet;
	float lifeTimeAfterCollision;
	float lifeTimeAfterCollisionPast;
	bool collided;
	bool useProjectveMove;
	int damage;
	FVector bulletForce;
	APawn* ownerActor;



	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Proj", meta = (AllowPrivateAccess = "true"))
	UProjectileMovementComponent* projMoveComp;
};
