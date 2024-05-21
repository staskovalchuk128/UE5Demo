#include "Catapult/CatapultRotator.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Engine/SkeletalMesh.h"
#include "ResourceManager.h"

ACatapultRotator::ACatapultRotator()
{
	PrimaryActorTick.bCanEverTick = true;

	UStaticMesh* mesh = ResourceManager::Get<UStaticMesh>("simpleCatapultRotatorBase");
	rotationBaseMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>("rotationMeshBaseComponent");
	rotationBaseMeshComponent->SetStaticMesh(mesh);
	rotationBaseMeshComponent->SetRelativeLocation(FVector(-230.0f, 0.0f, 47.0f));
	rotationBaseMeshComponent->SetRelativeScale3D(FVector(1.4f, 1.0f, 1.0f));

	USkeletalMesh* meshSocket = ResourceManager::Get<USkeletalMesh>("simpleCatapultRotator");
	rotationMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>("rotationMeshComponent");
	rotationMeshComponent->SetSkeletalMesh(meshSocket);
	rotationMeshComponent->SetRelativeLocation(FVector(-215.0f, 0.0f, 47.0f));

	USkeletalMeshSocket* socket = NewObject<USkeletalMeshSocket>(meshSocket);
	socket->SocketName = "GunSocket";
	socket->BoneName = "Bone";
	socket->RelativeLocation = FVector(0.0f, -1.0f, 61.0f);
	socket->RelativeRotation = FRotator(0.0f, 0.0f, 0.0f);
	//if (!meshSocket->FindSocket("GunSocket")) meshSocket->AddSocket(socket);
}

void ACatapultRotator::Init(USkeletalMeshComponent* parentMesh)
{
	rotationBaseMeshComponent->AttachToComponent(parentMesh, FAttachmentTransformRules::KeepRelativeTransform);
	rotationMeshComponent->AttachToComponent(parentMesh, FAttachmentTransformRules::KeepRelativeTransform);
}

void ACatapultRotator::BeginPlay()
{
	Super::BeginPlay();
	
}
void ACatapultRotator::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


USkeletalMeshComponent* ACatapultRotator::GetRotationMeshComponent()
{
	return rotationMeshComponent;
}
