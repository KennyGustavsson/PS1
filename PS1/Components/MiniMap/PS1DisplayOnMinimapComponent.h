#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PS1DisplayOnMinimapComponent.generated.h"

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PS1_API UPS1DisplayOnMinimapComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UPS1DisplayOnMinimapComponent();
	virtual void BeginPlay() override;
};
