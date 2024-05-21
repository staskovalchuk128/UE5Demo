/*
Copyright 2024 Stanislav Kovalchuk
*/

#pragma once

#include "CoreMinimal.h"

class USkeletalMeshSocket;
class USkeletalMeshComponent;
class UStaticMeshSocket;
class UStaticMeshComponent;


class MYPROJECT_API CommonFunctions
{
public:
	static FName GetClosestSocket(const USkeletalMeshComponent* mesh, const FVector& location);
	static FName GetClosestSocket(const UStaticMeshComponent* mesh, const FVector& location);
};
