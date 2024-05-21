/*
Copyright 2024 Stanislav Kovalchuk
*/

#pragma once
#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "ProjectileWeaponAiming.generated.h"


class UWorld;
class UStaticMesh;
class USplineComponent;
class USplineMeshComponent;
class UProjectileShootingComponent;
class UPaperFlipbookComponent;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MYPROJECT_API UProjectileWeaponAiming : public USceneComponent
{
	GENERATED_BODY()
public:	
	UProjectileWeaponAiming();
	void Init(UProjectileShootingComponent* sPtr);

	void OnComponentDestroyed(bool bDestroyingHierarchy);

	bool IsAimOnPlayer();
	void ResetAimOnPlayer();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void AddActorToIgnore(AActor* act);
	void RemoveActorToIgnore(AActor* act);

	void ShowAimTexture();
	void HideAimTexture();

	void ShowPath();
	void HidePath();
protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spline")
	USplineComponent* splineComponent;

	TArray<USplineMeshComponent*> slimeMeshArray;
	TArray<AActor*> addidionalActorsToIgnore;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Aim tex")
	UPaperFlipbookComponent* aimingTextureComp;
private:
	UStaticMesh* cubeMesh;
	UProjectileShootingComponent* shootingComponentPtr;
	bool aimOnPlayer;
	bool drawPath;
	bool showAimTexture;
};
