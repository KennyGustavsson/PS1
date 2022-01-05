#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "PS1GameState.generated.h"

USTRUCT()
struct FTriangle
{
	GENERATED_BODY()

public:
	TArray<FVector2D> Points;
};

USTRUCT()
struct FMesh
{
	GENERATED_BODY()

public:
	TArray<FTriangle> Tris;
	FColor Color;
};

UCLASS(BlueprintType, Blueprintable)
class PS1_API APS1GameState : public AGameStateBase
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void CreateTexture();

	void AddRectToMiniMap(FVector Location, FVector Extent, FRotator Rot, FColor Color);
	
	UFUNCTION(BlueprintPure)
	UTexture2D* GetMiniMapTexture() const
	{
		return MiniMapTexture;
	}

	// Function for shader
	UFUNCTION(BlueprintPure)
	FVector2D GetMaxWorldSpace() const
	{
		return SizeMax * SizeDivider;
	}

	// Function for shader
	UFUNCTION(BlueprintPure)
	FVector2D GetMinWorldSpace() const
	{
		return SizeMin * SizeDivider;
	}

	UFUNCTION()
	void AddWorldSpaceVertsToMinimap(TArray<FVector2D> &Vertices);

private:
	// Triangle functions
	static float Sign(FVector2D const A, FVector2D const B, FVector2D const C);
	static bool PointInTriangle(FVector2D const Point, FTriangle T);

	static float Lerp(float v0, float v1, float t);
	static float DiagonalDistance(FVector2D A, FVector2D B);

	UPROPERTY(Transient)
	UTexture2D* MiniMapTexture;

	UPROPERTY(Transient)
	int TextureSizeX;
	
	UPROPERTY(Transient)
	int TextureSizeY;

	UPROPERTY(Transient)
	int TextureOffsetX;

	UPROPERTY(Transient)
	int TextureOffsetY;

	UPROPERTY(Transient)
	FVector2D TextureSize;

	UPROPERTY(EditDefaultsOnly)
	float SizeDivider = 1.0f;
	
	TArray<FMesh> MiniMapQuadsToDraw;
	FVector2D SizeMax = FVector2D(-10000000, -1000000);
	FVector2D SizeMin = FVector2D(10000000, 10000000);

	TArray<FMesh> MinimapMeshes;
};
