// ILikeBanas


#include "FSConveyorBeltOperator.h"
#include "Buildables/FGBuildable.h"
#include "Buildables/FGBuildableConveyorBelt.h"
//#include "FGInstancedSplineMesh.h"
//#include "FGInstancedSplineMeshComponent.h"
#include "FactorySkyline/FSkyline.h"


AFGHologram* UFSConveyorBeltOperator::HologramCopy(FTransform& RelativeTransform)
{
	AFSkyline* FSkyline = AFSkyline::Get(this);

	RelativeTransform = Source->GetTransform();
	//AFGHologram* Hologram = CreateHologram();

	TSubclassOf<UFGRecipe> Recipe = SplineHologramFactory->GetRecipeFromClass(Source->GetClass());
	if (!Recipe) return nullptr;
	AFGHologram* Hologram = AFGHologram::SpawnHologramFromRecipe(Recipe, Builder, FVector(0.0f, 0.0f, 0.0f), ((AFSkyline*)Skyline)->FSCtrl->GetPlayer());

	if (!Hologram) return nullptr;
	AFGConveyorBeltHologram* ConveyorBeltHologram = Cast<AFGConveyorBeltHologram>(Hologram);
	if (!ConveyorBeltHologram) return Hologram;

	AFGBuildableConveyorBelt* SourceBelt = Cast<AFGBuildableConveyorBelt>(Source);

	FHitResult Hit;
	//Hit.Actor = nullptr;
	Hit.Time = 0.006946;
	Hit.Location = FVector(-11720.067f, 248538.719f, -10141.936f);
	Hit.ImpactPoint = FVector(-11720.066f, 248538.719f, -10141.936f);
	Hit.Normal = FVector(1.0f, 0.0f, 0.0f);
	Hit.ImpactNormal = FVector(1.0f, 0.0f, 0.0f);
	Hit.TraceStart = FVector(-11025.803f, 248538.188f, -10162.381f);
	Hit.TraceEnd = FVector(-110982.445f, 248615.406f, -12781.198f);
	Hit.PenetrationDepth = 0.0f;
	Hit.Item = -1;
	Hit.FaceIndex = -1;

	//Hologram->SetHologramLocationAndRotation(Hit);
	Hologram->SetActorTransform(Source->GetTransform());
	Hologram->SetPlacementMaterialState(EHologramMaterialState::HMS_OK);

	//UFGInstancedSplineMeshComponent* SourceComponent = Cast<UFGInstancedSplineMeshComponent>(SourceBelt->GetComponentByClass(UFGInstancedSplineMeshComponent::StaticClass()));
	USplineMeshComponent* SplineMeshComponent = nullptr;
	
	TSet<UActorComponent*> Set = Hologram->GetComponents();
	for (UActorComponent* Component : Set) {
		Log("%s", *Component->GetName());
		auto c = Cast<USplineMeshComponent>(Component);
		if(c) {
			SplineMeshComponent = Cast<USplineMeshComponent>(Component);
			break;
		}
	}
	
	bool NeedNew = false;

	//TArray< FSplinePointData >* SourceData = &SourceConveyorBelt->mSplineData;
	TArray< FSplinePointData >* SourceData = FSkyline->AdaptiveUtil->GetConveyorBeltSplineData(SourceBelt);
	AFGSplineHologram* splineHologram = Cast<AFGSplineHologram>(ConveyorBeltHologram);
	TArray< FSplinePointData > TargetData = splineHologram->mSplineData;

	TargetData.Empty();

	for (const FSplinePointData& PointData : *SourceData) {
		FSplinePointData NewPointData;
		NewPointData.Location = PointData.Location;
		NewPointData.ArriveTangent = PointData.ArriveTangent;
		NewPointData.LeaveTangent = PointData.LeaveTangent;
		TargetData.Add(NewPointData);
	}
	splineHologram->mSplineData = TargetData;

	// TODO DO WE NEED A ALTERNATIVE TO THIS?
	//ConveyorBeltHologram->OnPendingConstructionHologramCreated_Implementation(ConveyorBeltHologram);

	return Hologram;
}
/*
void UFSConveyorBeltOperator::UpdateSplineData(AFGSplineHologram* Hologram)
{
	//TArray< FSplinePointData >* SourceData = FSkyline->AdaptiveUtil->GetConveyorBeltSplineData(SourceBelt);
	AFGSplineHologram* splineHologram = Cast<AFGSplineHologram>(Hologram);
	TArray< FSplinePointData > TargetData = splineHologram->mSplineData;

	TargetData.Empty();

	for (const FSplinePointData& PointData : *SourceData) {
		FSplinePointData NewPointData;
		NewPointData.Location = PointData.Location;
		NewPointData.ArriveTangent = PointData.ArriveTangent;
		NewPointData.LeaveTangent = PointData.LeaveTangent;
		TargetData.Add(NewPointData);
	}
	splineHologram->mSplineData = TargetData;
}
*/

AFGBuildable* UFSConveyorBeltOperator::CreateCopy(const FSTransformOperator& TransformOperator)
{
	AFSkyline* FSkyline = AFSkyline::Get(this);

	FVector RelativeVector = TransformOperator.SourceTransform.InverseTransformPositionNoScale(Source->GetTransform().GetLocation());
	FQuat RelativeRotation = TransformOperator.SourceTransform.InverseTransformRotation(Source->GetTransform().GetRotation());
	FQuat Rotation = TransformOperator.TargetTransform.TransformRotation(RelativeRotation);

	FTransform Transform = FTransform(FRotator::ZeroRotator, TransformOperator.TargetTransform.TransformPositionNoScale(RelativeVector), Source->GetTransform().GetScale3D());

	AFGBuildableConveyorBelt* SourceConveyorBelt = Cast<AFGBuildableConveyorBelt>(Source);

	AFGBuildable* Buildable = BuildableSubsystem->BeginSpawnBuildable(Source->GetClass(), Transform);
	AFGBuildableConveyorBelt* TargetConveyorBelt = Cast<AFGBuildableConveyorBelt>(Buildable);

	TSubclassOf<UFGRecipe> Recipe = SplineHologramFactory->GetRecipeFromClass(Source->GetClass());
	if (!Recipe) Recipe = Source->GetBuiltWithRecipe();
	if (!Recipe) return nullptr;

	Buildable->SetBuiltWithRecipe(Recipe);
	//Buildable->SetBuildingID(Source->GetBuildingID());

	//TArray< FSplinePointData >* SourceData = &SourceConveyorBelt->mSplineData;
	TArray< FSplinePointData >* SourceData = FSkyline->AdaptiveUtil->GetConveyorBeltSplineData(SourceConveyorBelt);
	//TArray< FSplinePointData >* SourceData = &TargetConveyorBelt->mSplineData;
	TArray< FSplinePointData >* TargetData = FSkyline->AdaptiveUtil->GetConveyorBeltSplineData(TargetConveyorBelt);

	for (const FSplinePointData& PointData : *SourceData) {
		FSplinePointData NewPointData;
		NewPointData.Location = Rotation.RotateVector(PointData.Location);
		NewPointData.ArriveTangent = Rotation.RotateVector(PointData.ArriveTangent);
		NewPointData.LeaveTangent = Rotation.RotateVector(PointData.LeaveTangent);
		TargetData->Add(NewPointData);
	}

	Buildable->SetCustomizationData_Implementation(Source->GetCustomizationData_Implementation());
	Buildable->FinishSpawning(Transform);

	this->BuildableSubsystem->RemoveConveyorFromBucket(TargetConveyorBelt);

	return Buildable;
}


