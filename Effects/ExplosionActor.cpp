#include "Effects/ExplosionActor.h"
#include "Components/SphereComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "ResourceManager.h"

#include "NPC/ManTrooperChar.h"

AExplosionActor::AExplosionActor()
{
	PrimaryActorTick.bCanEverTick = true;
	sphereComponent = CreateDefaultSubobject<USphereComponent>("Sphere");
	sphereComponent->SetGenerateOverlapEvents(true);
	sphereComponent->SetSimulatePhysics(false);
	sphereComponent->SetWorldScale3D(FVector(15.0f));
	sphereComponent->bMultiBodyOverlap = true;

	explosionParticle = CreateDefaultSubobject<UParticleSystemComponent>("explosionParticle");
	explosionParticle->SetTemplate(ResourceManager::Get<UParticleSystem>("explosionParticle"));
	explosionParticle->CustomTimeDilation = 0.5f;
	explosionParticle->SetWorldScale3D(FVector(7.0f));

	sphereComponent->OnComponentBeginOverlap.AddUniqueDynamic(this, &AExplosionActor::OnOverlapBegin);
}

void AExplosionActor::BeginPlay()
{
	Super::BeginPlay();

	explosionParticle->SetWorldLocation(GetActorLocation());
}

void AExplosionActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


void AExplosionActor::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (alreadyDamagedActors.Num() > 0 && alreadyDamagedActors.Find(OtherActor)) return;

	if (OtherActor->IsA(AManTrooperChar::StaticClass()))
	{
		AManTrooperChar* manTrooper = Cast<AManTrooperChar>(OtherActor);

		FVector norm = (OtherActor->GetActorLocation() - GetActorLocation());
		norm.Normalize();
		
		FRotator actorRotation = manTrooper->GetActorRotation();
		actorRotation.Add(0.0f, FMath::RadiansToDegrees(GetActorLocation().CosineAngle2D(OtherActor->GetActorLocation())), 0.0f);
		manTrooper->SetActorRotation(actorRotation);
		manTrooper->DamageActor(15);
		manTrooper->ThrowNpc(FVector(1000000.0f) * norm);

		alreadyDamagedActors.Add(OtherActor);
	}
}