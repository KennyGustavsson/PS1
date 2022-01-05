#include "BlueprintLibrary.h"

FRotator UBlueprintLibrary::GetRelativeComponentRotation(USceneComponent* Component)
{
	return Component->GetRelativeRotation();
}

void UBlueprintLibrary::SetRelativeComponentRotation(USceneComponent* Component, FRotator const Rotation)
{
	Component->SetRelativeRotation(Rotation);
}

float UBlueprintLibrary::DegreesToRadians(float const Degrees)
{
	return FMath::DegreesToRadians(Degrees);
}
