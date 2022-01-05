#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MiniMap.generated.h"

// USTRUCT()
// struct FTriangle
// {
// 	GENERATED_BODY()
//
// 	TArray<FVector2D> Points;
// };

// USTRUCT()
// struct FMesh
// {
// 	GENERATED_BODY()
//
// 	TArray<FTriangle> Tris;
// 	FColor Color;
// };

USTRUCT()
struct FPixel
{
	GENERATED_BODY()

	FColor Color = FColor(0, 0, 0, 0);
};

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PS1_API AMiniMap : public AActor
{
	GENERATED_BODY()

public:
	AMiniMap();
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable)
	void CreateTexture();

	void AddRectToMiniMap(FVector Location, FVector Extent, FRotator Rot, FColor Color);
	
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
	
	UFUNCTION(BlueprintPure)
	UTexture2D* GetMiniMapTexture()
	{
		return MiniMapTexture;
	}

	UFUNCTION(BlueprintPure)
	int GetSizeX() const
	{
		return TextureSizeX;
	}

	UFUNCTION(BlueprintPure)
	int GetSizeY() const
	{
		return TextureSizeY;
	}

	UFUNCTION(BlueprintPure)
	int GetOffsetX() const
	{
		return TextureOffsetX;
	}

	UFUNCTION(BlueprintPure)
	int GetOffsetY() const
	{
		return TextureOffsetY;
	}

	UFUNCTION(BlueprintPure)
	FVector2D GetTextureSize() const
	{
		return TextureSize;
	}

	UFUNCTION(BlueprintPure)
	FVector2D GetMaxWorldSpace() const
	{
		return SizeMax * SizeDivider;
	}

	UFUNCTION(BlueprintPure)
	FVector2D GetMinWorldSpace() const
	{
		return SizeMin * SizeDivider;
	}
	
	UPROPERTY()
	class UMaterialInstanceDynamic* MiniMapMaterial;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
	class UStaticMeshComponent* MiniMapMesh;

	float SizeDivider = 1.0f;

	FVector2D SizeMax = FVector2D(-10000000, -1000000);
	FVector2D SizeMin = FVector2D(10000000, 10000000);

	// TArray<FMesh> MiniMapQuadsToDraw;

	float Sign(FVector2D a, FVector2D b, FVector2D c)
	{
		return (a.X - c.X) * (b.Y - c.Y) - (b.X - c.X) * (a.Y - c.Y);
	}
	
	// bool PointInTriangle(FVector2D point, FTriangle t)
	// {
	// 	float d1, d2, d3;
	// 	bool negative, positive;
	//
	// 	d1 = Sign(point, t.Points[0], t.Points[1]);
	// 	d2 = Sign(point, t.Points[1], t.Points[2]);
	// 	d3 = Sign(point, t.Points[2], t.Points[0]);
	//
	// 	negative = (d1 < 0) || (d2 < 0) || (d3 < 0);
	// 	positive = (d1 > 0) || (d2 > 0) || (d3 > 0);
	//
	// 	return !(negative && positive);
	// }
};
