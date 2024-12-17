// ILikeBanas


#include "FactorySkyline/UI/FSCopyPanel.h"
#include "FactorySkyline/FSkyline.h"

#include "Buildables/FGBuildableBlueprintDesigner.h"


UFSCopyPanel::UFSCopyPanel(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
}

void UFSCopyPanel::onCheckBoxChanged(bool IsChecked)
{
	if (IsChecked) this->Expand();
	else this->Fold();
}

void UFSCopyPanel::onFullPreviewChanged(bool IsChecked)
{
	AFSkyline* Skyline = AFSkyline::Get(this);
	Skyline->FSCtrl->FullPreview = IsChecked;
}

void UFSCopyPanel::onSetItem()
{
	AFSkyline* Skyline = AFSkyline::Get(this);
	Skyline->FSCtrl->StartSetAnchorMode();
}

void UFSCopyPanel::onStartCopy()
{
	AFSkyline* Skyline = AFSkyline::Get(this);
	if (Skyline->FSCtrl->CheckAnchor(true)) {
		Skyline->FSCtrl->StartCopyMode();
	}
}

void UFSCopyPanel::CopyToBpDesigner()
{
	return;
	AFSkyline* Skyline = AFSkyline::Get(this);

	FHitResult Hit = Skyline->FSCtrl->GetCopyHitResultGeneric();
	AFGBuildable* Building = Skyline->FSCtrl->AcquireBuildable(Hit);

	/*
	if (Building) {
		if (Cast<AFGBuildableBlueprintDesigner>(Building)) {

			AFGBuildableBlueprintDesigner* Designer = Cast<AFGBuildableBlueprintDesigner>(Building);

			
			//Designer->mBuildables = Skyline->FSCtrl->Design->BuildableSet;

			AFGBuildableSubsystem* BuildableSubsystem = AFGBuildableSubsystem::Get(this);

			for (TWeakObjectPtr<AFGBuildable> BuildablePtr : Skyline->FSCtrl->Design->BuildableSet) {
				if (BuildablePtr.Get()) {
					AFGBuildable* Buildable = BuildablePtr.Get();

					Designer->mBuildables.Add(Buildable);

					//BuildableSubsystem->RemoveBuildable(Buildable);

					//BuildableSubsystem->OnDesignerBuildableSpawned(Buildable);


					
					// crashes engine  dont use
					//BuildableSubsystem->RemoveBuildable(Buildable);

					//BuildableSubsystem->RemoveFromTickGroup(Buildable);

					//Buildable->SetActorTickEnabled(false);

					//Designer->OnBuildableConstructedInsideDesigner(Buildable);

				}
			}
			

		}
	}
	*/


	//std::this_thread::sleep_for(std::chrono::nanoseconds(10000000));
	//FTransform Transform = TransformOperator.Transform(Source->GetTransform());
	//FTransform Transform = FTransform ();

	AFGBuildable* Buildable;

	AFGBuildableSubsystem* BuildableSubsystem = AFGBuildableSubsystem::Get(this);

	if (Skyline->FSCtrl->Design->Anchor != FSBuildable()) {


		//FTransform Transform(FRotator(0.0f, 0.0f, 0.0f), FVector(0.0f, 0.0f, 0.0f));
		FTransform Transform;

		if (Skyline->FSCtrl->Design->Anchor != FSBuildable()) {

			
			if (Skyline->FSCtrl->Design->Anchor.Buildable) {
				//FTransform Transform(FRotator(0.0f, 0.0f, 0.0f), Skyline->FSCtrl->Design->Anchor.Buildable->GetTransform().GetTranslation());
				Transform = Skyline->FSCtrl->Design->Anchor.Buildable->GetTransform();
			}
			else {
				//FTransform Transform(FRotator(0.0f, 0.0f, 0.0f), Skyline->FSCtrl->Design->Anchor.Transform.GetTranslation());
				Transform = Skyline->FSCtrl->Design->Anchor.Transform;
			}
		}

		//UPrimitiveComponent* MainComponent;

		if (Skyline->FSCtrl->Design->Anchor.Buildable) {
			//UPrimitiveComponent* MainComponent = Cast<UPrimitiveComponent>(Skyline->FSCtrl->Design->Anchor.Buildable->GetRootComponent());

				// Get the bounds of the component in the world

			/*
			FVector Origin, BoxExtent;
			Skyline->FSCtrl->Design->Anchor.Buildable->GetActorBounds(false, Origin, BoxExtent);

			// Calculate the height (twice the Z extent of the bounding box)
			float Height = BoxExtent.Z * 2;

			// Adjust your transform as needed
				//FTransform NewTransform = Transform;

			// Example: Move the transform up by half the actor's height
			FVector AdjustedLocation = Transform.GetLocation();
			AdjustedLocation.Z += Height*4;
			Transform.SetLocation(AdjustedLocation);
			*/

			/*
			if (UPrimitiveComponent* PrimComp = Skyline->FSCtrl->Design->Anchor.Buildable->FindComponentByClass<UPrimitiveComponent>())
			{
				FBoxSphereBounds Bounds = PrimComp->Bounds;

				// Use the BoxExtent directly to get the height
				float ActorHeight = Bounds.BoxExtent.Z * 2;

				// Example: Adjust transform based on the height
				//FTransform NewTransform = Skyline->FSCtrl->Design->Anchor.Buildable->GetActorTransform();
				FVector AdjustedLocation = Transform.GetLocation();
				AdjustedLocation.Z += ActorHeight * 0.5f;  // Move up by half the height
				//Transform.SetLocation(AdjustedLocation);

				//MyBuildableActor->SetActorTransform(NewTransform);
			}

			if (Skyline->FSCtrl->Design->Anchor.Buildable->mInstanceHandles.Num() > 0) {

				TArray<FInstanceHandle*> InstanceHandles = Skyline->FSCtrl->Design->Anchor.Buildable->mInstanceHandles;

				for (int i = 0; i < InstanceHandles.Num(); i++) {
					UHierarchicalInstancedStaticMeshComponent* component = InstanceHandles[i]->GetInstanceComponent();

					FBoxSphereBounds Bounds = component->Bounds;


					// Use the BoxExtent directly to get the height
					float ActorHeight = (Bounds.Origin.Z - Bounds.BoxExtent.Z);

					// Example: Adjust transform based on the height
					//FTransform NewTransform = Skyline->FSCtrl->Design->Anchor.Buildable->GetActorTransform();
					FVector AdjustedLocation = Transform.GetLocation();
					AdjustedLocation.Z += ActorHeight/22;  // Move up by half the height
					Transform.SetLocation(AdjustedLocation);

				}
			}
			*/
		}

		TSubclassOf< UFGRecipe > ConveyorPoleRecipe = LoadClass<UFGRecipe>(this, TEXT("/Game/FactoryGame/Recipes/Buildings/Recipe_BlueprintDesigner.Recipe_BlueprintDesigner_C"));
		//try {
		Buildable = BuildableSubsystem->BeginSpawnBuildable(AFGBuildable::GetBuildableClassFromRecipe(ConveyorPoleRecipe), Transform);

		AFGBuildableBlueprintDesigner* Designer = Cast<AFGBuildableBlueprintDesigner>(Buildable);

		FBlueprintRecord newRecord = FBlueprintRecord();

		FString Value = VectorQ->GetText().ToString();

		newRecord.BlueprintName = Value;


		TArray< class AFGBuildable* > mBuildables;


		//TODO REWORK THIS AS SOME BUILDABLES DONT EXIST AS THE SAME TYPE ANYMORE

		AFGLightweightBuildableSubsystem* lightweightSubsystem;
		lightweightSubsystem = AFGLightweightBuildableSubsystem::Get(Skyline->World);
		
		for (FSBuildable BuildablePtr : Skyline->FSCtrl->Design->BuildableSet) {

			if (BuildablePtr.Buildable) {
				Designer->mBuildables.Add(BuildablePtr.Buildable);
			}
			else {

				FLightweightBuildableInstanceRef buildableRef;
				AFGLightweightBuildableSubsystem::ResolveLightweightInstance(BuildablePtr.Handle, buildableRef);

				//if (TemporaryBuildable == nullptr) {
					//TemporaryBuildable = buildableRef.SpawnTemporaryBuildable();

				AFGBuildable* TemporaryBuildable = UFGLightweightBuildableBlueprintLibrary::SpawnTemporaryFromLightweight(buildableRef);

				Designer->mBuildables.Add(TemporaryBuildable);

				//Designer->OnBuildableConstructedInsideDesigner(TemporaryBuildable);

			}

			/*
			if (BuildablePtr.Get()) {
				AFGBuildable* Buildable = BuildablePtr.Get();

				Designer->mBuildables.Add(Buildable);
			}
			*/

		}
		



		Designer->SaveBlueprint(newRecord, Skyline->FGController);

		// clear saved cache to the designer as we dont need it anymore
		Designer->mBuildables = mBuildables;

		Buildable->FinishSpawning(Transform);


		//Buildable->Destroy();

		lightweightSubsystem->RemoveStaleTemporaryBuildables();

	}
	
}

void UFSCopyPanel::Init()
{
	FScriptDelegate Func1;
	Func1.BindUFunction(this, FName("onCheckBoxChanged"));
	this->FoldPanel->FoldBox->OnCheckStateChanged.Add(Func1);

	Func1.BindUFunction(this, FName("onFullPreviewChanged"));
	this->CheckBoxFullPreview->OnCheckStateChanged.Add(Func1);

	Func1.BindUFunction(this, FName("onSetItem"));
	this->SetItemButton->OnClicked.Add(Func1);

	Func1.BindUFunction(this, FName("onStartCopy"));
	this->StartCopyButton->OnClicked.Add(Func1);

	Func1.BindUFunction(this, FName("CopyToBpDesigner"));
	this->CopyToBpDesignerButton->OnClicked.Add(Func1);

	this->FoldPanel->Title->SetText(FText::FromString(FString(TEXT("Basic Copy"))));
	this->SetItemStatus->SetVisibility(ESlateVisibility::Collapsed);

	this->FoldPanel->FoldBox->SetCheckedState(ECheckBoxState::Unchecked);
	this->onCheckBoxChanged(false);

	this->CheckBoxFullPreview->SetCheckedState(ECheckBoxState::Unchecked);
	this->onFullPreviewChanged(false);
}

void UFSCopyPanel::Load(UFSDesign* Design)
{
	this->Design = Design;
	this->Design->SetItemFeedback = this->SetItemStatus;
	 AFSkyline::Get(this)->FSCtrl->CheckAnchor(false);
}

void UFSCopyPanel::Fold()
{
}

void UFSCopyPanel::Expand()
{
}
