#include "MiniMap.h"

AMiniMap::AMiniMap()
{
	MiniMapMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
}

void AMiniMap::BeginPlay()
{
	Super::BeginPlay();
}

void AMiniMap::CreateTexture()
{
	return;
	
	// if(MiniMapQuadsToDraw.Num() == 0)
	// {
	// 	return;
	// }
	
	float OffsetX;
	float OffsetY;

	if(SizeMin.X < 0)
	{
		OffsetX = -SizeMin.X;
	}
	else
	{
		OffsetX = SizeMin.X;
	}

	if(SizeMin.Y < 0)
	{
		OffsetY = -SizeMin.Y;
	}
	else
	{
		OffsetY = SizeMin.Y;
	}
	
	int32 const SizeX = FVector2D::Distance(FVector2D(SizeMin.X + OffsetX, OffsetY), FVector2D(SizeMax.X + OffsetX, OffsetY));
	int32 const SizeY = FVector2D::Distance(FVector2D(OffsetX, SizeMin.Y + OffsetY), FVector2D(OffsetX, SizeMax.Y + OffsetY));
	
	uint32 const ArraySize = SizeX * SizeY;
	FColor* PixelColors = new FColor[ArraySize];

	for (int i = 0; i < SizeX * SizeY; i++)
	{
		PixelColors[i] = FColor::Black;
	}
	
	// for(FMesh QuadMesh : MiniMapQuadsToDraw)
	// {
	// 	for(FTriangle t : QuadMesh.Tris)
	// 	{
	// 		t.Points[0].X += OffsetX;
	// 		t.Points[0].Y += OffsetY;
	//
	// 		t.Points[1].X += OffsetX;
	// 		t.Points[1].Y += OffsetY;
	//
	// 		t.Points[2].X += OffsetX;
	// 		t.Points[2].Y += OffsetY;
	// 		
	// 		int maxX = FMath::Max(t.Points[0].X, FMath::Max(t.Points[1].X, t.Points[2].X));
	// 		int minX = FMath::Min(t.Points[0].X, FMath::Min(t.Points[1].X, t.Points[2].X));
	//
	// 		int maxY = FMath::Max(t.Points[0].Y, FMath::Max(t.Points[1].Y, t.Points[2].Y));
	// 		int minY = FMath::Min(t.Points[0].Y, FMath::Min(t.Points[1].Y, t.Points[2].Y));
	//
	// 		for (int y = minY; y < maxY + 1; y++)
	// 		{
	// 			for (int x = minX; x < maxX + 1; x++)
	// 			{
	// 				FVector2D point = FVector2D(x, y);
	//
	// 				int32 const CurrentPixelIndex = ((y * SizeX) + x);
	// 				
	// 				if (PointInTriangle(point, t))
	// 				{
	// 					PixelColors[CurrentPixelIndex] = QuadMesh.Color;
	// 				}
	// 			}
	// 		}
	// 	}
	// }
	
	// Create Package
	FString FileName = FString("MyTexture");
	
	FString const PathPackage = FString("/Game/MyTextures/");
	FString const AbsolutePathPackage = FPaths::GameDevelopersDir() + "/MyTextures/";
	FPackageName::RegisterMountPoint(*PathPackage, *AbsolutePathPackage);
	UPackage* Package = CreatePackage(*PathPackage);
	
	// Create the Texture
	FName const TextureName = MakeUniqueObjectName(Package, UTexture2D::StaticClass(), FName(*FileName));
	UTexture2D* Texture = NewObject<UTexture2D>(Package, TextureName, RF_Public | RF_Standalone);
	
	// Texture Settings
	Texture->PlatformData = new FTexturePlatformData();
	Texture->PlatformData->SizeX = SizeX;
	Texture->PlatformData->SizeY = SizeY;
	Texture->PlatformData->PixelFormat = PF_FloatRGB; //PF_A32B32G32R32F; //PF_FloatRGB; //PF_B8G8R8A8; // PF_R8G8B8A8;
	Texture->SRGB = 0;
	
	// Passing the pixels information to the texture
	FTexture2DMipMap* Mip = new(Texture->PlatformData->Mips) FTexture2DMipMap();
	Mip->SizeX = SizeX;
	Mip->SizeY = SizeY;
	Mip->BulkData.Lock(LOCK_READ_WRITE);
	FColor* TextureData = (FColor *) Mip->BulkData.Realloc(SizeY * SizeX * sizeof(FColor));
	
	FMemory::Memcpy(TextureData, PixelColors, sizeof(FColor) * SizeY * SizeX);
	Mip->BulkData.Unlock();
	
	// Updating Texture & mark it as unsaved
	Texture->AddToRoot();
	Texture->UpdateResource();
	Package->MarkPackageDirty();
	
	MiniMapMaterial = MiniMapMesh->CreateDynamicMaterialInstance(0, MiniMapMaterial);
	MiniMapMaterial->SetTextureParameterValue("RT_MiniMap", Texture);

	MiniMapTexture = Texture;
	TextureSizeX = SizeX;
	TextureSizeY = SizeY;
	TextureOffsetX = OffsetX;
	TextureOffsetY = OffsetY;
	TextureSize = FVector2D(SizeX, SizeY);
	
	delete PixelColors;	
}

void AMiniMap::AddRectToMiniMap(FVector Location, FVector Extent, FRotator Rot, FColor Color)
{
	// float const RotationRad = FMath::DegreesToRadians(Rot.Yaw);
	// Extent.X /= SizeDivider;
	// Extent.Y /= SizeDivider;
	// Location.X /= SizeDivider;
	// Location.Y /= SizeDivider;
	//
	// FMesh MiniMapQuad;
	//
	// FTriangle Triangle1;
	// Triangle1.Points.Add(FVector2D(Extent.X, Extent.Y));
	// Triangle1.Points.Add(FVector2D(-Extent.X, Extent.Y));
	// Triangle1.Points.Add(FVector2D(Extent.X, -Extent.Y));
	//
	// FTriangle Triangle2;
	// Triangle2.Points.Add(FVector2D(-Extent.X, Extent.Y));
	// Triangle2.Points.Add(FVector2D(-Extent.X, -Extent.Y));
	// Triangle2.Points.Add(FVector2D(Extent.X, -Extent.Y));
	//
	// MiniMapQuad.Tris.Add(Triangle1);
	// MiniMapQuad.Tris.Add(Triangle2);
	//
	// // Transform Triangles
	// FMesh Transformed;
	//
	// // Rotation
	// for (int i = 0; i < MiniMapQuad.Tris.Num(); i++)
	// {
	// 	Transformed.Tris.Add(MiniMapQuad.Tris[i]);
	// 	
	// 	for (int j = 0; j < 3; j++)
	// 	{
	// 		Transformed.Tris[i].Points[j].X = MiniMapQuad.Tris[i].Points[j].X * FMath::Cos(RotationRad) - MiniMapQuad.Tris[i].Points[j].Y * FMath::Sin(RotationRad);
	// 		Transformed.Tris[i].Points[j].Y = MiniMapQuad.Tris[i].Points[j].X * FMath::Sin(RotationRad) + MiniMapQuad.Tris[i].Points[j].Y * FMath::Cos(RotationRad);
	// 	}
	// }
	//
	// // Translate
	// for (int i = 0; i < Transformed.Tris.Num(); i++)
	// {
	// 	for (int j = 0; j < 3; j++)
	// 	{
	// 		Transformed.Tris[i].Points[j].X += Location.X;
	// 		Transformed.Tris[i].Points[j].Y += Location.Y;
	//
	// 		// Min Max Check
	// 		if(Transformed.Tris[i].Points[j].X > SizeMax.X)
	// 		{
	// 			SizeMax.X = Transformed.Tris[i].Points[j].X;
	// 		}
	//
	// 		if(Transformed.Tris[i].Points[j].X < SizeMin.X)
	// 		{
	// 			SizeMin.X = Transformed.Tris[i].Points[j].X;
	// 		}
	//
	// 		if(Transformed.Tris[i].Points[j].Y > SizeMax.Y)
	// 		{
	// 			SizeMax.Y = Transformed.Tris[i].Points[j].Y;
	// 		}
	//
	// 		if(Transformed.Tris[i].Points[j].Y < SizeMin.Y)
	// 		{
	// 			SizeMin.Y = Transformed.Tris[i].Points[j].Y;
	// 		}
	// 	}
	// }
	//
	// Transformed.Color = Color;
	//
	// MiniMapQuadsToDraw.Add(Transformed);
}
