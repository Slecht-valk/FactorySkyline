// ILikeBanas


#include "FSResourceExtractorOperator.h"
#include "FGWaterVolume.h"
#include "Resources/FGResourceNode.h"
#include "Buildables/FGBuildableResourceExtractor.h"


AFGBuildable* UFSResourceExtractorOperator::CreateCopy(const FSTransformOperator& TransformOperator)
{
	AFGBuildableResourceExtractor* SourceExtractor = nullptr;

	if (Source.Buildable) {
		SourceExtractor = Cast<AFGBuildableResourceExtractor>(Source.Buildable);
	}
	
	/*
	if (Cast<AFGWaterVolume>(SourceExtractor->mExtractableResource)) {
		FTransform Transform = TransformOperator.Transform(Source->GetTransform());

		TArray<FOverlapResult> Result;
		FCollisionShape Shape;
		Shape.SetSphere(200.0f);
		
		World->OverlapMultiByChannel(Result, Transform.GetLocation(), Transform.GetRotation(), ECollisionChannel::ECC_Visibility, Shape);

		AFGWaterVolume* Resource = nullptr;
		//SML::Logging::info(Result.Num());
		for (FOverlapResult& Res : Result) {
			Resource = Cast<AFGWaterVolume>(Res.GetActor());
			if (Resource) break;
		}

		if (Resource) {
			AFGBuildable* Buildable = BuildableSubsystem->BeginSpawnBuildable(Source->GetClass(), Transform);
			AFGBuildableResourceExtractor* TargetExtractor = Cast<AFGBuildableResourceExtractor>(Buildable);

			TSubclassOf<UFGRecipe> Recipe = SplineHologramFactory->GetRecipeFromClass(Source->GetClass());
			if (!Recipe) Recipe = Source->GetBuiltWithRecipe();
			if (!Recipe) return nullptr;

			Buildable->SetBuiltWithRecipe(Recipe);
			//TODO:
			//Buildable->SetBuildingID(Source->GetBuildingID());

			TargetExtractor->SetExtractableResource(Resource);

			Buildable->SetCustomizationData_Implementation(Source->GetCustomizationData_Implementation());
			Buildable->FinishSpawning(Transform);

			return Buildable;
		}
	}
	*/


	//if (Cast<AFGResourceNode>(SourceExtractor->mExtractableResource)) {
 		//FTransform Transform = TransformOperator.Transform(Source->GetTransform());

		FTransform Transform;

		if (Source.Buildable) {
			Transform = TransformOperator.Transform(Source.Buildable->GetTransform());
		}

		/*
		TArray<FOverlapResult> Result;
		FCollisionShape Shape;
		Shape.SetSphere(200.0f);

		World->OverlapMultiByChannel(Result, Transform.GetLocation(), Transform.GetRotation(), ECollisionChannel::ECC_Visibility, Shape);

		AFGResourceNode* Resource = nullptr;
		//SML::Logging::info(Result.Num());
		for (FOverlapResult& Res : Result) {
			Resource = Cast<AFGResourceNode>(Res.GetActor());
			if (Resource) break;
		}
		*/

		//AFGResourceNode* Resource = Cast<AFGResourceNode>(SourceExtractor->mExtractableResource);

		//if (Resource) {
		AFGBuildable* Buildable = nullptr;

			if (Source.Buildable) {
				Buildable = BuildableSubsystem->BeginSpawnBuildable(Source.Buildable->GetClass(), Transform);
			}

			AFGBuildableResourceExtractor* TargetExtractor = Cast<AFGBuildableResourceExtractor>(Buildable);

			TSubclassOf<UFGRecipe> Recipe;

			if (Source.Buildable) {
				Recipe = SplineHologramFactory->GetRecipeFromClass(Source.Buildable->GetClass());
				if (!Recipe) Recipe = Source.Buildable->GetBuiltWithRecipe();
			}
			if (!Recipe) return nullptr;

			Buildable->SetBuiltWithRecipe(Recipe);
			//TODO:
			//Buildable->SetBuildingID(Source->GetBuildingID());

			//TargetExtractor->SetExtractableResource(Resource);
			//TargetExtractor->SetResourceNode(Resource);
			TargetExtractor->mExtractableResource = SourceExtractor->mExtractableResource;

			if (Source.Buildable) {
				Buildable->SetCustomizationData_Implementation(Source.Buildable->GetCustomizationData_Implementation());
			}
			Buildable->FinishSpawning(Transform);

			return Buildable;
		//}
	//}

	//return nullptr;
}