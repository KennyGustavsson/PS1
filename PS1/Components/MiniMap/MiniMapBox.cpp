#include "MiniMapBox.h"

#include "MiniMap.h"
#include "Components/BoxComponent.h"
#include "PS1/GameMode/PS1GameState.h"

AMiniMapBox::AMiniMapBox()
{
	Box = CreateDefaultSubobject<UBoxComponent>(TEXT("Box"));
}

void AMiniMapBox::BeginPlay()
{
	Super::BeginPlay();

	APS1GameState* GameState = Cast<APS1GameState>(GetWorld()->GetGameState());

	if(GameState)
	{
		GameState->AddRectToMiniMap(Box->GetComponentLocation(), Box->GetScaledBoxExtent(), Box->GetComponentRotation(), FColor::Silver);
	}

	
	AMiniMap* MiniMap = Cast<AMiniMap>(MiniMapActor);

	if(MiniMap)
	{
		MiniMap->AddRectToMiniMap(Box->GetComponentLocation(), Box->GetScaledBoxExtent(), Box->GetComponentRotation(), FColor::Silver);
	}
}
