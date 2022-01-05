#include "PS1AI.h"
#include "AIController.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Movement/PawnMovementController.h"
#include "Perception/PawnSensingComponent.h"
#include "PS1/Components/HealthComponent/PS1HealthComponent.h"
#include "PS1/Components/WeaponComponent/PS1WeaponComponent.h"
#include "PS1/Weapons/PS1WeaponBase.h"

APS1AI::APS1AI()
{
	PrimaryActorTick.bCanEverTick = true;

	CapsuleComponent = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Capsule"));
	CapsuleComponent->SetCollisionObjectType(ECC_Pawn);
	RootComponent = CapsuleComponent;

	MeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Mesh"));
	MeshComponent->SetupAttachment(RootComponent);

	NavMovementComponent = CreateDefaultSubobject<UPawnMovementController>(TEXT("NavMovement"));
	PawnSensingComponent = CreateDefaultSubobject<UPawnSensingComponent>(TEXT("SensingComponent"));
	WeaponComponent = CreateDefaultSubobject<UPS1WeaponComponent>(TEXT("WeaponComponent"));
	HealthComponent = CreateDefaultSubobject<UPS1HealthComponent>(TEXT("HealthComponent"));

	EnemySquad.Add(this);
}

void APS1AI::BeginPlay()
{
	Super::BeginPlay();
	
	PawnSensingComponent->OnSeePawn.AddDynamic(this, &APS1AI::SeenPawn);
	PawnSensingComponent->OnHearNoise.AddDynamic(this, &APS1AI::HeardNoise);

	PlayerPawn = GetWorld()->GetFirstPlayerController()->GetPawn();

	AttackDistanceSqrt = FMath::Square(AttackDistance);
	ActivationDistanceSqrt = FMath::Square(ActivationDistance);
	
	LastPosition = GetActorLocation();
	StartingLoc = GetActorLocation();
	
	AIController = Cast<AAIController>(GetController());

	EnemyState = EEnemyState::Idle;
	OnIdleStart.Broadcast();

	HealthComponent->OnTakeDamage.AddDynamic(this, &APS1AI::OnDamageTaken);

	for(AActor* Enemy : EnemySquad)
	{
		APS1AI* AI = Cast<APS1AI>(Enemy);

		if(AI && AI != this)
		{
			AIEnemySquad.Add(AI);
		}
	}
}

void APS1AI::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if(HealthComponent->GetIsDead() || !PlayerPawn)
	{
		return;
	}

	float const DistSq = FVector::DistSquared(PlayerPawn->GetActorLocation(), GetActorLocation());

	if(DistSq < ActivationDistance)
	{
		return;
	}
	
	VelocityMagnitude = FVector::Distance(LastPosition, GetActorLocation());
	LastPosition = GetActorLocation();

	FRotator const CurrentRot = GetActorRotation();
	FVector const CurrentRotVec = FVector(CurrentRot.Pitch, CurrentRot.Yaw, CurrentRot.Roll);
	RotationDeltaMagnitude = FVector::Distance(LastRotation, CurrentRotVec);
	LastRotation = CurrentRotVec;
	
	switch (EnemyState)
	{
		case EEnemyState::Idle:
			IdleState();
			break;
		case EEnemyState::Looking:
			LookingState();
			break;
		case EEnemyState::Combat:
			CombatState();
			break;
		default:
			break;
	}
}

TArray<AActor*> APS1AI::GetSquad() const
{
	return EnemySquad;
}

void APS1AI::SetEnemySquad(TArray<AActor*> const Squad)
{
	EnemySquad = Squad;
}

float APS1AI::GetDefaultHalfHeight() const
{
	return CapsuleComponent->GetScaledCapsuleHalfHeight();
}

void APS1AI::OnDamageTaken()
{
	EnemyState = EEnemyState::Combat;
	OnCombatStart.Broadcast();
}

void APS1AI::SeenPawn(APawn* SeenPawn)
{
	if(EnemyState == EEnemyState::Combat)
	{
		return;
	}

	if(SeenPawn != PlayerPawn)
	{
		return;
	}

	ResetTimers();
	
	EnemyState = EEnemyState::Combat;
	OnCombatStart.Broadcast();

	for (APS1AI* Enemy : AIEnemySquad)
	{
		Enemy->SeenPawn(SeenPawn);
	}
}

void APS1AI::HeardNoise(APawn* NoiseInstigator, const FVector& Location, float Volume)
{
	if(!PlayerPawn)
	{
		return;
	}
	
	if(EnemyState == EEnemyState::Combat || NoiseInstigator != PlayerPawn)
	{
		return;
	}
	
	LastRot = GetControlRotation().Quaternion();
	SetLookAt(Location);

	ResetTimers();
	EnemyState = EEnemyState::Looking;
	OnLookingStart.Broadcast();
}

void APS1AI::IdleState()
{
	if(bIdleMovement)
	{

		if(IdleTimer >= IdleMovementFrequency)
		{
			IdleTimer = 0;
			
			AIController->MoveToLocation(StartingLoc + FVector(
				FMath::FRandRange(0, IdleMovementAmount),
				FMath::FRandRange(0, IdleMovementAmount),
				0));
		}
		
		IdleTimer += GetWorld()->GetDeltaSeconds();
	}
}

void APS1AI::LookingState()
{
	LookTimer += GetWorld()->GetDeltaSeconds();
	
	Accumulator += GetWorld()->GetDeltaSeconds() * TurnRate;
	Accumulator = Accumulator > 1.f ? 1.f : Accumulator;

	
	SetActorRotation(FQuat::Slerp(LastRot, LookAtRotationYaw, Accumulator).Rotator());

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

void APS1AI::CombatState()
{
	if(!PlayerPawn)
	{
		return;
	}

	bPlayerInsightAndInDistance = false;

	LookTimer += GetWorld()->GetDeltaSeconds();
	
	Accumulator += GetWorld()->GetDeltaSeconds() * TurnRate;
	Accumulator = Accumulator > 1.f ? 1.f : Accumulator;

	SetLookAtLocation(PlayerPawn->GetActorLocation());
	SetActorRotation(FQuat::Slerp(LastRot, LookAtRotationYaw, Accumulator).Rotator());

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
			if(AIController)
			{
				AIController->StopMovement();
			}

			bPlayerInsightAndInDistance = true;
			
			WeaponComponent->Attack();

			return;
		}

		if(AIController)
		{
			AIController->MoveToLocation(PlayerPawn->GetActorLocation());
		}
	}
	else
	{		
		if(AIController)
		{
			AIController->MoveToLocation(PlayerPawn->GetActorLocation());
		}

		OutOfSightTimer += GetWorld()->GetDeltaSeconds();

		if(OutOfSightTimer >= OutOfSightTime)
		{
			ResetTimers();

			SetLookAt(PlayerPawn->GetActorLocation());
			EnemyState = EEnemyState::Looking;
		}
	}
}

void APS1AI::SetEnemyState(EEnemyState const State)
{
	ResetTimers();
	EnemyState = State;
}

void APS1AI::SetLookAtLocation(FVector Target)
{
	SetLookAt(Target);
}

FRotator APS1AI::GetLookAtRotation() const
{
	return LookAtRotation.Rotator();
}

FRotator APS1AI::GetLookAtRotationYaw() const
{
	return LookAtRotationYaw.Rotator();
}

EEnemyState APS1AI::GetEnemyState() const
{
	return EnemyState;
}

bool APS1AI::IsPlayerInSightAndInDistance() const
{
	return bPlayerInsightAndInDistance;
}

FRotator APS1AI::GetLookAtForComponent(USceneComponent* Component, FVector TargetPosition)
{
	FVector const Forward = TargetPosition - Component->GetComponentLocation();
	FRotator const Rot = UKismetMathLibrary::MakeRotFromXZ(Forward, FVector::UpVector);
	return Rot;
}

FRotator APS1AI::GetLookAtPlayerForComponent(USceneComponent* Component)
{
	if(!PlayerPawn)
	{
		return FRotator();
	}
	
	FVector const Forward = PlayerPawn->GetActorLocation() - Component->GetComponentLocation();
	FRotator const Rot = UKismetMathLibrary::MakeRotFromXZ(Forward, FVector::UpVector);
	return Rot;
}

void APS1AI::ResetTimers()
{
	IdleTimer = 0;
	Accumulator = 0;
	LookTimer = 0;
	OutOfSightTimer = 0;
}

void APS1AI::SetLookAt(FVector TargetPosition)
{
	FVector const Forward = TargetPosition - GetActorLocation();
	FRotator Rot = UKismetMathLibrary::MakeRotFromXZ(Forward, FVector::UpVector);
	LookAtRotation = Rot.Quaternion();
	Rot.Pitch = 0;
	Rot.Roll = 0;
	LookAtRotationYaw = Rot.Quaternion();
}
