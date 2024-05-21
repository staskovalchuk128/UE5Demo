/*
Copyright 2024 Stanislav Kovalchuk
*/

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CatapultRotator.generated.h"

UCLASS()
class MYPROJECT_API ACatapultRotator : public AActor
{
	GENERATED_BODY()
	
public:	
	ACatapultRotator();

	UFUNCTION(BlueprintCallable)
	void Init(USkeletalMeshComponent* parentMesh);
	virtual void Tick(float DeltaTime) override;

	USkeletalMeshComponent* GetRotationMeshComponent();
protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Mesh", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* rotationBaseMeshComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Mesh", meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* rotationMeshComponent;
};
