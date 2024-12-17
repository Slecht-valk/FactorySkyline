// Copyright Coffee Stain Studios. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "HologramHelper.generated.h"

UCLASS()
class FACTORYSKYLINE_API AHologramHelper : public AActor
{
	GENERATED_BODY()

	UPROPERTY(Category = Meshes, VisibleAnywhere)
	UStaticMeshComponent* CubeMesh;
	UStaticMeshComponent* cubeMeshComponent;
	
public:	
	// Sets default values for this actor's properties
	AHologramHelper();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	void Initialize(FString Path);
	void InitializeTest(UStaticMesh* Mesh);
	void InitializeTest2(UStaticMeshComponent* Mesh);

};
