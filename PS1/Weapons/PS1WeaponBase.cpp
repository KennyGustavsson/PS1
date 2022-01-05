#include "PS1WeaponBase.h"

FROnAttackInfo APS1WeaponBase::Attack(AActor* ProjectileOwner, UPS1WeaponComponent* WeaponComponent)
{
	return FROnAttackInfo();
}

int APS1WeaponBase::Reload(int AmmoAmount)
{
	return 0;
}

int APS1WeaponBase::ReloadXBullets(int AmmoAmount, int AmmoToReload)
{
	return 0;
}

int APS1WeaponBase::ReloadCalc(int const AmmoAmount, int const ClipAmmoSize, int& CurrentClipAmmo)
{
	int const AmmoDiff = ClipAmmoSize - CurrentClipAmmo;
	
	if(AmmoDiff < AmmoAmount)
	{
		CurrentAmmoInClip = ClipAmmoSize;
		return AmmoDiff;
	}

	CurrentClipAmmo += AmmoAmount;
	return AmmoAmount;
}
