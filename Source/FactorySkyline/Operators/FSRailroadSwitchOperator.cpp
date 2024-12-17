// ILikeBanas


#include "FSRailroadSwitchOperator.h"
#include "Buildables/FGBuildableRailroadSwitchControl.h"

/*
AFGHologram* UFSRailroadSwitchOperator::CreateHologram()
{
	//return nullptr;
}

void UFSRailroadSwitchOperator::UpdateHologramState(const FHitResult& Hit, AFGHologram* Hologram, bool& ShouldShow, bool& Valid)
{

}

AFGHologram* UFSRailroadSwitchOperator::HologramCopy(FTransform& RelativeTransform)
{
	return nullptr;
}
*/

AFGBuildable* UFSRailroadSwitchOperator::CreateCopy(const FSTransformOperator& TransformOperator)
{

	//std::this_thread::sleep_for(std::chrono::nanoseconds(10000000));

	//FTransform Transform = TransformOperator.Transform(Source->GetTransform());

	FTransform Transform;

	if (Source.Buildable) {
		Transform = TransformOperator.Transform(Source.Buildable->GetTransform());
	}
	else {
		Transform = TransformOperator.Transform(Source.Transform);

		//RelativeVector = TransformOperator.SourceTransform.InverseTransformPositionNoScale(Source.Transform.GetLocation());
		//RelativeRotation = TransformOperator.SourceTransform.InverseTransformRotation(Source.Transform.GetRotation());

		//FTransform NewTransform(TransformOperator.TargetTransform.TransformRotation(RelativeRotation),
			//TransformOperator.TargetTransform.TransformPositionNoScale(RelativeVector), Source.Transform.GetScale3D());

		//Transform = NewTransform;
	}

	if (TemporaryBuildable) {
		Transform = Source.Transform;
	}

	//FTransform Transform = FTransform ();

	//sourceClass = Source->GetClass();

	if (Source.Buildable) {
		sourceClass = Source.Buildable->GetClass();
	}
	else {
		sourceClass = Source.BuildableClass;
	}

	Buildable = nullptr;

	//TSubclassOf< UFGRecipe > ConveyorPoleRecipe = LoadClass<UFGRecipe>(this, TEXT("/Game/FactoryGame/Recipes/Buildings/Foundations/Recipe_Foundation_8x1_01.Recipe_Foundation_8x1_01_C"));
	//try {
	if (Source.Buildable) {
		Buildable = BuildableSubsystem->BeginSpawnBuildable(sourceClass, Transform);
	}
	else {
		Buildable = BuildableSubsystem->BeginSpawnBuildable(sourceClass, Transform);

		//Buildable->TogglePendingDismantleMaterial(true);
		//Buildable->SetIsPendingDismantleRemoval(true);
		//Buildable->mIsLightweightTemporary = 1;
		//TArray< FInstanceHandle* > arr;
		//Buildable->SetIsLightweightTemporary(arr, 10000);
		//AFSkyline* FSkylin = AFSkyline::Get(this);

		//AFGBuildableSubsystem* sub = AFGBuildableSubsystem::Get(FSkylin->World);
		//sub->OnDesignerBuildableSpawned(Buildable);
	}
	//Buildable->FinishSpawning(Transform);

	//if (Buildable == nullptr) return nullptr;
	//fgcheck(Buildable);
	//}
	//catch (exception& e) {
		//cout << "Standard exception: " << e.what() << endl;
	//}

	//if (sourceClass->ClassGeneratedBy == nullptr) {
		//return nullptr;
	//}

	//TSubclassOf<UFGRecipe> Recipe = SplineHologramFactory->GetRecipeFromClass(sourceClass);
	//if (!Recipe) Recipe = Source->GetBuiltWithRecipe();
	//if (!Recipe) return nullptr;

	//Buildable->SetBuiltWithRecipe(Recipe);
	//TODO:
	//Buildable->SetBuildingID(Source->GetBuildingID());

	//std::string  str = TCHAR_TO_UTF8(*Buildable->GetName());
	//str.append("\n");
	//std::wstring temp = std::wstring(str.begin(), str.end());
	//LPCWSTR wideString = temp.c_str();
	//OutputDebugStringW(wideString);
	//FFactoryCustomizationData data = Source->GetCustomizationData_Implementation();
	/*
	FFactoryCustomizationData data = FFactoryCustomizationData();
	FFactoryCustomizationData data2 = Source->mCustomizationData;
	Buildable->mHologramClass = Source->mHologramClass;
	Buildable->mDisplayName = Source->mDisplayName;
	Buildable->mDescription = Source->mDescription;
	Buildable->MaxRenderDistance = Source->MaxRenderDistance;
	Buildable->mHighlightVector = Source->mHighlightVector;
	Buildable->mDecoratorClass = Source->mDecoratorClass;
	Buildable->mDefaultSwatchCustomizationOverride = Source->mDefaultSwatchCustomizationOverride;
	//Buildable->SetCustomizationData_Implementation(Source->mCustomizationData);
	//Buildable->SetCustomizationData_Native(Source->mCustomizationData);
	Buildable->SetCustomizationData_Implementation(data2);
	Buildable->mCustomizationData.ColorSlot = Source->mColorSlot;
	TSubclassOf< UFGFactoryCustomizationDescriptor_Swatch > SwatchDesc = TSubclassOf< UFGFactoryCustomizationDescriptor_Swatch >();
	data.SwatchDesc = SwatchDesc;
	Buildable->OnRep_CustomizationData();
	*/
	//TSubclassOf< UFGFactoryCustomizationDescriptor_Skin > skin1 = Buildable->GetActiveSkin();
	//TSubclassOf< UFGFactoryCustomizationDescriptor_Skin > skin2 = Buildable->GetActiveSkin();
	//auto skin1 = Source->GetActiveSkin_Native();
	//auto skin2 = Buildable->GetActiveSkin_Native();
	//skin2.SwatchDesc = skin1.SwatchDesc;
	//TSubclassOf< class UFGFactoryCustomizationDescriptor_Swatch > swatch = Source->swatch;
	//Buildable->ApplySkinData(skinImp);
	//Buildable->OnSkinCustomizationApplied(skinImp);
	//Buildable->OverrideColorData =
	// doesn't appear to save any color slot data in any scenario
	//Buildable->mCustomizationData.ColorSlot = Source->mColorSlot;

	//Source->GetCustomizationData();
	// crashes game
	//Buildable->SetCustomizationData(Source->GetCustomizationData());
	// crashes game
	//Buildable->ApplyCustomizationData(Source->GetCustomizationData());
	//Buildable->ApplyCustomizationData_Native(Source->GetCustomizationData());

	//Buildable->FlagReevaluateMaterialOnColored();
	//Source->OnRep_CustomizationData();
	//Buildable->OnRep_CustomizationData();
	//Buildable->OnSkinCustomizationApplied(Source->GetActiveSkin_Implementation());

	//TSubclassOf< UFGFactoryCustomizationDescriptor_Skin > skinImp = Source->UFGFactoryCustomizationDescriptor_Skin();
	//Buildable->ApplySkinData(skinImp);
	//Buildable->OnSkinCustomizationApplied(skinImp);

	/*
	AFSkyline* FSkyline = AFSkyline::Get(this);
	// hack to refresh static mesh - This is needed for power indicators and meshes to update properly.
	auto OutlineComp = UFGOutlineComponent::Get(FSkyline->FSCtrl->World);
	OutlineComp->ShowDismantlePendingMaterial(Buildable);
	OutlineComp->HideAllDismantlePendingMaterial();
	*/


	//Buildable->OnSkinCustomizationApplied(Source->TSubclassOf< class UFGFactoryCustomizationDescriptor_Skin >());

	//Buildable->mColorSlot = Source->mColorSlot;
	//uint8 color = Buildable->mColorSlot;


	AFGBuildableRailroadSwitchControl* SourceSwitch = Cast<AFGBuildableRailroadSwitchControl>(Source.Buildable);
	AFGBuildableRailroadSwitchControl* Switch = Cast<AFGBuildableRailroadSwitchControl>(Buildable);

	Connection0->SetSwitchControl(Switch);
	Switch->SetControlledConnection(Connection0);
	Connection0->mSwitchPosition = SourceSwitch->mSwitchData.Position;
	Switch->mSwitchData.Position = SourceSwitch->mSwitchData.Position;
	Switch->OnRep_ControlledConnection();
	Switch->OnRep_VisualState();
	Switch->UpdateVisuals();

	TSubclassOf<UFGRecipe> Recipe;
	//Recipe = SplineHologramFactory->GetRecipeFromClass(Source->GetClass());

	if (Source.Buildable) {
		sourceClass = Source.Buildable->GetClass();
		Recipe = SplineHologramFactory->GetRecipeFromClass(Source.Buildable->GetClass());
	}
	else {
		Recipe = SplineHologramFactory->GetRecipeFromClass(Source.BuildableClass);
	}


	if (!Recipe) {
		if (Source.Buildable) {
			Recipe = Source.Buildable->GetBuiltWithRecipe();
		}
	}
	if (!Recipe) return nullptr;
	Buildable->SetBuiltWithRecipe(Recipe);

	if (Source.Buildable) {
		Buildable->SetCustomizationData_Implementation(Source.Buildable->GetCustomizationData_Implementation());

		/*
		AFSkyline* FSkyline = AFSkyline::Get(this);

		FActorSpawnParameters* parameters = new FActorSpawnParameters();

		(AMyActor*)FSkyline->FSCtrl->World->SpawnActorAbsolute(AMyActor::StaticClass(), Transform, *parameters);
		*/


	}

	if (!Source.Buildable) {
		Buildable->SetCustomizationData_Implementation(Source.RuntimeData.CustomizationData);
	}

	if (TemporaryBuildable) {
		//Buildable->mIsLightweightTemporary = 1;
		//Buildable->mIsStaleLightweightTemporary = 1;
	}

	//Buildable->mManagedByLightweightBuildableSubsystem = 0;

	Buildable->FinishSpawning(Transform);

	//AFSkyline* FSkylin = AFSkyline::Get(this);

	//AFGBuildableSubsystem* sub = AFGBuildableSubsystem::Get(FSkylin->World);
	//sub->OnDesignerBuildableSpawned(Buildable);

	UFGFactoryLegsComponent* Legs = Buildable->FindComponentByClass<UFGFactoryLegsComponent>();
	if (Legs) {
		TArray< FFeetOffset > feetOffset = Legs->TraceFeetOffsets(Transform, Buildable);
		Legs->SetFeetOffsets(feetOffset);

		// TODO DO WE NEED A ALTERNATIVE TO THIS?
		//Legs->RecreateLegs();
	}

	/*
	SML::Logging::info(TEXT("UFSBuildableOperator::CreateCopy"));
	SML::Logging::info(*Source->GetFullName());
	for (UActorComponent* Component : Source->GetComponents()) {
		SML::Logging::info(*Component->GetFullName());
		UMeshComponent* MeshComponent = Cast<UMeshComponent>(Component);
		if (MeshComponent) {
			for (UMaterialInterface* MaterialInterface : MeshComponent->GetMaterials())
				SML::Logging::info(*MaterialInterface->GetFullName());
		}
	}*/
	//this->firstBuild = true;

	return Buildable;

}