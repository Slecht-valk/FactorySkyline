// Copyright Coffee Stain Studios. All Rights Reserved.


#include "HologramHelper.h"

#include "FSSelection.h"
#include "FSDesign.h"
#include "FSkyline.h"
#include "Buildables/FGBuildable.h"
#include "Buildables/FGBuildableFoundation.h"
#include "FSBuildableService.h"
#include "FGFactorySettings.h"
#include "FGProductionIndicatorInstanceComponent.h"
#include "Components/ProxyInstancedStaticMeshComponent.h"

// Sets default values
AHologramHelper::AHologramHelper()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Create a static mesh component
	cubeMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Cube"));

	//TInlineComponentArray<UMeshComponent*> MeshComps(Buildable);

	/*
	UTexture* Scanline = Cast<UTexture2D>(StaticLoadObject(UTexture2D::StaticClass(), this, TEXT("/FactorySkyline/Icon/White.White")));

	UMaterialInstanceDynamic* SelectMaterial = UMaterialInstanceDynamic::Create(UFGFactorySettings::Get()->mDefaultInvalidPlacementMaterial, this);
	SelectMaterial->SetScalarParameterValue(FName("MinOpacity"), 0.05f);
	SelectMaterial->SetScalarParameterValue(FName("MaxOpacity"), 0.25f);
	SelectMaterial->SetVectorParameterValue(FName("Color"), FLinearColor(0.043735f, 0.201556f, 0.496933f));
	SelectMaterial->SetTextureParameterValue(FName("LineTexture"), Scanline);
	*/

	/*
	AFSkyline* FSkyline = AFSkyline::Get(this);

	
	int num = cubeMeshComponent->GetNumMaterials();
	if (num) {
		for (int i = 0; i < num; i++) {
			cubeMeshComponent->SetMaterial(i, FSkyline->FSCtrl->Select->SelectMaterial);
		}
	}
	*/
	

	// Load the Cube mesh
	//UStaticMesh* cubeMesh = ConstructorHelpers::FObjectFinder<UStaticMesh>(TEXT("StaticMesh'/Engine/BasicShapes/Cube.Cube'")).Object;
	//UStaticMesh* cubeMesh = ConstructorHelpers::FObjectFinder<UStaticMesh>(TEXT("StaticMesh'/Game/FactoryGame/Buildable/Building/Foundation/FicsitSet/SM_Foundation_FicsitSet_8x1_01.SM_Foundation_FicsitSet_8x1_01'")).Object;

	// Set the component's mesh
	//cubeMeshComponent->SetStaticMesh(cubeMesh);

	// Set as root component
	//RootComponent = cubeMeshComponent;

	//PrimaryActorTick.bCanEverTick = true;
	//PrimaryActorTick.bStartWithTickEnabled = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent->SetMobility(EComponentMobility::Static);

}

void AHologramHelper::Initialize(FString Path) {
	// Load the Cube mesh
	//UStaticMesh* cubeMesh = ConstructorHelpers::FObjectFinder<UStaticMesh>(TEXT("StaticMesh'/Game/FactoryGame/Buildable/Building/Foundation/FicsitSet/SM_Foundation_FicsitSet_8x1_01.SM_Foundation_FicsitSet_8x1_01'")).Object;
	//UStaticMesh* cubeMesh = LoadObject<UStaticMesh>(nullptr, TEXT("StaticMesh'/Game/FactoryGame/Buildable/Building/Foundation/FicsitSet/SM_Foundation_FicsitSet_8x1_01.SM_Foundation_FicsitSet_8x1_01'"));
	//std::string  str = TCHAR_TO_UTF8(*Path);
	UStaticMesh* cubeMesh = LoadObject<UStaticMesh>(nullptr, *Path);

	// Set the component's mesh
	cubeMeshComponent->SetStaticMesh(cubeMesh);

	// Set as root component
	RootComponent = cubeMeshComponent;

}

void AHologramHelper::InitializeTest(UStaticMesh* Mesh) {
	// Load the Cube mesh
	//UStaticMesh* cubeMesh = ConstructorHelpers::FObjectFinder<UStaticMesh>(TEXT("StaticMesh'/Game/FactoryGame/Buildable/Building/Foundation/FicsitSet/SM_Foundation_FicsitSet_8x1_01.SM_Foundation_FicsitSet_8x1_01'")).Object;
	//UStaticMesh* cubeMesh = LoadObject<UStaticMesh>(nullptr, TEXT("StaticMesh'/Game/FactoryGame/Buildable/Building/Foundation/FicsitSet/SM_Foundation_FicsitSet_8x1_01.SM_Foundation_FicsitSet_8x1_01'"));
	//std::string  str = TCHAR_TO_UTF8(*Path);
	UStaticMesh* cubeMesh = Mesh;

	// Set the component's mesh
	cubeMeshComponent->SetStaticMesh(cubeMesh);

	// Set as root component
	RootComponent = cubeMeshComponent;

}

void AHologramHelper::InitializeTest2(UStaticMeshComponent* Mesh) {
	// Load the Cube mesh
	//UStaticMesh* cubeMesh = ConstructorHelpers::FObjectFinder<UStaticMesh>(TEXT("StaticMesh'/Game/FactoryGame/Buildable/Building/Foundation/FicsitSet/SM_Foundation_FicsitSet_8x1_01.SM_Foundation_FicsitSet_8x1_01'")).Object;
	//UStaticMesh* cubeMesh = LoadObject<UStaticMesh>(nullptr, TEXT("StaticMesh'/Game/FactoryGame/Buildable/Building/Foundation/FicsitSet/SM_Foundation_FicsitSet_8x1_01.SM_Foundation_FicsitSet_8x1_01'"));
	//std::string  str = TCHAR_TO_UTF8(*Path);
	//UStaticMesh* cubeMesh = Mesh;

	// Set the component's mesh
	//cubeMeshComponent = Mesh;

	// Set as root component
	RootComponent = Mesh;

}

// Called when the game starts or when spawned
void AHologramHelper::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AHologramHelper::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

