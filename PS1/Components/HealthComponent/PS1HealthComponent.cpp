#include "PS1HealthComponent.h"

UPS1HealthComponent::UPS1HealthComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UPS1HealthComponent::BeginPlay()
{
	Super::BeginPlay();

	CurrentHealth = MaxHealth;
}

void UPS1HealthComponent::Damage(int const Damage, AActor* DamageDealer)
{
	if(Damage <= 0 || bIsDead)
	{
		return;
	}

	FOnDamageInfo DamageInfo;
	DamageInfo.DamageDealer = DamageDealer;
	
	// If going to die
	if(CurrentHealth - Damage <= 0)
	{
		CurrentHealth = 0;
		OnDeathEvent.Broadcast();

		bIsDead = true;
		
		return;
	}

	CurrentHealth -= Damage;
	DamageInfo.CurrentHealth = CurrentHealth;
	OnDamageEvent.Broadcast(DamageInfo);
	OnTakeDamage.Broadcast();
}

void UPS1HealthComponent::Heal(int const Heal)
{
	if(Heal <= 0 || bIsDead || CurrentHealth == MaxHealth)
	{
		return;
	}
	
	// If reaches max hp
	if(CurrentHealth + Heal >= MaxHealth)
	{
		CurrentHealth = MaxHealth;
		OnHealEvent.Broadcast(CurrentHealth);

		return;
	}

	CurrentHealth += Heal;
	OnHealEvent.Broadcast(CurrentHealth);
}

int UPS1HealthComponent::GetCurrentHealth() const
{
	return CurrentHealth;
}

int UPS1HealthComponent::GetMaxHealth() const
{
	return MaxHealth;
}

bool UPS1HealthComponent::GetIsDead() const
{
	return bIsDead;
}
