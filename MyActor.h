// Copyright Coffee Stain Studios. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MyActor.generated.h"

UCLASS()
class FACTORYSKYLINE_API AMyActor : public AActor
{
	GENERATED_BODY()

	UPROPERTY(Category = Meshes, VisibleAnywhere)
	UStaticMeshComponent* CubeMesh;
	
public:	
	// Sets default values for this actor's properties
	AMyActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
