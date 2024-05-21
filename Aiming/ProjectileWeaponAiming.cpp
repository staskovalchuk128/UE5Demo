#include "Aiming/ProjectileWeaponAiming.h"
#include "Aiming/ProjectileShootingComponent.h"


#include "GameFramework/Character.h"
#include "Components/SplineComponent.h"
#include "Components/SplineMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "ResourceManager.h"


#include "Player/PlayerCatapult.h"
#include "Player/PlayerCharacter.h"

#include "PaperFlipbookComponent.h"
#include "PaperFlipbook.h"

UProjectileWeaponAiming::UProjectileWeaponAiming()
{
	PrimaryComponentTick.bCanEverTick = true;
	splineComponent = CreateDefaultSubobject<USplineComponent>("AimSplineComponent");
	cubeMesh = ConstructorHelpers::FObjectFinder<UStaticMesh>(TEXT("Script/Engine.StaticMesh'/Game/Meshes/Aim/aimCube.aimCube'")).Object;

	aimingTextureComp = CreateDefaultSubobject<UPaperFlipbookComponent>("aimingTextureComp");
	aimingTextureComp->SetFlipbook(ResourceManager::Get<UPaperFlipbook>("aiming_gif"));
	aimingTextureComp->SetWorldScale3D(FVector(0.2f));

	aimOnPlayer = false;
	showAimTexture = false;
}

void UProjectileWeaponAiming::Init(UProjectileShootingComponent* sPtr)
{
	shootingComponentPtr = sPtr;
	drawPath = sPtr->GetParentActorPtr()->IsA(APlayerCharacter::StaticClass());
}

void UProjectileWeaponAiming::OnComponentDestroyed(bool bDestroyingHierarchy)
{
	if(splineComponent) splineComponent->DestroyComponent();
	if (aimingTextureComp)
	{
		aimingTextureComp->UnregisterComponent();
		aimingTextureComp->DestroyComponent();
	}
	for (auto& it : slimeMeshArray)
	{
		it->DestroyComponent();
	}
}



// Called when the game starts
void UProjectileWeaponAiming::BeginPlay()
{
	Super::BeginPlay();
	if(drawPath) splineComponent->RegisterComponent();
}


// Called every frame
void UProjectileWeaponAiming::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);


	FPredictProjectilePathParams inParams;
	FPredictProjectilePathResult pathResult;

	FVector forwardVec = shootingComponentPtr->GetForward();
	float velocity = shootingComponentPtr->GetBulletInitialVelocity();

	inParams.LaunchVelocity = FVector(velocity) * forwardVec;
	inParams.StartLocation = shootingComponentPtr->GetBulletStartPosition();
	//inParams.DrawDebugType = EDrawDebugTrace::ForOneFrame;
	inParams.ProjectileRadius = shootingComponentPtr->GetBulletSize().GetSphere().W;
	inParams.MaxSimTime = 6.0f;
	inParams.ActorsToIgnore.Add(shootingComponentPtr->GetParentActorPtr());
	for (auto& it : addidionalActorsToIgnore)
	{
		inParams.ActorsToIgnore.Add(it);
	}
	inParams.bTraceWithCollision = true;
	inParams.bTraceWithChannel = true;

	bool wasHit = UGameplayStatics::PredictProjectilePath(this, inParams, pathResult);

	if (wasHit && (pathResult.HitResult.GetActor()->IsA(APlayerCatapult::StaticClass())
		|| pathResult.HitResult.GetActor()->IsA(APlayerCharacter::StaticClass())))
	{
		aimOnPlayer = true;
	}
	else aimOnPlayer = false;

	if (showAimTexture)
	{

		aimingTextureComp->SetWorldLocation(pathResult.HitResult.Location);

		// Set rotation
		{
			FVector upVector(0.0f, 0.0f, 1.0f);

			FVector rotationAxis = upVector.Cross(pathResult.HitResult.Normal);
			rotationAxis.Normalize();
			if (rotationAxis.Size() == 0)
			{
				rotationAxis.Z = 1.0f;
			}

			float dotProduct = upVector.Dot(pathResult.HitResult.Normal);
			float rotationAngle = FMath::RadiansToDegrees(acosf(dotProduct));

			FVector zeroVector(0.0f, 0.0f, 90.0f);
			FVector rotatedVector = zeroVector.RotateAngleAxis(rotationAngle, rotationAxis);
			FRotator newRotation = FRotator(rotatedVector.Y, rotatedVector.X, rotatedVector.Z);
			aimingTextureComp->SetWorldRotation(newRotation);
		}

	}

	
	
	
	
	if (drawPath)
	{
		splineComponent->ClearSplinePoints();

		int key = 0;
		for (auto& it : pathResult.PathData)
		{
			splineComponent->AddPoint(FSplinePoint(key, it.Location), true);
			key++;

		}
		if (slimeMeshArray.Num() == 0)
		{
			for (int i = 0; i < 50; i++)
			{
				USplineMeshComponent* splineMeshComponet = NewObject<USplineMeshComponent>(this);
				splineMeshComponet->RegisterComponent();
				splineMeshComponet->Mobility = EComponentMobility::Movable;
				splineMeshComponet->SetStaticMesh(cubeMesh);
				//splineMeshComponet->SetStartScale(FVector2D(0.03f, 0.06f));
				//splineMeshComponet->SetEndScale(FVector2D(0.03f, 0.06f));

				float aimStaticRadius = splineMeshComponet->GetStaticMesh()->GetBounds().GetSphere().W;

				splineMeshComponet->SetStartScale(FVector2D(inParams.ProjectileRadius / aimStaticRadius));
				splineMeshComponet->SetEndScale(FVector2D(inParams.ProjectileRadius / aimStaticRadius));

				splineMeshComponet->SetStartPosition(splineComponent->GetLocationAtSplineInputKey(i, ESplineCoordinateSpace::Local));
				splineMeshComponet->SetStartTangent(splineComponent->GetDirectionAtSplineInputKey(i, ESplineCoordinateSpace::Local));

				splineMeshComponet->SetEndPosition(splineComponent->GetLocationAtSplineInputKey(i + 1, ESplineCoordinateSpace::Local));
				splineMeshComponet->SetEndTangent(splineComponent->GetDirectionAtSplineInputKey(i + 1, ESplineCoordinateSpace::Local));


				slimeMeshArray.Add(splineMeshComponet);

			}
		}
		else
		{
			int numOfPoints = splineComponent->GetNumberOfSplinePoints();
			key = 0;
			for (auto& it : slimeMeshArray)
			{


				it->SetStartPosition(splineComponent->GetLocationAtSplineInputKey(key, ESplineCoordinateSpace::Local));
				it->SetStartTangent(splineComponent->GetDirectionAtSplineInputKey(key, ESplineCoordinateSpace::Local));

				it->SetEndPosition(splineComponent->GetLocationAtSplineInputKey(key + 1, ESplineCoordinateSpace::Local));
				it->SetEndTangent(splineComponent->GetDirectionAtSplineInputKey(key + 1, ESplineCoordinateSpace::Local));
				key++;
			}
		}


	}

}


bool UProjectileWeaponAiming::IsAimOnPlayer()
{
	return aimOnPlayer;
}

void UProjectileWeaponAiming::ResetAimOnPlayer()
{
	aimOnPlayer = false;
}

void UProjectileWeaponAiming::AddActorToIgnore(AActor* act)
{
	addidionalActorsToIgnore.Add(act);
}
void UProjectileWeaponAiming::RemoveActorToIgnore(AActor* act)
{
	addidionalActorsToIgnore.Remove(act);
}

void UProjectileWeaponAiming::ShowAimTexture()
{
	showAimTexture = true;
	aimingTextureComp->RegisterComponent();
	aimingTextureComp->SetVisibility(true);
}

void UProjectileWeaponAiming::HideAimTexture()
{
	showAimTexture = false;
	aimingTextureComp->UnregisterComponent();
	aimingTextureComp->SetVisibility(false);
}

void UProjectileWeaponAiming::ShowPath()
{
	drawPath = true;
	if (!splineComponent)
	{
		splineComponent = NewObject<USplineComponent>(this);
	}
}

void UProjectileWeaponAiming::HidePath()
{
	drawPath = false;
	if (splineComponent)
	{
		splineComponent->DestroyComponent();
		splineComponent->UnregisterComponent();
	}
	for (auto& it : slimeMeshArray)
	{
		it->DestroyComponent();
	}
	slimeMeshArray.Empty();
	splineComponent = NULL;
}
