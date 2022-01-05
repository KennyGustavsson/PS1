#pragma once

#include "Engine/DataAsset.h"
#include "PS1ProjectileDataAsset.generated.h"

UCLASS()
class PS1_API UPS1ProjectileDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
	int Damage = 10;

	UPROPERTY(EditAnywhere)
	float Speed = 1000.0f;

	UPROPERTY(EditAnywhere)
	float LifeTime = 10.0f;

	UPROPERTY(EditAnywhere)
	UCurveFloat* SpeedMultiplierCurve;
};
