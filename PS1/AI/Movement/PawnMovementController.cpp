#include "PawnMovementController.h"

#include "AI/Navigation/NavigationDataInterface.h"
#include "Kismet/KismetMathLibrary.h"

UPawnMovementController::UPawnMovementController()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = true;
}

void UPawnMovementController::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if(!bHasRequestedVelocity)
		return;

	GetOwner()->AddActorWorldOffset(RequestedVelocity * DeltaTime);
	FRotator Rot = UKismetMathLibrary::MakeRotFromX(RequestedVelocity);
	GetOwner()->SetActorRotation(FRotator(0.0f, Rot.Yaw, 0.0f));

	FNavLocation NavFloorLocation;
	if(FindNavFloor(GetOwner()->GetActorLocation(), NavFloorLocation))
	{
		float FloorHeight = NavFloorLocation.Location.Z;
		FVector CurrentLocation = GetOwner()->GetActorLocation();
		CurrentLocation.Z = FloorHeight + GetPawnOwner()->GetDefaultHalfHeight() + FloorOffset;
		GetOwner()->SetActorLocation(CurrentLocation);
	}
}

void UPawnMovementController::RequestDirectMove(const FVector& MoveVelocity, bool bForceMaxSpeed)
{
	RequestedVelocity = MoveVelocity.GetSafeNormal() * MovementSpeed;
	bHasRequestedVelocity = true;
}

void UPawnMovementController::StopActiveMovement()
{
	Super::StopActiveMovement();

	RequestedVelocity = FVector::ZeroVector;
	bHasRequestedVelocity = false;
}

bool UPawnMovementController::CanStartPathFollowing() const
{
	return true;
}

bool UPawnMovementController::CanStopPathFollowing() const
{
	return true;
}

float UPawnMovementController::GetPathFollowingBrakingDistance(float MaxSpeed) const
{
	return 1.0f;
}

bool UPawnMovementController::FindNavFloor(const FVector& TestLocation, FNavLocation& NavFloorLocation) const
{
	const INavigationDataInterface* NavData = GetNavData();
	if(!NavData)
	{
		return false;
	}

	const FNavAgentProperties& AgentProps = GetPawnOwner()->GetNavAgentPropertiesRef();
	const float SearchRadius = AgentProps.AgentRadius * 2.0f;
	const float SearchHeight = AgentProps.AgentHeight * AgentProps.NavWalkingSearchHeightScale;

	return  NavData->ProjectPoint(TestLocation - FVector(0.0f, 0.0f, GetPawnOwner()->GetDefaultHalfHeight()),
		NavFloorLocation, FVector(SearchRadius, SearchRadius, SearchHeight));
}

const INavigationDataInterface* UPawnMovementController::GetNavData() const
{
	const UWorld* World = GetWorld();
	if(!World || !World->GetNavigationSystem())
	{
		return nullptr;
	}

	const INavigationDataInterface* NavData = FNavigationSystem::GetNavDataForActor(*GetOwner());
	return NavData;
}

bool UPawnMovementController::IsMoving() const
{
	return bHasRequestedVelocity;
}
