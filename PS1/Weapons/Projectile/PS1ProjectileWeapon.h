#pragma once

#include "PS1/Weapons/PS1WeaponBase.h"
#include "PS1ProjectileWeapon.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnAttackEvent);

class APS1ProjectileActor;

UCLASS(BlueprintType, Blueprintable)
class PS1_API APS1ProjectileWeapon : public APS1WeaponBase
{
	GENERATED_BODY()

public:
	APS1ProjectileWeapon();
	virtual void BeginPlay() override;
	virtual FROnAttackInfo Attack(AActor* ProjectileOwner, UPS1WeaponComponent* WeaponComponent) override;
	virtual int Reload(int AmmoAmount) override;
	virtual int ReloadXBullets(int AmmoAmount, int AmmoToReload) override;
	
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<APS1ProjectileActor> ProjectileActor;

	UPROPERTY(EditDefaultsOnly)
	class USceneComponent* Root;
	
	UPROPERTY(EditDefaultsOnly)
	class UStaticMeshComponent* MeshComponent;

	UPROPERTY(EditDefaultsOnly)
	class UArrowComponent* ProjectileSpawnPoint;

	UPROPERTY(EditDefaultsOnly)
	int ProjectileAmount = 1;
	
	UPROPERTY(EditDefaultsOnly)
	float BulletStartOffset = 5.0f;

	UPROPERTY(EditDefaultsOnly)
	float TimeBetweenShots = 1.0f;

	UPROPERTY(EditDefaultsOnly)
	float WeaponKick = 1.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float InaccuracyMultiplier = 1.0f;

	UPROPERTY(EditDefaultsOnly)
	float RandomYawInaccuracy = 0.0f;

	UPROPERTY(EditDefaultsOnly)
	float RandomPitchInaccuracy = 0.0f;
	
	UPROPERTY(BlueprintAssignable)
    FOnAttackFiredEvent OnAttackFiredEvent;

	UPROPERTY(EditDefaultsOnly)
	TEnumAsByte<EAmmoType> AmmoType;

	UPROPERTY(EditDefaultsOnly)
	int ClipSize = 30;
	
	UFUNCTION(BlueprintPure)
	EAmmoType GetAmmoType() const;

	UFUNCTION(BlueprintPure)
	int GetClipSize() const;
};
