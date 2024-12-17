// ILikeBanas


#include "FSLadderOperator.h"
#include "Buildables/FGBuildableLadder.h"
#include "Hologram/FGLadderHologram.h"


AFGHologram* UFSLadderOperator::HologramCopy(FTransform& RelativeTransform)
{
	//RelativeTransform = Source->GetTransform();

	if (Source.Buildable) {
		RelativeTransform = Source.Buildable->GetTransform();
	}

	AFGHologram* Hologram = CreateHologram();
	if (!Hologram) return nullptr;
	AFGLadderHologram* LadderHologram = Cast<AFGLadderHologram>(Hologram);
	if (!LadderHologram) return Hologram;

	AFGBuildableLadder* SourceLadder = nullptr;

	if (Source.Buildable) {
		SourceLadder = Cast<AFGBuildableLadder>(Source.Buildable);
	}
	
	LadderHologram->mTargetSegmentHeight = SourceLadder->mNumSegments;
	for (int i = 1; i < SourceLadder->mNumSegments; i++) {
		LadderHologram->mInstancedMeshComponent->AddInstance(FTransform(FVector(0.f, 1.f, 0.f).ToOrientationQuat(), FVector(0.0f, 0.0f, LadderHologram->mMeshHeight * i)));
	}

	return LadderHologram;
}

AFGBuildable* UFSLadderOperator::CreateCopy(const FSTransformOperator& TransformOperator)
{
	//FTransform Transform = TransformOperator.Transform(Source->GetTransform());

	FTransform Transform;

	if (Source.Buildable) {
		Transform = TransformOperator.Transform(Source.Buildable->GetTransform());
	}

	AFGBuildable* Buildable = nullptr;
	AFGBuildableLadder* SourceLadder = nullptr;
	
	if (Source.Buildable) {
		Buildable = BuildableSubsystem->BeginSpawnBuildable(Source.Buildable->GetClass(), Transform);
		SourceLadder = Cast<AFGBuildableLadder>(Source.Buildable);
	}

	AFGBuildableLadder* TargetLadder = Cast<AFGBuildableLadder>(Buildable);
	
	TSubclassOf<UFGRecipe> Recipe;
	if (Source.Buildable) {
		Recipe = SplineHologramFactory->GetRecipeFromClass(Source.Buildable->GetClass());
		if (!Recipe) Recipe = Source.Buildable->GetBuiltWithRecipe();
	}
	if (!Recipe) return nullptr;

	Buildable->SetBuiltWithRecipe(Recipe);
	//TODO:
	//Buildable->SetBuildingID(Source->GetBuildingID());
	TargetLadder->SetNumSegments(SourceLadder->mNumSegments);

	if (Source.Buildable) {
		Buildable->SetCustomizationData_Implementation(Source.Buildable->GetCustomizationData_Implementation());
	}
	Buildable->FinishSpawning(Transform);

	return Buildable;
}

void UFSLadderOperator::GetSelectConnectList(FSBuildable* Buildable, TArray<TWeakObjectPtr<AFGBuildable>>& List) const
{
	if (Buildable->Buildable) {
		AFGBuildableLadder* Ladder = Cast<AFGBuildableLadder>(Buildable->Buildable);
		if (Ladder) {
			TArray<FOverlapResult> Result;
			FCollisionShape Shape;
			FTransform Transform = Ladder->GetTransform();
			FVector Loc = Transform.GetLocation();
			Loc.Z += Ladder->mMeshHeight * Ladder->mNumSegments / 2.0;
			Shape.SetBox(FVector3f(10.0f, 10.0f, Ladder->mMeshHeight * Ladder->mNumSegments / 2.0 + 50.0f));
			World->OverlapMultiByChannel(Result, Loc, Transform.GetRotation(), ECollisionChannel::ECC_Visibility, Shape);

			for (FOverlapResult& Res : Result) {
				AFGBuildable* Buildable = Cast<AFGBuildable>(Res.GetActor());
				if (Buildable) List.Add(Buildable);
			}
		}
	}
}

