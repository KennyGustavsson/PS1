#pragma once

#include "PS1/Weapons/PS1WeaponBase.h"
#include "PS1MeleeWeapon.generated.h"

UCLASS(BlueprintType, BlueprintType)
class PS1_API APS1MeleeWeapon : public APS1WeaponBase
{
	GENERATED_BODY()

public:
	APS1MeleeWeapon();
	virtual void BeginPlay() override;
	virtual FROnAttackInfo Attack(AActor* ProjectileOwner, UPS1WeaponComponent* WeaponComponent) override;

	UPROPERTY(EditDefaultsOnly)
	class USceneComponent* Root;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	class UStaticMeshComponent* MeshComponent;
	
	UPROPERTY(EditDefaultsOnly)
	class USphereComponent* HitSphere;

	UPROPERTY(EditDefaultsOnly)
	class UPS1MeleeDataAsset* MeleeDataAsset;

	UPROPERTY(EditDefaultsOnly)
	float WeaponKick = 0;

	UPROPERTY(EditDefaultsOnly)
	float AttackCooldown = 0;

	UPROPERTY(BlueprintAssignable)
	FOnAttackFiredEvent OnAttackFiredEvent;

	UPROPERTY(EditDefaultsOnly)
	bool bDrawDebug = false;

private:
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectType;
};
