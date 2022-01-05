#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "BlueprintLibrary.generated.h"

UCLASS()
class PS1_API UBlueprintLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Get Component Relative Rotation", CompactTitle = "GetCompRelativeRot", Category = "Custom"))
	static FRotator GetRelativeComponentRotation(USceneComponent* Component);

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Set Component Relative Rotation", CompactTitle = "SetCompRelativeRot", Category = "Custom"))
	static void SetRelativeComponentRotation(USceneComponent* Component, FRotator const Rotation);

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "DegreesToRadians", CompactTitle = "DegToRad", Category = "Custom"))
	static float DegreesToRadians(float const Degrees);
};
