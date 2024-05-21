/*
Copyright 2024 Stanislav Kovalchuk
*/

#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PickableObject.generated.h"

UENUM(BlueprintType)
enum class PickableObjectType : uint8
{
	THROWABLE UMETA(DisplayName = "Throwable"),
	MELEE_HIT UMETA(DisplayName = "Melee hit"),
	PLACEABLE  UMETA(DisplayName = "Placable")
};


class UBlendSpace;
class UAnimMontage;
class UBillboardComponent;
class UProjectileMovementComponent;
class UArrowComponent;

UCLASS()
class MYPROJECT_API APickableObject : public AActor
{
	GENERATED_BODY()
	
public:	
	APickableObject();

	virtual void Tick(float DeltaTime) override;

	void OnObjectPicked(UChildActorComponent* obj, APawn* ownerPawn);

	bool CanPick();

	PickableObjectType GetType();

	UFUNCTION(BlueprintCallable)
	UBlendSpace* GetCarryAnimation();

	UFUNCTION(BlueprintCallable)
	UAnimMontage* GetPickAnimataion();


	UStaticMeshComponent* GetMainMesh();
	FVector GetPlacedRelativeLocation();
	FVector GetPlacedOnGroundRelativeLocation();
	FVector GetAxisToSwitchWhenRotate();
	FRotator GetPlacedRelativeRotation();
	FRotator GetPlacedOnGroundRelativeRotation();
	bool ShouldUseNormalWhenPlace();
	bool ShouldEnablePhysicsAfterPlaced();

	void OnThrow(FVector velocity, APawn* ownerPtr, const FVector& forwardVec);

	void OnObjectLost();
	void SetPlayerCanPick(bool v);

	void ResetPositionToPlayer();
protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnHit(class UPrimitiveComponent* HitComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	bool needToRemove;
	bool picked;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pickable", meta = (AllowPrivateAccess = "true"))
	bool playerCanPick;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Respawnable", meta = (AllowPrivateAccess = "true"))
	bool respawnAfterDestory;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Respawnable", meta = (AllowPrivateAccess = "true"))
	float removeTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Billboard", meta = (AllowPrivateAccess = "true"))
	UBillboardComponent* pickIconBillboard;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mesh", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* mainMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Behavior", meta = (AllowPrivateAccess = "true"))
	bool enablePhysicsAfterPlaced;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Transform", meta = (AllowPrivateAccess = "true"))
	FVector pickedRelativeLocation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Transform", meta = (AllowPrivateAccess = "true"))
	FRotator pickedRelativeRotation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Transform", meta = (AllowPrivateAccess = "true"))
	FVector pickedRelativeScale;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Transform", meta = (AllowPrivateAccess = "true"))
	bool useNormalWhenPlace;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Transform", meta = (AllowPrivateAccess = "true"))
	FVector axisToSwitchWhenRotate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Transform", meta = (AllowPrivateAccess = "true"))
	FVector placedRelativeLocation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Transform", meta = (AllowPrivateAccess = "true"))
	FVector placedOnGroundRelativeLocation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Transform", meta = (AllowPrivateAccess = "true"))
	FRotator placedRelativeRotation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Transform", meta = (AllowPrivateAccess = "true"))
	FRotator placedOnGroundRelativeRotation;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Type", meta = (AllowPrivateAccess = "true"))
	PickableObjectType type;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Proj", meta = (AllowPrivateAccess = "true"))
	UProjectileMovementComponent* projMoveComp;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Anim", meta = (AllowPrivateAccess = "true"))
	UAnimMontage* pickAnimation;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Anim", meta = (AllowPrivateAccess = "true"))
	UBlendSpace* carryAnimation;
private:
	float collidedTimer;
	FVector beginLocation, beginScale;
	FRotator beginRotation;
	APawn* ownerPawnPtr;
	UChildActorComponent* objectHolderPtr;

	//Prev collision
	FName prevCollisionProfile;
	ECollisionEnabled::Type prevCollisionEnabled;
};
