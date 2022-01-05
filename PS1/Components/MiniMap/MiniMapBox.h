#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MiniMapBox.generated.h"

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent), BlueprintType, Blueprintable)
class PS1_API AMiniMapBox : public AActor
{
	GENERATED_BODY()

public:
	AMiniMapBox();
	virtual void BeginPlay() override;

	UPROPERTY()
	class UBoxComponent* Box;

	UPROPERTY(EditAnywhere)
	AActor* MiniMapActor;
};
