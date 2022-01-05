#include "PS1FlyingAI.h"

#include "AIController.h"
#include "PS1/AI/Movement/PawnMovementController.h"
#include "PS1/Components/WeaponComponent/PS1WeaponComponent.h"

APS1FlyingAI::APS1FlyingAI()
{
	PrimaryActorTick.bCanEverTick = true;
}

void APS1FlyingAI::BeginPlay()
{
	Super::BeginPlay();

	PlayerPawn = GetWorld()->GetFirstPlayerController()->GetPawn();
	
	MoveThresholdSq = FMath::Square(MoveThreshold);
	
	OnIdleStart.AddDynamic(this, &APS1FlyingAI::OnIdleBegin);
	OnLookingStart.AddDynamic(this, &APS1FlyingAI::OnLookingBegin);
	OnCombatStart.AddDynamic(this, &APS1FlyingAI::OnCombatBegin);
}

void APS1FlyingAI::OnIdleBegin()
{
	LastRot = GetActorRotation().Quaternion();
	
	ResetTimers();
	
	SetMoveTargetAndDirection(StartingLoc + FVector(FMath::FRandRange(0, IdleMovementAmount),
	FMath::FRandRange(0, IdleMovementAmount),0));
}

void APS1FlyingAI::OnLookingBegin()
{
	LastRot = GetActorRotation().Quaternion();
	
	ResetTimers();
}

void APS1FlyingAI::OnCombatBegin()
{
	LastRot = GetActorRotation().Quaternion();
	
	ResetTimers();
	
	SetMoveTargetAndDirection(PlayerPawn->GetActorLocation());
}

void APS1FlyingAI::IdleState()
{
	if(bIdleMovement)
	{
		MoveTowardsTarget();
		
		if(IdleTimer >= IdleMovementFrequency)
		{
			ResetTimers();
			
			SetMoveTargetAndDirection(StartingLoc + FVector(FMath::FRandRange(0, IdleMovementAmount),
			FMath::FRandRange(0, IdleMovementAmount),0));
		}
		
		IdleTimer += GetWorld()->GetDeltaSeconds();
	}
}

void APS1FlyingAI::LookingState()
{
	LookTimer += GetWorld()->GetDeltaSeconds();
	
	Accumulator += GetWorld()->GetDeltaSeconds() * TurnRate;
	Accumulator = Accumulator > 1.f ? 1.f : Accumulator;

	SetActorRotation(FQuat::Slerp(LastRot, LookAtRotation, Accumulator).Rotator());

	if(Accumulator >= 1.0f)
	{
		LookTimer += GetWorld()->GetDeltaSeconds();

		if(LookTimer >= LookTime)
		{
			ResetTimers();
			EnemyState = EEnemyState::Idle;
			OnIdleStart.Broadcast();
		}
	}
}

void APS1FlyingAI::CombatState()
{	
	if(!PlayerPawn)
	{
		return;
	}

	SetMoveTargetAndDirection(PlayerPawn->GetActorLocation());
	
	bPlayerInsightAndInDistance = false;
	
	FHitResult HitResult = FHitResult();
	FVector const StartTrace = GetActorLocation();
	FVector const EndTrace = PlayerPawn->GetActorLocation();

	if(!GetWorld()->LineTraceSingleByChannel(HitResult, StartTrace, EndTrace, ECC_Visibility))
	{
		SetLookAt(PlayerPawn->GetActorLocation());
		OutOfSightTimer = 0;
		float const DistSq = FVector::DistSquared(PlayerPawn->GetActorLocation(), GetActorLocation());
	
		if(DistSq < AttackDistanceSqrt)
		{
			bPlayerInsightAndInDistance = true;

			RotateTowardsTarget();
			
			WeaponComponent->Attack();

			return;
		}

		MoveTowardsTarget();
	}
	else
	{
		SetMoveTargetAndDirection(PlayerPawn->GetActorLocation());
		MoveTowardsTarget();

		OutOfSightTimer += GetWorld()->GetDeltaSeconds();

		if(OutOfSightTimer >= OutOfSightTime)
		{
			ResetTimers();

			SetLookAt(PlayerPawn->GetActorLocation());
			EnemyState = EEnemyState::Looking;
		}
	}
}

void APS1FlyingAI::SetMoveTargetAndDirection(FVector Target)
{
	MoveTarget = Target;
	MoveTargetDirection = (Target - GetActorLocation()).Rotation().Quaternion();
}

void APS1FlyingAI::MoveTowardsTarget()
{
	RotateTowardsTarget();

	if(FVector::DistSquared(GetActorLocation(), MoveTarget) < MoveThresholdSq)
	{
		return;
	}
	
	FVector Move = (MoveTarget - GetActorLocation()).GetSafeNormal() * NavMovementComponent->MovementSpeed * GetWorld()->GetDeltaSeconds();
	FHitResult Hit;
	AddActorWorldOffset(Move, true, &Hit);

	if(Hit.bBlockingHit)
	{
		AddActorWorldOffset(FVector::UpVector * NavMovementComponent->MovementSpeed * GetWorld()->GetDeltaSeconds(), true, &Hit);
	}
}

void APS1FlyingAI::RotateTowardsTarget()
{
	Accumulator += GetWorld()->GetDeltaSeconds() * TurnRate;
	Accumulator = Accumulator > 1.f ? 1.f : Accumulator;
	
	SetActorRotation(FQuat::Slerp(LastRot, MoveTargetDirection, Accumulator).Rotator());
}
