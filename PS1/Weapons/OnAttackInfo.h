// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "OnAttackInfo.generated.h"

USTRUCT(BlueprintType)
struct FROnAttackInfo
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	bool bMelee = false;
	
	UPROPERTY(BlueprintReadOnly)
	float WeaponKick = 0.0f;
	
	UPROPERTY(BlueprintReadOnly)
	TArray<UPhysicalMaterial*> Surfaces;

	UPROPERTY(BlueprintReadOnly)
	TArray<FVector> HitLocations;
};

UCLASS()
class PS1_API UOnAttackInfo : public UObject
{
	GENERATED_BODY()
	UOnAttackInfo(){}

	FROnAttackInfo Hej()
	{
		return FROnAttackInfo();
	}
};