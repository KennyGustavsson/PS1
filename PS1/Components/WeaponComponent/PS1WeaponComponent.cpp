#include "PS1WeaponComponent.h"
#include "PS1/Weapons/PS1WeaponBase.h"

UPS1WeaponComponent::UPS1WeaponComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UPS1WeaponComponent::BeginPlay()
{
	Super::BeginPlay();

	WeaponIndex = 0;

	for(int i = 0; i < E_Count; i++ )
	{
		WeaponAmmo.Add(0);
		MaxAmmoArray.Add(0);

		if(MaxAmmo.Contains((EAmmoType)i))
		{
			MaxAmmoArray[i] = MaxAmmo[(EAmmoType)i];
		}
	}
}

void UPS1WeaponComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if(bOnCooldown)
	{
		Cooldown -= DeltaTime;

		if(Cooldown <= 0)
		{
			bOnCooldown = false;
			Cooldown = 0;
		}
	}
}

void UPS1WeaponComponent::ChangeActiveWeaponIndex(int Index)
{
	if(Index < 0 || Index >= Weapons.Num())
	{
		return;
	}

	WeaponList[WeaponIndex]->SetActorHiddenInGame(true);
	WeaponIndex = Index;
	WeaponList[WeaponIndex]->SetActorHiddenInGame(false);

	BroadcastOnAmmoChange();
}

int UPS1WeaponComponent::GetCurrentWeaponIndex() const
{
	return WeaponIndex;
}

FROnAttackInfo UPS1WeaponComponent::Attack()
{
	if(WeaponIndex >= WeaponList.Num())
	{
		return FROnAttackInfo();
	}

	if(bOnCooldown)
	{
		return FROnAttackInfo();
	}
	
	if(WeaponList[WeaponIndex])
	{
		if(WeaponList[WeaponIndex]->CurrentAmmoInClip == 0)
		{
			return FROnAttackInfo();
		}
		
		bOnCooldown = true;
		Cooldown = WeaponList[WeaponIndex]->AttackSpeed;
		WeaponList[WeaponIndex]->CurrentAmmoInClip -= 1;
		
		BroadcastOnAmmoChange();
		
		return WeaponList[WeaponIndex]->Attack(GetOwner(), this);
	}

	return FROnAttackInfo();
}

void UPS1WeaponComponent::SpawnWeaponsAttachedToComponent(USceneComponent* Component, FVector Offset)
{
	for(auto& weaponClass : Weapons)
	{
		auto* weapon = GetWorld()->SpawnActor<APS1WeaponBase>(weaponClass);

		weapon->AttachToComponent(Component, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
		weapon->AddActorLocalOffset(Offset);

		weapon->SetActorHiddenInGame(true);
		
		WeaponList.Add(weapon);
	}

	WeaponList[WeaponIndex]->SetActorHiddenInGame(false);
}

void UPS1WeaponComponent::DestroyWeapons()
{
	for(auto& Weapon : WeaponList)
	{
		Weapon->Destroy();
	}
	
	WeaponList.Empty();
	Weapons.Empty();
}

bool UPS1WeaponComponent::IsCurrentWeaponMelee()
{
	return WeaponList[WeaponIndex]->bIsMeleeWeapon;
}

int UPS1WeaponComponent::AddWeapon(TSubclassOf<APS1WeaponBase> Weapon, USceneComponent* AttachComponent, FVector Offset)
{
	// If weapon already exist in the weapon component
	if(Weapons.Contains(Weapon))
	{
		int const Index = Weapons.Find(Weapon);
		if(Index >= 0)
		{
			// If Melee weapon return
			if(WeaponList[Index]->bIsMeleeWeapon)
			{
				return -1;
			}

			// Add one clip of that ammo
			AddAmmo(WeaponList[Index]->WeaponAmmoType, WeaponList[Index]->AmmoClipSize);

			// Ammo change broadcast
			BroadcastOnAmmoChange();
		}
		
		return -1;
	}

	// Add weapon
	Weapons.Add(Weapon);

	// Spawn weapon actor
	auto* weapon = GetWorld()->SpawnActor<APS1WeaponBase>(Weapon);
	weapon->AttachToComponent(AttachComponent, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
	weapon->AddActorLocalOffset(Offset);
	weapon->SetActorHiddenInGame(true);

	// Add actor to list
	WeaponList.Add(weapon);

	// Returns the weapons index
	return WeaponList.Num() - 1;
}

void UPS1WeaponComponent::AddAmmo(EAmmoType AmmoType, int Amount)
{
	if(WeaponAmmo[AmmoType] + Amount > MaxAmmoArray[AmmoType])
	{
		WeaponAmmo[AmmoType] = MaxAmmoArray[AmmoType];
		return;
	}
	
	WeaponAmmo[AmmoType] += Amount;
}

int UPS1WeaponComponent::GetCurrentAmmo()
{
	return WeaponAmmo[(int)WeaponList[WeaponIndex]->WeaponAmmoType];
}

int UPS1WeaponComponent::GetCurrentMaxAmmo()
{
	return MaxAmmoArray[(int)WeaponList[WeaponIndex]->WeaponAmmoType];
}

void UPS1WeaponComponent::Reload()
{
	if(!WeaponList[WeaponIndex])
	{
		return;
	}
	
	int const AmmoReduction = WeaponList[WeaponIndex]->Reload(GetCurrentAmmo());
	AddAmmo(WeaponList[WeaponIndex]->WeaponAmmoType, -AmmoReduction);
	
	BroadcastOnAmmoChange();
}

bool UPS1WeaponComponent::IsWeaponIndexValid(int Index)
{
	if(Index < 0 || Index >= Weapons.Num())
	{
		return false;
	}

	return true;
}

bool UPS1WeaponComponent::CanReloadCurrentWeapon()
{
	if(WeaponList[WeaponIndex]->bIsMeleeWeapon ||
		WeaponAmmo[WeaponList[WeaponIndex]->WeaponAmmoType] <= 0 ||
		WeaponList[WeaponIndex]->CurrentAmmoInClip >= WeaponList[WeaponIndex]->AmmoClipSize ||
		WeaponList[WeaponIndex]->AmmoClipSize <= 0)
	{
		return false;
	}

	return true;
}

void UPS1WeaponComponent::BroadcastOnAmmoChange()
{
	FOnAmmoChangeInfo AmmoInfo;
	AmmoInfo.CurrentClipAmmo = WeaponList[WeaponIndex]->CurrentAmmoInClip;
	AmmoInfo.CurrentSpareAmmo = GetCurrentAmmo();
	AmmoInfo.CurrentClipMax = WeaponList[WeaponIndex]->AmmoClipSize;
	AmmoInfo.CurrentSpareMax = GetCurrentMaxAmmo();
	
	OnAmmoChange.Broadcast(AmmoInfo);
}

FLoadXBulletInMagazineInfo UPS1WeaponComponent::ReloadXBullets(int BulletAmount)
{
	if(!WeaponList[WeaponIndex])
	{
		return FLoadXBulletInMagazineInfo();
	}
	
	int const AmmoReduction = WeaponList[WeaponIndex]->ReloadXBullets(GetCurrentAmmo(), BulletAmount);
	AddAmmo(WeaponList[WeaponIndex]->WeaponAmmoType, -AmmoReduction);

	BroadcastOnAmmoChange();

	FLoadXBulletInMagazineInfo LoadBulletInfo;
	LoadBulletInfo.BulletsInserted = AmmoReduction;
	LoadBulletInfo.bMagazineIsFull = false;
	
	if(WeaponList[WeaponIndex]->CurrentAmmoInClip == WeaponList[WeaponIndex]->AmmoClipSize)
	{
		LoadBulletInfo.bMagazineIsFull = true;
	}
	
	return LoadBulletInfo;
}

AActor* UPS1WeaponComponent::GetCurrentWeaponAsActor()
{
	return Cast<AActor>(WeaponList[WeaponIndex]);
}

APS1WeaponBase* UPS1WeaponComponent::GetCurrentWeapon()
{
	return WeaponList[WeaponIndex];
}

TSubclassOf<APS1WeaponBase> UPS1WeaponComponent::GetWeaponAsSubClassOfBase()
{
	return Weapons[WeaponIndex];
}

TArray<APS1WeaponBase*> UPS1WeaponComponent::GetWeaponList() const
{
	return WeaponList;
}


