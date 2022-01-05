#include "PS1ProjectileActor.h"
#include "Components/CapsuleComponent.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "PS1/Components/HealthComponent/PS1HealthComponent.h"
#include "PS1/Components/WeaponComponent/PS1WeaponComponent.h"
#include "PS1/Weapons/DataAssets/PS1ProjectileDataAsset.h"

#define E_PHYSICAL_SURFACE_DIRT EPhysicalSurface::SurfaceType2;

APS1ProjectileActor::APS1ProjectileActor()
{
	PrimaryActorTick.bCanEverTick = true;

	CapsuleComponent = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Capsule"));
	RootComponent = CapsuleComponent;
	CapsuleComponent->bReturnMaterialOnMove = true;
	
	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	StaticMeshComponent->SetupAttachment(RootComponent);
}

void APS1ProjectileActor::BeginPlay()
{
	Super::BeginPlay();
}

void APS1ProjectileActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if(!ProjectileDataAsset)
	{
		UE_LOG(LogTemp, Warning, TEXT("Projectile Actor is missing a Projetile Data Asset"));
		return;
	}

	if(Timer >= ProjectileDataAsset->LifeTime)
	{
		if(!IsActorBeingDestroyed())
		{
			Destroy();
		}
	}

	float SpeedMultiplier = 1;
	if(ProjectileDataAsset->SpeedMultiplierCurve)
	{
		SpeedMultiplier = ProjectileDataAsset->SpeedMultiplierCurve->GetFloatValue(Timer);
	}
	
	FHitResult Hit;
	AddActorWorldOffset(GetActorForwardVector() * (ProjectileDataAsset->Speed * DeltaTime * SpeedMultiplier), true, &Hit);
	
	if(Hit.bBlockingHit)
	{
		FAttackInfo AttackInfo;
		AttackInfo.bProjectile = true;

		if(Hit.Actor != nullptr && Hit.Actor != OwnerWeaponComponent->GetOwner())
		{
			TArray<UPS1HealthComponent*> Components;
			
			AttackInfo.ActorsHit.Add(Hit.Actor.Get());
			AttackInfo.HitLocations.Add(Hit.ImpactPoint);
			AttackInfo.HitLocations.Add(Hit.ImpactNormal);
			
			Hit.Actor->GetComponents(Components);
	
			for(UPS1HealthComponent* HealthComp : Components)
			{
				AttackInfo.bAttackHit = true;
				
				if(!ProjectileOwner)
				{
					return;
				}

				UPhysicalMaterial* PhyMatSurface = nullptr;
			
				if(OwnerWeaponComponent != nullptr)
				{
					OwnerWeaponComponent->OnAttack.Broadcast(AttackInfo);
				}
				
				HealthComp->Damage(ProjectileDataAsset->Damage, ProjectileOwner);
			}

			FOnBulletCollideInfo BulletCollideInfo;
			BulletCollideInfo.HitLocation = Hit.Location;
			BulletCollideInfo.PhysicalMaterial = Hit.PhysMaterial.Get();
			OnBulletCollide.Broadcast(BulletCollideInfo);
			
			AttackInfo.Damageable.Add(AttackInfo.bAttackHit);
		}

		if(!IsActorBeingDestroyed())
		{
			Destroy();
		}
	}

	Timer += DeltaTime;
}
