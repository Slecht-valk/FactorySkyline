// ILikeBanas


#include "FSPipelineSupportOperator.h"
#include "FGBuildablePipelineSupport.h"
#include "Hologram/FGPipelineSupportHologram.h"


AFGHologram* UFSPipelineSupportOperator::HologramCopy(FTransform& RelativeTransform)
{
	//RelativeTransform = Source->GetTransform();

	if (Source.Buildable) {
		RelativeTransform = Source.Buildable->GetTransform();
	}

	AFGHologram* Hologram = CreateHologram();
	if (!Hologram) return nullptr;
	AFGPipelineSupportHologram* PipelineSupportHologram = Cast<AFGPipelineSupportHologram>(Hologram);
	if (!PipelineSupportHologram) return Hologram;

	AFGBuildablePipelineSupport* SourcePipelineSupport = nullptr;

	if (Source.Buildable) {
		SourcePipelineSupport = Cast<AFGBuildablePipelineSupport>(Source.Buildable);
	}

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

	PipelineSupportHologram->SetHologramLocationAndRotation(Hit);
	PipelineSupportHologram->DoMultiStepPlacement(false);
	PipelineSupportHologram->SetVerticalAngle(SourcePipelineSupport->mVerticalAngle);
	PipelineSupportHologram->SetSupportLength(SourcePipelineSupport->mLength);

	return PipelineSupportHologram;
}

AFGBuildable* UFSPipelineSupportOperator::CreateCopy(const FSTransformOperator& TransformOperator)
{
	//FTransform Transform = TransformOperator.Transform(Source->GetTransform());

	FTransform Transform;

	if (Source.Buildable) {
		Transform = TransformOperator.Transform(Source.Buildable->GetTransform());
	}

	AFGBuildable* Buildable = nullptr;

	if (Source.Buildable) {
		Buildable = BuildableSubsystem->BeginSpawnBuildable(Source.Buildable->GetClass(), Transform);
	}
	TSubclassOf<UFGRecipe> Recipe;
	if (Source.Buildable) {
		Recipe = SplineHologramFactory->GetRecipeFromClass(Source.Buildable->GetClass());
		if (!Recipe) Recipe = Source.Buildable->GetBuiltWithRecipe();
	}
	if (!Recipe) return nullptr;

	Buildable->SetBuiltWithRecipe(Recipe);
	//TODO:
	//Buildable->SetBuildingID(Source->GetBuildingID());

	AFGBuildablePipelineSupport* SourcePipelineSupport = nullptr;

	if (Source.Buildable) {
		SourcePipelineSupport = Cast<AFGBuildablePipelineSupport>(Source.Buildable);
	}

	AFGBuildablePipelineSupport* TargetPipelineSupport = Cast<AFGBuildablePipelineSupport>(Buildable);

	TargetPipelineSupport->SetSupportLength(SourcePipelineSupport->mLength);
	TargetPipelineSupport->SetVerticalAngle(SourcePipelineSupport->mVerticalAngle);

	if (Source.Buildable) {
		Buildable->SetCustomizationData_Implementation(Source.Buildable->GetCustomizationData_Implementation());
	}
	Buildable->FinishSpawning(Transform);

	return Buildable;
}

FSBuildableType UFSPipelineSupportOperator::GetType() const
{
	return FSBuildableType::Building;
}