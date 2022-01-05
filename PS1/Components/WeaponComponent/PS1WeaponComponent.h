#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PS1/Weapons/Ammo/AmmoType.h"
#include "PS1/Weapons/OnAttackInfo.h"
#include "PS1WeaponComponent.generated.h"

USTRUCT(BlueprintType)
struct FAttackInfo
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly)
	bool bAttackHit = false;

	UPROPERTY(BlueprintReadOnly)
	bool bProjectile = false;

	UPROPERTY(BlueprintReadOnly)
	TArray<FVector> HitLocations;

	UPROPERTY(BlueprintReadOnly)
	TArray<FVector> HitNormals;

	UPROPERTY(BlueprintReadOnly)
	TArray<AActor*> ActorsHit;

	UPROPERTY(BlueprintReadOnly)
	TArray<bool> Damageable;
};

USTRUCT(BlueprintType)
struct FOnAmmoChangeInfo
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly)
	int CurrentClipAmmo = 0;

	UPROPERTY(BlueprintReadOnly)
	int CurrentSpareAmmo = 0;

	UPROPERTY(BlueprintReadOnly)
	int CurrentSpareMax = 0;

	UPROPERTY(BlueprintReadOnly)
	int CurrentClipMax = 0;
};

USTRUCT(BlueprintType)
struct FLoadXBulletInMagazineInfo
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	int BulletsInserted = 0;

	UPROPERTY(BlueprintReadOnly)
	bool bMagazineIsFull = false;
};

class APS1WeaponBase;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAttack, FAttackInfo, AttackInfo);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAmmoChange, FOnAmmoChangeInfo, AmmoChangeInfo);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PS1_API UPS1WeaponComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UPS1WeaponComponent();
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
	UPROPERTY(EditDefaultsOnly)
	TArray<TSubclassOf<APS1WeaponBase>> Weapons;

	UPROPERTY(Transient)
	TArray<APS1WeaponBase*> WeaponList;
	
	UFUNCTION(BlueprintCallable)
	void ChangeActiveWeaponIndex(int Index);

	UFUNCTION(BlueprintPure)
	int GetCurrentWeaponIndex() const;

	UFUNCTION(BlueprintCallable)
	FROnAttackInfo Attack();

	UFUNCTION(BlueprintCallable)
	void SpawnWeaponsAttachedToComponent(USceneComponent* Component, FVector Offset);

	UFUNCTION(BlueprintCallable)
	void DestroyWeapons();
	
	UPROPERTY(EditDefaultsOnly)
	FName SocketAttachName;

	UPROPERTY(EditDefaultsOnly)
	FVector AttachOffset = FVector::ZeroVector;

	UFUNCTION(BlueprintPure)
	bool IsCurrentWeaponMelee();

	UPROPERTY(BlueprintAssignable)
	FOnAttack OnAttack;

	UPROPERTY(BlueprintAssignable)
	FOnAmmoChange OnAmmoChange;
	
	UFUNCTION(BlueprintCallable)
	int AddWeapon(TSubclassOf<APS1WeaponBase> Weapon, USceneComponent* AttachComponent, FVector Offset);

	UFUNCTION(BlueprintCallable)
	void AddAmmo(EAmmoType AmmoType, int Amount);

	UFUNCTION(BlueprintCallable)
	int GetCurrentAmmo();
	
	UFUNCTION(BlueprintCallable)
	int GetCurrentMaxAmmo();

	UFUNCTION(BlueprintCallable)
	void Reload();
	
	UPROPERTY(EditDefaultsOnly)
	TMap<TEnumAsByte<EAmmoType>, int> MaxAmmo;
	
	UFUNCTION(BlueprintCallable)
	bool IsWeaponIndexValid(int Index);

	UFUNCTION(BlueprintPure)
	bool CanReloadCurrentWeapon();

	UFUNCTION(BlueprintCallable)
	void BroadcastOnAmmoChange();

	UFUNCTION(BlueprintCallable)
	FLoadXBulletInMagazineInfo ReloadXBullets(int BulletAmount);

	UFUNCTION(BlueprintPure)
	AActor* GetCurrentWeaponAsActor();

	UFUNCTION(BlueprintPure)
	APS1WeaponBase* GetCurrentWeapon();

	UFUNCTION(BlueprintPure)
	TSubclassOf<APS1WeaponBase> GetWeaponAsSubClassOfBase();

	UFUNCTION(BlueprintPure)
	TArray<APS1WeaponBase*> GetWeaponList() const;
	
private:
	int WeaponIndex;

	bool bOnCooldown = false;
	float Cooldown = 0;

	TArray<int> WeaponAmmo;
	TArray<int> MaxAmmoArray;
};
