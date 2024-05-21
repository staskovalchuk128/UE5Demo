#include "MeleeWeapons/MeleeWeaponActor.h"
#include "MeleeWeapons/MeleeWeaponsManager.h"
#include "Particles/ParticleSystemComponent.h"

#include "ResourceManager.h"

#include "NPC/ManTrooperChar.h"
#include "Player/PlayerCharacter.h"

// Sets default values
AMeleeWeaponActor::AMeleeWeaponActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	weaponMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>("weaponMeshComp");
	weaponMeshComponent->SetSimulatePhysics(false);
	SetRootComponent(weaponMeshComponent);

	weaponMeshComponent->SetGenerateOverlapEvents(true);



}


void AMeleeWeaponActor::Init(const MeleeWeaponData& weaponData, APawn* ownerPtr)
{
	this->ownerActor = ownerPtr;
	damage = weaponData.damage;

	// ignore self actor collision
	{
		ownerPtr->MoveIgnoreActorAdd(this);
		weaponMeshComponent->MoveIgnoreActors.Add(ownerPtr);
	}
}


void AMeleeWeaponActor::BeginPlay()
{
	Super::BeginPlay();
	
	weaponMeshComponent->OnComponentBeginOverlap.AddUniqueDynamic(this, &AMeleeWeaponActor::OnOverlapBegin);
}

void AMeleeWeaponActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

UStaticMeshComponent* AMeleeWeaponActor::GetWeaponMeshComponent()
{
	return weaponMeshComponent;
}

bool AMeleeWeaponActor::CanDamage()
{
	return canDamage;
}

void AMeleeWeaponActor::SetCanDamage(bool v)
{
	canDamage = v;
}

void AMeleeWeaponActor::OnParticleExpired(class UParticleSystemComponent* PSystem)
{
	hitParticles.Remove(PSystem);
	if(PSystem) PSystem->DestroyComponent();
}

void AMeleeWeaponActor::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!CanDamage()) return;
	if (GetOwner() == OtherActor) return;

	if (OtherActor->IsA(AManTrooperChar::StaticClass()) || OtherActor->IsA(APlayerCharacter::StaticClass()))
	{

		if (OtherActor->IsA(AManTrooperChar::StaticClass()))
		{
			Cast<AManTrooperChar>(OtherActor)->DamageActor(damage);
		}
		else
		{
			Cast<APlayerCharacter>(OtherActor)->DamagePlayer(damage);
		}

		
		FVector leftVector = OtherActor->GetActorRotation().Vector().RotateAngleAxis(90.0f, FVector(0.0f, -1.0f, 0.0f));

		UParticleSystemComponent* particle = NewObject<UParticleSystemComponent>(this, UParticleSystemComponent::StaticClass());
		particle->SetTemplate(ResourceManager::Get<UParticleSystem>("manBloodParticle"));
		particle->SetWorldScale3D(FVector(0.6f));
		particle->RegisterComponentWithWorld(GetWorld());
		particle->SetWorldLocation(OtherActor->GetActorLocation());
		particle->SetWorldRotation(FRotator(90.0f * leftVector.X, 90.0f * leftVector.Y, 90.0f * leftVector.Z));
		particle->Activate();
		particle->ActivateSystem(true);
		particle->OnSystemFinished.AddUniqueDynamic(this, &AMeleeWeaponActor::OnParticleExpired);

		hitParticles.Add(particle);

		SetCanDamage(false);
	}

}