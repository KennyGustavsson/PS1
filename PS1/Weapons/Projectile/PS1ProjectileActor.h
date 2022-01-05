#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PS1ProjectileActor.generated.h"

USTRUCT(BlueprintType)
struct FOnBulletCollideInfo
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	UPhysicalMaterial* PhysicalMaterial = nullptr;

	UPROPERTY(BlueprintReadOnly)
	FVector HitLocation;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnBulletCollide, FOnBulletCollideInfo, BulletCollideInfo);

UCLASS(BlueprintType, Blueprintable)
class PS1_API APS1ProjectileActor : public AActor
{
	GENERATED_BODY()

public:
	APS1ProjectileActor();
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditDefaultsOnly)
	class UCapsuleComponent* CapsuleComponent;
	
	UPROPERTY(EditDefaultsOnly)
	class UStaticMeshComponent* StaticMeshComponent;

	UPROPERTY(EditDefaultsOnly)
	class UPS1ProjectileDataAsset* ProjectileDataAsset;

	UPROPERTY(Transient)
	AActor* ProjectileOwner = nullptr;

	UPROPERTY(Transient)
	class UPS1WeaponComponent* OwnerWeaponComponent = nullptr;

	UPROPERTY(Transient)
	float Timer = 0;

	UPROPERTY(BlueprintAssignable)
	FOnBulletCollide OnBulletCollide;
};
