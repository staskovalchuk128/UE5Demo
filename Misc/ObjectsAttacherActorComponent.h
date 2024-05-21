/*
Copyright 2024 Stanislav Kovalchuk
*/

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ObjectsAttacherActorComponent.generated.h"

class APickableObject;
class APlayerCharacter;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MYPROJECT_API UObjectsAttacherActorComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UObjectsAttacherActorComponent();
	void Init(APlayerCharacter* parentActor, APickableObject* pickableObject);
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void OnComponentDestroyed(bool bDestroyingHierarchy);

	void OnLMouseDown();
	void OnMouseScroll(float dt);
	void OnMouseWheeleClick();
protected:
	virtual void BeginPlay() override;
private:
	int currentRotationAxis;
	bool loseObjectWhenDestroy;
	FRotator additionalRotation;
	APlayerCharacter* parentActorPtr;
	APickableObject* pickableObjectPtr;
	APickableObject* prevHitObject;
	FName prevSocketName;
	float prevHitDistance;
	float placableDistance;

};
