#pragma once

#include "Engine/DataAsset.h"
#include "PS1MeleeDataAsset.generated.h"

UCLASS()
class PS1_API UPS1MeleeDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
	int Damage = 10;
};
