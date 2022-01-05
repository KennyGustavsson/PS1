#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PS1HealthComponent.generated.h"

USTRUCT(BlueprintType)
struct FOnDamageInfo
{
	GENERATED_BODY()

public:
	int CurrentHealth;

	UPROPERTY(Transient)
	AActor* DamageDealer;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDamageEvent, FOnDamageInfo, DamageInfo);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnHealEvent, int, CurrentHealth);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnTakeDamage);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDeathEvent);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PS1_API UPS1HealthComponent : public UActorComponent
{
	GENERATED_BODY()

	public:
	UPS1HealthComponent();
	virtual void BeginPlay() override;
	
	UFUNCTION(BlueprintCallable)
	void Damage(int const Damage, AActor* DamageDealer);

	UFUNCTION(BlueprintCallable)
	void Heal(int const Heal);

	UFUNCTION(BlueprintPure)
	int GetCurrentHealth() const;

	UFUNCTION(BlueprintPure)
	int GetMaxHealth() const;

	UFUNCTION(BlueprintPure)
	bool GetIsDead() const;
	
	UPROPERTY(EditDefaultsOnly)
	int MaxHealth;

	UPROPERTY(BlueprintAssignable)
	FOnDamageEvent OnDamageEvent;

	UPROPERTY(BlueprintAssignable)
	FOnHealEvent OnHealEvent;

	UPROPERTY(BlueprintAssignable)
	FOnDeathEvent OnDeathEvent;

	UPROPERTY()
	FOnTakeDamage OnTakeDamage;
	
	private:
	bool bIsDead;
	int CurrentHealth;
};