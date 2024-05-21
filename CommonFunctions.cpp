#include "CommonFunctions.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Engine/StaticMeshSocket.h"

FName CommonFunctions::GetClosestSocket(const USkeletalMeshComponent* mesh, const FVector& location)
{
	TArray<USkeletalMeshSocket*> socketNames = mesh->GetSkeletalMeshAsset()->GetActiveSocketList();

	float minDist = -1.0f;
	FName closestSocket = "";

	for (auto it : socketNames)
	{
		float dist = FVector::Dist(mesh->GetSocketLocation(it->SocketName), location);

		if (minDist == -1.0f)
		{
			closestSocket = it->SocketName;
			minDist = dist;
		}
		else if (dist < minDist)
		{
			closestSocket = it->SocketName;
			minDist = dist;
		}
	}

	return closestSocket;
}

FName CommonFunctions::GetClosestSocket(const UStaticMeshComponent* mesh, const FVector& location)
{
	float minDist = -1.0f;
	FName closestSocket = "";

	TArray<FName> socketNames = mesh->GetAllSocketNames();

	for (auto it : socketNames)
	{
		float dist = FVector::Dist(mesh->GetSocketLocation(it), location);

		if (minDist == -1.0f)
		{
			closestSocket = it;
			minDist = dist;
		}
		else if (dist < minDist)
		{
			closestSocket = it;
			minDist = dist;
		}
	}

	return closestSocket;
}