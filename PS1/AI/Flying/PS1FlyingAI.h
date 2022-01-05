#pragma once

#include "CoreMinimal.h"
#include "PS1/AI/PS1AI.h"
#include "PS1FlyingAI.generated.h"

UCLASS()
class PS1_API APS1FlyingAI : public APS1AI
{
	GENERATED_BODY()

public:
	APS1FlyingAI();
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnIdleBegin();

	UFUNCTION()
	void OnLookingBegin();

	UFUNCTION()
	void OnCombatBegin();
	
	virtual void IdleState() override;
	virtual void LookingState() override;
	virtual void CombatState() override;

	void SetMoveTargetAndDirection(FVector Target);
	
	void MoveTowardsTarget();
	void RotateTowardsTarget();

	UPROPERTY(EditDefaultsOnly)
	float MoveThreshold = 20.0f;

	float MoveThresholdSq;
	
	FVector MoveTarget = FVector::ZeroVector;
	FQuat MoveTargetDirection = FQuat::Identity;
};
