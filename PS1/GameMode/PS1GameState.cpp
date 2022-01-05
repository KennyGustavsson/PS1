#include "PS1GameState.h"

#include "ImageUtils.h"

void APS1GameState::CreateTexture()
{
	if(MinimapMeshes.Num() == 0)
	{
		return;
	}
	
	float const OffsetX = SizeMin.X < 0 ? -SizeMin.X : SizeMin.X;
	float const OffsetY = SizeMin.Y < 0 ? -SizeMin.Y : SizeMin.Y;
	
	int32 const SizeX = FVector2D::Distance(FVector2D(SizeMin.X + OffsetX, OffsetY), FVector2D(SizeMax.X + OffsetX, OffsetY));
	int32 const SizeY = FVector2D::Distance(FVector2D(OffsetX, SizeMin.Y + OffsetY), FVector2D(OffsetX, SizeMax.Y + OffsetY));
	
	uint32 const ArraySize = SizeX * SizeY;
	FColor* PixelColors = new FColor[ArraySize];

	// Initialize pixels as black
	for (int i = 0; i < SizeX * SizeY; i++)
	{
		PixelColors[i] = FColor::Black;
	}

	// Draw triangles
	// for(FMesh QuadMesh : MiniMapQuadsToDraw)
	// {
	for(FMesh QuadMesh : MinimapMeshes)
	{
		for(FTriangle t : QuadMesh.Tris)
		{
			t.Points[0].X += OffsetX;
			t.Points[0].Y += OffsetY;

			t.Points[1].X += OffsetX;
			t.Points[1].Y += OffsetY;

			t.Points[2].X += OffsetX;
			t.Points[2].Y += OffsetY;

			for (int i = 0; i < 3; i++)
			{
				if(i == 2)
				{
					float const N = DiagonalDistance(t.Points[i], t.Points[0]);

					for (int j = 0; j <= N; j++)
					{
						float const ti = (N == 0) ? 0.0 : j / N;
						float const x = Lerp(t.Points[i].X, t.Points[0].X, ti);
						float const y = Lerp(t.Points[i].Y, t.Points[0].Y, ti);

						int32 const CurrentPixelIndex = ((y * SizeX) + x);
						PixelColors[CurrentPixelIndex] = FColor::Silver;
				
					}
					
					return;
				}

				float const N = DiagonalDistance(t.Points[i], t.Points[i + 1]);

				for (int j = 0; j <= N; j++)
				{
					float const ti = (N == 0) ? 0.0 : j / N;
					float const x = Lerp(t.Points[i].X, t.Points[i + 1].X, ti);
					float const y = Lerp(t.Points[i].Y, t.Points[i + 1].Y, ti);

					int32 const CurrentPixelIndex = ((y * SizeX) + x);
					PixelColors[CurrentPixelIndex] = FColor::Silver;
				
				}
			}
			
			// int const MaxX = FMath::Max(t.Points[0].X, FMath::Max(t.Points[1].X, t.Points[2].X));
			// int const MinX = FMath::Min(t.Points[0].X, FMath::Min(t.Points[1].X, t.Points[2].X));
			//
			// int const MaxY = FMath::Max(t.Points[0].Y, FMath::Max(t.Points[1].Y, t.Points[2].Y));
			// int const MinY = FMath::Min(t.Points[0].Y, FMath::Min(t.Points[1].Y, t.Points[2].Y));

			// for (int y = MinY; y < MaxY + 1; y++)
			// {
			// 	for (int x = MinX; x < MaxX + 1; x++)
			// 	{
			// 		FVector2D const Point = FVector2D(x, y);
			// 		int32 const CurrentPixelIndex = ((y * SizeX) + x);
			// 		
			// 		if (PointInTriangle(Point, t))
			// 		{
			// 			PixelColors[CurrentPixelIndex] = FColor::Silver;
			// 		}
			// 	}
			// }
		}
	}
	
	UTexture2D* Texture = UTexture2D::CreateTransient(SizeX, SizeY);
	
	// Texture Settings
	Texture->PlatformData = new FTexturePlatformData();
	Texture->PlatformData->SizeX = SizeX;
	Texture->PlatformData->SizeY = SizeY;
	Texture->PlatformData->PixelFormat = PF_R8G8B8A8;
	Texture->SRGB = 0;
	
	// Passing the pixels information to the texture
	FTexture2DMipMap* Mip = new(Texture->PlatformData->Mips) FTexture2DMipMap();
	Mip->SizeX = SizeX;
	Mip->SizeY = SizeY;
	Mip->BulkData.Lock(LOCK_READ_WRITE);
	FColor* TextureData = (FColor*) Mip->BulkData.Realloc(SizeY * SizeX * sizeof(FColor));
	
	FMemory::Memcpy(TextureData, PixelColors, sizeof(FColor) * SizeY * SizeX);
	Mip->BulkData.Unlock();
	
	// Updating Texture & mark it as unsaved
	Texture->AddToRoot();
	Texture->UpdateResource();

	MiniMapTexture = Texture;
	TextureSizeX = SizeX;
	TextureSizeY = SizeY;
	TextureOffsetX = OffsetX;
	TextureOffsetY = OffsetY;
	TextureSize = FVector2D(SizeX, SizeY);
	
	delete PixelColors;	
}

void APS1GameState::AddRectToMiniMap(FVector Location, FVector Extent, FRotator Rot, FColor Color)
{
	float const RotationRad = FMath::DegreesToRadians(Rot.Yaw);
	Extent.X /= SizeDivider;
	Extent.Y /= SizeDivider;
	Location.X /= SizeDivider;
	Location.Y /= SizeDivider;
	
	FMesh MiniMapQuad;

	FTriangle Triangle1;
	Triangle1.Points.Add(FVector2D(Extent.X, Extent.Y));
	Triangle1.Points.Add(FVector2D(-Extent.X, Extent.Y));
	Triangle1.Points.Add(FVector2D(Extent.X, -Extent.Y));

	FTriangle Triangle2;
	Triangle2.Points.Add(FVector2D(-Extent.X, Extent.Y));
	Triangle2.Points.Add(FVector2D(-Extent.X, -Extent.Y));
	Triangle2.Points.Add(FVector2D(Extent.X, -Extent.Y));
	
	MiniMapQuad.Tris.Add(Triangle1);
	MiniMapQuad.Tris.Add(Triangle2);

	// Transform Triangles
	FMesh Transformed;

	// Rotation
	for (int i = 0; i < MiniMapQuad.Tris.Num(); i++)
	{
		Transformed.Tris.Add(MiniMapQuad.Tris[i]);
		
		for (int j = 0; j < 3; j++)
		{
			Transformed.Tris[i].Points[j].X = MiniMapQuad.Tris[i].Points[j].X * FMath::Cos(RotationRad) - MiniMapQuad.Tris[i].Points[j].Y * FMath::Sin(RotationRad);
			Transformed.Tris[i].Points[j].Y = MiniMapQuad.Tris[i].Points[j].X * FMath::Sin(RotationRad) + MiniMapQuad.Tris[i].Points[j].Y * FMath::Cos(RotationRad);
		}
	}

	// Translate
	for (int i = 0; i < Transformed.Tris.Num(); i++)
	{
		for (int j = 0; j < 3; j++)
		{
			Transformed.Tris[i].Points[j].X += Location.X;
			Transformed.Tris[i].Points[j].Y += Location.Y;

			// Min Max Check
			if(Transformed.Tris[i].Points[j].X > SizeMax.X)
			{
				SizeMax.X = Transformed.Tris[i].Points[j].X;
			}

			if(Transformed.Tris[i].Points[j].X < SizeMin.X)
			{
				SizeMin.X = Transformed.Tris[i].Points[j].X;
			}

			if(Transformed.Tris[i].Points[j].Y > SizeMax.Y)
			{
				SizeMax.Y = Transformed.Tris[i].Points[j].Y;
			}

			if(Transformed.Tris[i].Points[j].Y < SizeMin.Y)
			{
				SizeMin.Y = Transformed.Tris[i].Points[j].Y;
			}
		}
	}

	Transformed.Color = Color;
	
	MiniMapQuadsToDraw.Add(Transformed);
}

void APS1GameState::AddWorldSpaceVertsToMinimap(TArray<FVector2D> &Vertices)
{
	FMesh Mesh;
	Mesh.Color = FColor::Silver;
	
	for (int i = 0; i < Vertices.Num(); i += 3)
	{
		FTriangle Tri;
		
		if(i + 3 == Vertices.Num() + 1)
		{
			Vertices[i] /= SizeDivider;
			Vertices[i + 1] /= SizeDivider;

			Tri.Points.Add(Vertices[i]);
			Tri.Points.Add(Vertices[i + 1]);
			Tri.Points.Add(Vertices[0]);
		}
		else if(i + 3 == Vertices.Num() + 2)
		{
			Vertices[i] /= SizeDivider;

			Tri.Points.Add(Vertices[i]);
			Tri.Points.Add(Vertices[0]);
			Tri.Points.Add(Vertices[1]);
		}
		else
		{
			Vertices[i] /= SizeDivider;
			Vertices[i + 1] /= SizeDivider;
			Vertices[i + 2] /= SizeDivider;

			Tri.Points.Add(Vertices[i]);
			Tri.Points.Add(Vertices[i + 1]);
			Tri.Points.Add(Vertices[i + 2]);
		}

		Mesh.Tris.Add(Tri);
	}

	for (int i = 0; i < Mesh.Tris.Num(); i++)
	{
		for (int j = 0; j < 3; j++)
		{

			// Min Max Check
			if(Mesh.Tris[i].Points[j].X > SizeMax.X)
			{
				SizeMax.X = Mesh.Tris[i].Points[j].X;
			}

			if(Mesh.Tris[i].Points[j].X < SizeMin.X)
			{
				SizeMin.X = Mesh.Tris[i].Points[j].X;
			}

			if(Mesh.Tris[i].Points[j].Y > SizeMax.Y)
			{
				SizeMax.Y = Mesh.Tris[i].Points[j].Y;
			}

			if(Mesh.Tris[i].Points[j].Y < SizeMin.Y)
			{
				SizeMin.Y = Mesh.Tris[i].Points[j].Y;
			}
		}
	}
	
	MinimapMeshes.Add(Mesh);
}

float APS1GameState::Sign(FVector2D const A, FVector2D const B, FVector2D const C)
{
	return (A.X - C.X) * (B.Y - C.Y) - (B.X - C.X) * (A.Y - C.Y);
}

bool APS1GameState::PointInTriangle(FVector2D const Point, FTriangle T)
{
	float const D1 = Sign(Point, T.Points[0], T.Points[1]);
	float const D2 = Sign(Point, T.Points[1], T.Points[2]);
	float const D3 = Sign(Point, T.Points[2], T.Points[0]);

	bool const Negative = (D1 < 0) || (D2 < 0) || (D3 < 0);
	bool const Positive = (D1 > 0) || (D2 > 0) || (D3 > 0);

	return !(Negative && Positive);
}

float APS1GameState::Lerp(float v0, float v1, float t)
{
	return (1 - t) * v0 + t * v1;
}

float APS1GameState::DiagonalDistance(FVector2D A, FVector2D B)
{
	float dx = B.X - A.X;
	float dy = B.Y - A.Y;
	return FMath::Max(FMath::Abs(dx), FMath::Abs(dy));
}

