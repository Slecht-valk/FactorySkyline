// ILikeBanas


#include "FSConveyorPoleStackableOperator.h"


AFGHologram* UFSConveyorPoleStackableOperator::HologramCopy(FTransform& RelativeTransform)
{
	//RelativeTransform = Source->GetTransform();

	if (Source.Buildable) {
		RelativeTransform = Source.Buildable->GetTransform();
	}

	return CreateHologram();
}

AFGBuildable* UFSConveyorPoleStackableOperator::CreateCopy(const FSTransformOperator& TransformOperator)
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

	if (Source.Buildable) {
		Buildable->SetCustomizationData_Implementation(Source.Buildable->GetCustomizationData_Implementation());
	}
	Buildable->FinishSpawning(Transform);

	return Buildable;
}

FSBuildableType UFSConveyorPoleStackableOperator::GetType() const
{
	return FSBuildableType::Building;
}

void UFSConveyorPoleStackableOperator::GetSelectConnectList(FSBuildable* Buildable, TArray<TWeakObjectPtr<AFGBuildable>>& List) const
{
	if (Buildable->Buildable) {
		TArray<FOverlapResult> Result;
		FCollisionShape Shape;
		FTransform Transform = Buildable->Buildable->GetTransform();
		FVector Loc = Transform.GetLocation();

		Shape.SetBox(FVector3f(10.0f, 120.0f, 50.0f));
		World->OverlapMultiByChannel(Result, FVector(Loc.X, Loc.Y, Loc.Z + 200.0f), Transform.GetRotation(), ECollisionChannel::ECC_Visibility, Shape);

		for (FOverlapResult& Res : Result) {
			AFGBuildable* Buildable = Cast<AFGBuildable>(Res.GetActor());
			if (Buildable) List.Add(Buildable);
		}
	}
}