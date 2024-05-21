#include "Misc/ObjectsAttacherActorComponent.h"
#include "Misc/PickableObject.h"
#include "CommonFunctions.h"
#include "Player/PlayerCharacter.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/ArrowComponent.h"
#include "ResourceManager.h"
#include "Materials/MaterialInstanceDynamic.h"

UObjectsAttacherActorComponent::UObjectsAttacherActorComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	currentRotationAxis = 0;
	placableDistance = 500.0f;
	prevHitDistance = 0.0f;
}

void UObjectsAttacherActorComponent::Init(APlayerCharacter* parentActor, APickableObject* pickableObject)
{
	parentActorPtr = parentActor;
	pickableObjectPtr = pickableObject;

	parentActorPtr->SetCenterAimIconVisibility(true);

	parentActorPtr->GetCameraBoom()->SetRelativeLocation(FVector(0.0f, 0.0f, 300.0f));
	parentActorPtr->GetCameraBoom()->TargetArmLength = -100.0f;
	parentActor->SetCameraMovmentScale(0.3f);


	UStaticMeshComponent* mainMesh = pickableObjectPtr->GetMainMesh();

	UMaterialInstanceDynamic* MI = UMaterialInstanceDynamic::Create(pickableObjectPtr->GetMainMesh()->GetMaterial(0), this);
	mainMesh->SetMaterial(0, MI);
	MI->SetScalarParameterValue(TEXT("Opacity"), 0.8f);

	mainMesh->SetSimulatePhysics(false);
	pickableObjectPtr->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);

}

void UObjectsAttacherActorComponent::BeginPlay()
{
	Super::BeginPlay();
	
}
void UObjectsAttacherActorComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();

	int32 ViewportSizeX, ViewportSizeY;
	PlayerController->GetViewportSize(ViewportSizeX, ViewportSizeY);

	auto ScreenLocation = FVector2D(ViewportSizeX, ViewportSizeY);

	FVector WorldLocation;
	FVector WorldDirection;

	if (PlayerController->DeprojectScreenPositionToWorld(ScreenLocation.X / 2.0f, ScreenLocation.Y / 2.0f, WorldLocation, WorldDirection))
	{
		WorldLocation = WorldLocation + (WorldDirection);
		FVector endVector = WorldLocation + ((placableDistance) * WorldDirection);

		TArray<AActor*> ignoredActors;
		ignoredActors.Add(parentActorPtr);
		ignoredActors.Add(pickableObjectPtr);

		FCollisionQueryParams QueryParams;
		QueryParams.AddIgnoredActors(ignoredActors);

		FHitResult hitRes;
		bool res = GetWorld()->LineTraceSingleByChannel(hitRes, WorldLocation, endVector, ECollisionChannel::ECC_WorldStatic, QueryParams);
		if (hitRes.bBlockingHit && IsValid(hitRes.GetActor()) && hitRes.GetActor()->IsA(APickableObject::StaticClass()))
		{
			if (!prevHitObject)
			{
				additionalRotation = FRotator(0.0f);
			}


			APickableObject* poActor = Cast<APickableObject>(hitRes.GetActor());
			prevHitObject = poActor;

			bool alreadyAttachedToPO = poActor->GetAttachParentActor() && poActor->GetAttachParentActor()->IsA(APickableObject::StaticClass());

			UStaticMeshComponent* mainMesh = poActor->GetMainMesh();
			FName closestSocket = CommonFunctions::GetClosestSocket(poActor->GetMainMesh(), hitRes.Location);
			prevSocketName = closestSocket;
			
			pickableObjectPtr->AttachToComponent(mainMesh, FAttachmentTransformRules::KeepRelativeTransform, closestSocket);
			pickableObjectPtr->SetActorRelativeLocation(FVector(0.0f));

			FVector upVector(0.0f, 0.0f, 1.0f);
			FRotator normalRotation = hitRes.Normal.Rotation();

			float dotProduct = upVector.Dot(hitRes.Normal);
			float rotationAngle = FMath::RadiansToDegrees(acosf(dotProduct));

			if (closestSocket == "End1" || closestSocket == "End2")
			{
				FQuat quat;
				
				FRotator poRot = poActor->GetActorRotation();

				FQuat q2 = mainMesh->GetSocketRotation(closestSocket).Quaternion();
				FQuat q1 = poRot.Quaternion();

				VectorQuaternionMultiply(&quat, &q1, &q2);

				normalRotation = FRotator(0.0f);
			}
			else
			{
				FQuat quat;

				FQuat q2 = pickableObjectPtr->GetPlacedRelativeRotation().Quaternion().Inverse();
				FQuat q1 = normalRotation.Quaternion();

				VectorQuaternionMultiply(&quat, &q1, &q2);

				normalRotation = quat.Rotator();
			}

			

			pickableObjectPtr->SetActorRelativeRotation(normalRotation);
			pickableObjectPtr->AddActorLocalRotation(additionalRotation);

			FVector curRot = pickableObjectPtr->GetActorRotation().Vector();
			curRot.Normalize();

			if (pickableObjectPtr->ShouldUseNormalWhenPlace())
			{
				pickableObjectPtr->AddActorWorldOffset(pickableObjectPtr->GetPlacedRelativeLocation() * curRot);
			}
			else
			{
				pickableObjectPtr->AddActorWorldOffset(pickableObjectPtr->GetPlacedRelativeLocation());
			}

		}
		else
		{
			prevHitObject = NULL;

			
			pickableObjectPtr->SetActorLocation(hitRes.Location);

			pickableObjectPtr->SetActorRotation(pickableObjectPtr->GetPlacedOnGroundRelativeRotation());
			pickableObjectPtr->AddActorWorldRotation(hitRes.Normal.Rotation());
			pickableObjectPtr->AddActorWorldRotation(additionalRotation);

			pickableObjectPtr->AddActorWorldOffset(pickableObjectPtr->GetPlacedOnGroundRelativeLocation());


		}

		if (!hitRes.bBlockingHit)
		{
			prevHitDistance = 0.0F;
		}
		else prevHitDistance = hitRes.Distance;

	}
	

	

}

void UObjectsAttacherActorComponent::OnComponentDestroyed(bool bDestroyingHierarchy)
{
	if (!IsValid(this)) return;

	if (!parentActorPtr) return;
	parentActorPtr->SetCenterAimIconVisibility(false);
	parentActorPtr->SetCameraToDefault();


	UStaticMeshComponent* mainMesh = pickableObjectPtr->GetMainMesh();

	//Reset materials
	{
		int matIndex = 0;
		for (auto& it : mainMesh->GetStaticMesh()->GetStaticMaterials())
		{
			mainMesh->SetMaterial(matIndex, it.MaterialInterface);
			matIndex++;
		}
	}

	if (!loseObjectWhenDestroy)
	{
		parentActorPtr->OnPickedupObject(true);
	}
}

void UObjectsAttacherActorComponent::OnMouseScroll(float dt)
{
	float angle = dt * 5.0f;
	if (pickableObjectPtr)
	{
		additionalRotation.Add(currentRotationAxis == 0 ? angle : 0.0f, currentRotationAxis == 1 ? angle : 0.0f, currentRotationAxis == 2 ? angle : 0.0f);
	}
}

void UObjectsAttacherActorComponent::OnMouseWheeleClick()
{
	if (pickableObjectPtr)
	{
		FVector axisToSwitch = pickableObjectPtr->GetAxisToSwitchWhenRotate();
		currentRotationAxis++;
		if (currentRotationAxis == 0 && axisToSwitch.X == 0.0f) currentRotationAxis = 1;
		else if (currentRotationAxis == 1 && axisToSwitch.Y == 0.0f) currentRotationAxis = 2;
		else if (currentRotationAxis == 2 && axisToSwitch.Z == 0.0f) currentRotationAxis = 0;
		if (currentRotationAxis > 2)
		{
			currentRotationAxis = -1;
			OnMouseWheeleClick();
		}
	}
}

void UObjectsAttacherActorComponent::OnLMouseDown()
{
	if (pickableObjectPtr->ShouldEnablePhysicsAfterPlaced())
	{
		pickableObjectPtr->GetMainMesh()->SetSimulatePhysics(true);
	}

	loseObjectWhenDestroy = true;

	if (prevHitDistance == 0.0f)
	{
		loseObjectWhenDestroy = false;
		return;
	}

	if (!prevHitObject)
	{
		pickableObjectPtr->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
		parentActorPtr->OnLostPickedObject();
		return;
	}

	pickableObjectPtr->AttachToComponent(prevHitObject->GetMainMesh(), FAttachmentTransformRules::KeepWorldTransform, prevSocketName);
	
	parentActorPtr->OnLostPickedObject();
	

}
