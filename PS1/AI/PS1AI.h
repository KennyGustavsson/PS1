#pragma once

#include "CoreMinimal.h"
#include "PS1AI.generated.h"

UENUM(BlueprintType)
enum EEnemyState
{
	Idle,
	Looking,
	Combat
};

USTRUCT(BlueprintType)
struct FTAMoveData
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadOnly)
	FVector WantedDestination;
};

class APS1WeaponBase;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnIdleStateStart);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnLookingStateStart);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCombatStateStart);

UCLASS(Blueprintable, BlueprintType)
class PS1_API APS1AI : public APawn
{
	GENERATED_BODY()

public:
	APS1AI();
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(VisibleDefaultsOnly, Category=Collision, BlueprintReadWrite)
	class UCapsuleComponent* CapsuleComponent;

	UPROPERTY(VisibleDefaultsOnly, Category=Mesh, BlueprintReadWrite)
	USkeletalMeshComponent* MeshComponent;

	UPROPERTY(VisibleDefaultsOnly, Category=Movement, BlueprintReadWrite)
	class UPawnMovementController* NavMovementComponent;
	
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite)
	class UPS1HealthComponent* HealthComponent;

	UPROPERTY(EditInstanceOnly)
	TArray<AActor*> EnemySquad;

	UPROPERTY(Transient)
	TArray<APS1AI*> AIEnemySquad;

	UFUNCTION(BlueprintCallable)
	TArray<AActor*> GetSquad() const;

	UFUNCTION(BlueprintCallable)
	void SetEnemySquad(TArray<AActor*> const Squad);

	virtual float GetDefaultHalfHeight() const override;
	UCapsuleComponent* GetCapsule() const {return CapsuleComponent;}

	UPROPERTY(Transient)
	FVector LastPosition = FVector::ZeroVector;

	UPROPERTY(BlueprintReadOnly)
	float VelocityMagnitude = 0;

	UPROPERTY(EditAnywhere)
	AActor* PatrolSplineActor;

	UFUNCTION(BlueprintImplementableEvent)
	void OnWantedMovement(FTAMoveData MoveData);

	UPROPERTY(BlueprintReadOnly)
	float RotationDeltaMagnitude = 0.0f;

	UPROPERTY(Transient)
	FVector LastRotation = FVector::ZeroVector;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	class UPawnSensingComponent* PawnSensingComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	class UPS1WeaponComponent* WeaponComponent;

	UPROPERTY(BlueprintAssignable)
	FOnIdleStateStart OnIdleStart;

	UPROPERTY(BlueprintAssignable)
	FOnLookingStateStart OnLookingStart;

	UPROPERTY(BlueprintAssignable)
	FOnCombatStateStart OnCombatStart;

	UFUNCTION()
	void OnDamageTaken();
	
	UFUNCTION()
	void SeenPawn(APawn* SeenPawn);

	UFUNCTION()
	void HeardNoise(APawn* NoiseInstigator, const FVector& Location, float Volume);

	UFUNCTION()
	virtual void IdleState();

	UFUNCTION()
	virtual void LookingState();
	
	UFUNCTION()
	virtual void CombatState();

	UFUNCTION(BlueprintCallable)
	void SetEnemyState(EEnemyState const State);

	UFUNCTION(BlueprintCallable)
	void SetLookAtLocation(FVector Target);

	UFUNCTION(BlueprintPure)
	FRotator GetLookAtRotation() const;

	UFUNCTION(BlueprintPure)
	FRotator GetLookAtRotationYaw() const;

	UFUNCTION(BlueprintPure)
	EEnemyState GetEnemyState() const;

	UFUNCTION(BlueprintPure)
	bool IsPlayerInSightAndInDistance() const;

	UFUNCTION(BlueprintCallable)
	FRotator GetLookAtForComponent(USceneComponent* Component, FVector TargetPosition);

	UFUNCTION(BlueprintCallable)
	FRotator GetLookAtPlayerForComponent(USceneComponent* Component);

	UPROPERTY(Transient)
	APawn* PlayerPawn = nullptr;

	UFUNCTION()
	void ResetTimers();

	UFUNCTION()
	void SetLookAt(FVector TargetPosition);
	
	UPROPERTY(EditDefaultsOnly)
	bool bIdleMovement = true;

	UPROPERTY(EditDefaultsOnly)
	float IdleMovementFrequency = 5.0f;

	UPROPERTY(EditDefaultsOnly)
	float IdleMovementAmount = 100.0f;
	
	UPROPERTY(EditDefaultsOnly)
	float TurnRate = 30.0f;

	UPROPERTY(EditDefaultsOnly)
	float LookTime = 2.0f;

	UPROPERTY(EditDefaultsOnly)
	float AttackDistance = 50.0f;

	UPROPERTY(EditDefaultsOnly)
	float OutOfSightTime = 5.0f;

	UPROPERTY(EditDefaultsOnly)
	float ActivationDistance = 10000.0f;

	UPROPERTY(Transient)
	float AttackDistanceSqrt;

	UPROPERTY(Transient)
	float ActivationDistanceSqrt;
	
	EEnemyState EnemyState;

	UPROPERTY(Transient)
	FQuat LookAtRotationYaw = FQuat::Identity;

	UPROPERTY(Transient)
	FQuat LookAtRotation = FQuat::Identity;

	UPROPERTY(Transient)
	FQuat LastRot = FQuat::Identity;

	UPROPERTY(Transient)
	class AAIController* AIController;

	UPROPERTY(Transient)
	bool bPlayerInsightAndInDistance = false;

	float IdleTimer = 0;
	FVector StartingLoc = FVector::ZeroVector;
	
	float LookTimer = 0.0f;
	float Accumulator = 0.0f;
	float OutOfSightTimer = 0.0f;
};
