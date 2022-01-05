#include "DrawDebugHelpers.h"
#include "PS1MeleeWeapon.h"
#include "Components/SphereComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "PS1/Components/HealthComponent/PS1HealthComponent.h"
#include "PS1/Components/WeaponComponent/PS1WeaponComponent.h"
#include "PS1/Weapons/DataAssets/PS1MeleeDataAsset.h"

APS1MeleeWeapon::APS1MeleeWeapon()
{
	PrimaryActorTick.bCanEverTick = false;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = Root;
	
	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	MeshComponent->SetupAttachment(Root);
	
	HitSphere = CreateDefaultSubobject<USphereComponent>(TEXT("HitBox"));
	HitSphere->SetupAttachment(Root);
	HitSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void APS1MeleeWeapon::BeginPlay()
{
	Super::BeginPlay();

	bIsMeleeWeapon = true;
	AttackSpeed = AttackCooldown;
}

FROnAttackInfo APS1MeleeWeapon::Attack(AActor* ProjectileOwner, UPS1WeaponComponent* WeaponComponent)
{
	if(!MeleeDataAsset)
	{
		UE_LOG(LogTemp, Log, TEXT("Melee weapon missing MeleeDataAsset"));
		return FROnAttackInfo();
	}
	
	TArray<AActor*> IgnoreActors;
	IgnoreActors.Add(ProjectileOwner);
	IgnoreActors.Add(this);
	TArray<AActor*> OutActors;

	FROnAttackInfo OnAttackInfo;
	OnAttackInfo.bMelee = true;
	OnAttackInfo.WeaponKick = WeaponKick;

	UKismetSystemLibrary::SphereOverlapActors(this, HitSphere->GetComponentLocation(), HitSphere->GetScaledSphereRadius(), ObjectType, nullptr, IgnoreActors, OutActors);

	if(bDrawDebug)
	{
		DrawDebugSphere(GetWorld(), HitSphere->GetComponentLocation(), HitSphere->GetScaledSphereRadius(), 32, FColor::Red, false, 2.0f, 0, 2.0f);	
	}
	
	FAttackInfo AttackInfo;
	AttackInfo.bProjectile = false;

	bool DidHit = false;
	
	for (AActor* Actor : OutActors)
	{
		TArray<UPS1HealthComponent*> Components;
		Actor->GetComponents(Components);

		TArray<USkeletalMeshComponent*> SkeletalMeshComponents;
		Actor->GetComponents(SkeletalMeshComponents);

		AttackInfo.ActorsHit.Add(Actor);
		bool HasHealthComponent = false;

		OnAttackInfo.HitLocations.Add(Actor->GetActorLocation());
		UPhysicalMaterial* PhyMatSurface = nullptr;
		
		for(UPS1HealthComponent* HealthComp : Components)
		{
		    if(HealthComp)
		    {
			    HealthComp->Damage(MeleeDataAsset->Damage, ProjectileOwner);
		    	HasHealthComponent = true;

		    	if(!DidHit)
		    	{
		    		DidHit = true;
		    	}
			}
		}
		
		if(SkeletalMeshComponents.Num() > 0)
		{
			if(SkeletalMeshComponents[0]->GetMaterial(0))
			{			
				if(SkeletalMeshComponents[0]->GetMaterial(0)->GetPhysicalMaterial())
				{
					PhyMatSurface = SkeletalMeshComponents[0]->GetMaterial(0)->GetPhysicalMaterial();
				}
			}
		}
		else
		{
			TArray<UStaticMeshComponent*> MeshComponents;
			Actor->GetComponents(MeshComponents);

			if(MeshComponents.Num() > 0)
			{
				if(MeshComponents[0]->GetMaterial(0))
				{
					if(MeshComponents[0]->GetMaterial(0)->GetPhysicalMaterial())
					{
						PhyMatSurface = MeshComponents[0]->GetMaterial(0)->GetPhysicalMaterial();
					}
				}
			}
		}
		
		OnAttackInfo.Surfaces.Add(PhyMatSurface);
		
		AttackInfo.Damageable.Add(HasHealthComponent);
		AttackInfo.HitLocations.Add(Actor->GetActorLocation());
		AttackInfo.HitNormals.Add((Actor->GetActorLocation() - HitSphere->GetComponentLocation()).GetSafeNormal());

		if(bDrawDebug)
		{
			UE_LOG(LogTemp, Log, TEXT("Hit actor"));
		}
	}

	AttackInfo.bAttackHit = DidHit;

	if(WeaponComponent != nullptr)
	{
		WeaponComponent->OnAttack.Broadcast(AttackInfo);
	}

	OnAttackFiredEvent.Broadcast();

	return OnAttackInfo;
}