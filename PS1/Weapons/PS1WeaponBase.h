#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PS1/Weapons/Ammo/AmmoType.h"
#include "OnAttackInfo.h"
#include "PS1WeaponBase.generated.h"

class UPS1WeaponComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnAttackFiredEvent);

UCLASS()
class PS1_API APS1WeaponBase : public AActor
{
	GENERATED_BODY()

public:
	virtual FROnAttackInfo Attack(AActor* ProjectileOwner, UPS1WeaponComponent* WeaponComponent);
	virtual int Reload(int AmmoAmount);
	virtual int ReloadXBullets(int AmmoAmount, int AmmoToReload);

	UFUNCTION()
	int ReloadCalc(int const AmmoAmount, int const ClipAmmoSize, int& CurrentClipAmmo);
	
	UPROPERTY(Transient)
	float AttackSpeed = 1.0f;

	UPROPERTY(Transient)
	bool bIsMeleeWeapon = false;

	UPROPERTY(Transient)
	int CurrentAmmoInClip = -1;

	UPROPERTY(Transient)
	int AmmoClipSize = -1;

	UPROPERTY(Transient)
	TEnumAsByte<EAmmoType> WeaponAmmoType;
};
