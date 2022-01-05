#include "PS1ProjectileWeapon.h"
#include "PS1ProjectileActor.h"
#include "Components/ArrowComponent.h"

APS1ProjectileWeapon::APS1ProjectileWeapon()
{
	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = Root;
	
	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	MeshComponent->SetupAttachment(Root);
	
	ProjectileSpawnPoint = CreateDefaultSubobject<UArrowComponent>(TEXT("ProjectileSpawnPoint"));
	ProjectileSpawnPoint->SetupAttachment(Root);
}

void APS1ProjectileWeapon::BeginPlay()
{
	Super::BeginPlay();
	AttackSpeed = TimeBetweenShots;
	WeaponAmmoType = AmmoType;
	
	AmmoClipSize = ClipSize;
	CurrentAmmoInClip = ClipSize;
}

FROnAttackInfo APS1ProjectileWeapon::Attack(AActor* ProjectileOwner, UPS1WeaponComponent* WeaponComponent)
{
	if(!ProjectileActor)
	{
		UE_LOG(LogTemp, Log, TEXT("Projectile Weapon is missing a Projectile Actor"));
		return FROnAttackInfo();
	}

	for (int i = 0; i < ProjectileAmount; i++)
	{
		FVector const Location(ProjectileSpawnPoint->GetComponentLocation());
		FRotator Rotation(ProjectileSpawnPoint->GetComponentRotation());

		Rotation.Yaw += FMath::RandRange(-RandomYawInaccuracy, RandomYawInaccuracy) * InaccuracyMultiplier;
		Rotation.Pitch += FMath::RandRange(-RandomPitchInaccuracy, RandomPitchInaccuracy) * InaccuracyMultiplier;
	
		FActorSpawnParameters const SpawnInfo;
		APS1ProjectileActor* const Projectile = GetWorld()->SpawnActor<APS1ProjectileActor>(ProjectileActor->GetDefaultObject()->GetClass(), Location, Rotation, SpawnInfo);
		Projectile->ProjectileOwner = ProjectileOwner;
		Projectile->OwnerWeaponComponent = WeaponComponent;	
	}

	OnAttackFiredEvent.Broadcast();

	FROnAttackInfo OnAttackInfo;
	OnAttackInfo.bMelee = false;
	OnAttackInfo.WeaponKick = WeaponKick;
	
	return OnAttackInfo;
}

int APS1ProjectileWeapon::Reload(int AmmoAmount)
{
	return ReloadCalc(AmmoAmount, AmmoClipSize, CurrentAmmoInClip);
}

int APS1ProjectileWeapon::ReloadXBullets(int AmmoAmount, int AmmoToReload)
{
	if(AmmoToReload < AmmoAmount)
	{
		CurrentAmmoInClip += AmmoToReload;
		return AmmoToReload;
	}

	CurrentAmmoInClip += AmmoAmount;
	return AmmoAmount;
}

EAmmoType APS1ProjectileWeapon::GetAmmoType() const
{
	return AmmoType;
}

int APS1ProjectileWeapon::GetClipSize() const
{
	return ClipSize;
}

