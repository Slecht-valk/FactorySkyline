// ILikeBanas


#include "FSPumpOperator.h"
#include "Buildables/FGBuildable.h"
#include "Buildables/FGBuildablePipelinePump.h"

AFGBuildable* UFSPumpOperator::CreateCopy(const FSTransformOperator& TransformOperator)
{
	//FTransform Transform = TransformOperator.Transform(Source->GetTransform());

	FTransform Transform;

	if (Source.Buildable) {
		Transform = TransformOperator.Transform(Source.Buildable->GetTransform());
	}

	AFGBuildable* Buildable = nullptr;
	AFGBuildablePipelinePump* SourcePump = nullptr;

	if (Source.Buildable) {
		Buildable = BuildableSubsystem->BeginSpawnBuildable(Source.Buildable->GetClass(), Transform);
		SourcePump = Cast<AFGBuildablePipelinePump>(Source.Buildable);
	}

	AFGBuildablePipelinePump* TargetPump = Cast<AFGBuildablePipelinePump>(Buildable);
	TSubclassOf<UFGRecipe> Recipe;
	if (Source.Buildable) {
		Recipe = SplineHologramFactory->GetRecipeFromClass(Source.Buildable->GetClass());
		if (!Recipe) Recipe = Source.Buildable->GetBuiltWithRecipe();
	}
	if (!Recipe) return nullptr;

	Buildable->SetBuiltWithRecipe(Recipe);
	//TODO:
	//Buildable->SetBuildingID(Source->GetBuildingID());

	if (Source.Buildable) {
		Buildable->SetCustomizationData_Implementation(Source.Buildable->GetCustomizationData_Implementation());
	}
	Buildable->FinishSpawning(Transform);

	TargetPump->mDefaultFlowLimit = SourcePump->mDefaultFlowLimit;
	TargetPump->SetUserFlowLimit(SourcePump->GetFlowLimit());

	TargetPump->mIndicatorFlowPct = SourcePump->mIndicatorFlowPct;
	TargetPump->mIndicatorPressurePct = SourcePump->mIndicatorPressurePct;

	return Buildable;
}

