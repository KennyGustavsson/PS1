#include "PS1DisplayOnMinimapComponent.h"

#include "PS1/GameMode/PS1GameState.h"

UPS1DisplayOnMinimapComponent::UPS1DisplayOnMinimapComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UPS1DisplayOnMinimapComponent::BeginPlay()
{
	Super::BeginPlay();
	
	APS1GameState* GameState = Cast<APS1GameState>(GetWorld()->GetGameState());
	
	if(GameState)
	{
		if(!IsValidLowLevel())
		{
			return;
		}
		
		TArray<UStaticMeshComponent*> Meshes;
		GetOwner()->GetComponents(Meshes);

		TArray<FVector2D> Vertices;
	
		for(UStaticMeshComponent* Mesh : Meshes)
		{
			if(!Mesh->IsValidLowLevel())
			{
				return;
			}
			
			if(!Mesh->GetStaticMesh())
			{
				return;
			}

			if(!Mesh->GetStaticMesh()->RenderData)
			{
				return;
			}
			
			// GameState->AddRectToMiniMap(Mesh->GetComponentLocation(),Mesh->GetStaticMesh()->ExtendedBounds.BoxExtent * Mesh->GetComponentScale(),
			// 	Mesh->GetComponentRotation(), FColor::Silver);

			if(Mesh->GetStaticMesh()->RenderData->LODResources.Num() > 0)
			{
				FPositionVertexBuffer* VertexBuffer = &Mesh->GetStaticMesh()->RenderData->LODResources[0].VertexBuffers.PositionVertexBuffer;

				if(VertexBuffer)
				{
					const int32 VertexCount = VertexBuffer->GetNumVertices();
					
					for (int32 Index = 0; Index < VertexCount; Index++)
					{
						const FVector WorldSpaceVertexLocation = Mesh->GetComponentTransform().TransformPosition(VertexBuffer->VertexPosition(Index));
						
						Vertices.Add(FVector2D(WorldSpaceVertexLocation.X, WorldSpaceVertexLocation.Y));
					}
				}
			}
		}

		GameState->AddWorldSpaceVertsToMinimap(Vertices);
	}
}
