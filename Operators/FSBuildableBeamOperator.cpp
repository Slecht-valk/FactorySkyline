// ILikeBanas


#include "FSBuildableBeamOperator.h"
#include "Buildables/FGBuildable.h"
#include "Buildables/FGBuildableConveyorLift.h"
#include "FGBuildableBeam.h"
#include "FGBeamHologram.h"
#include "Hologram/FGConveyorLiftHologram.h"
#include "FactorySkyline/FSkyline.h"
/*
void UFSBuildableBeamOperator::UpdateHologramState(const FHitResult& Hit, AFGHologram* Hologram, bool& ShouldShow, bool& Valid)
{
	
	AFGConveyorLiftHologram* LiftHologram = Cast<AFGConveyorLiftHologram>(Hologram);

	ShouldShow = Valid = false;
	if (!Hologram->IsValidHitResult(Hit)) return;

	Hologram->SetHologramLocationAndRotation(Hit);

	AActor* Actor = Hit.GetActor();
	this->HitConnection = nullptr;

	if (!Actor) return;

	FVector HologramLocation = Hologram->GetTransform().GetLocation();

	for (UActorComponent* Connection : Actor->GetComponentsByClass(UFGFactoryConnectionComponent::StaticClass())) {
		UFGFactoryConnectionComponent* FactoryConnection = Cast<UFGFactoryConnectionComponent>(Connection);
		if (FactoryConnection) {
			if (HitConnection == nullptr) HitConnection = FactoryConnection;
			else {
				FVector HitLocation = HitConnection->GetComponentLocation();
				FVector FactoryLocation = FactoryConnection->GetComponentLocation();
				if ((FactoryLocation - HologramLocation).SizeSquared() < (HitLocation - HologramLocation).SizeSquared())
					HitConnection = FactoryConnection;
			}
		}
	}
	
	ShouldShow = Valid = true;
}
*/

AFGHologram* UFSBuildableBeamOperator::HologramCopy(FTransform& RelativeTransform)
{
	//return SplineHologramFactory->CreateLiftHologram(Cast<AFGBuildableConveyorLift>(Source), RelativeTransform);
	RelativeTransform = Source->GetTransform();
	AFGHologram* Hologram = CreateHologram();
	if (!Hologram) return nullptr;

	AFGBuildableBeam* SourceBuildableBeam = Cast<AFGBuildableBeam>(Source);
	AFGBeamHologram* BuildableBeamHologram = Cast<AFGBeamHologram>(Hologram);

	if (!BuildableBeamHologram) return Hologram;

	BuildableBeamHologram->mSize = SourceBuildableBeam->mSize;
	BuildableBeamHologram->mDefaultLength = SourceBuildableBeam->mDefaultLength;
	BuildableBeamHologram->mMaxLength = SourceBuildableBeam->mMaxLength;
	BuildableBeamHologram->mCurrentLength = SourceBuildableBeam->mLength;
	//BuildableBeamHologram->SetCurrentLength(SourceBuildableBeam->mLength);
	BuildableBeamHologram->OnRep_CurrentLength();
	return Hologram;
}

AFGBuildable* UFSBuildableBeamOperator::CreateCopy(const FSTransformOperator& TransformOperator)
{
	AFSkyline* FSkyline = AFSkyline::Get(this);

	// linear translate, doesnt take into account rotation transformation
	/*
	// the original anchor point
	AFGBuildable* BuildableAnchor = FSkyline->Builder->Design->Anchor.Get();
	FTransform AnchorTransform = BuildableAnchor->GetTransform();
	// where the original anchor point is
	FVector AnchorPoint = AnchorTransform.GetTranslation();

	FTransform SourceTransform = Source->GetTransform();
	// where the original beam point is
	FVector SourcePoint = SourceTransform.GetTranslation();
	// the 3 distance dimensions of the beam from the point of the original anchor point
	float xOffset = SourcePoint.X - AnchorPoint.X;
	float yOffset = SourcePoint.Y - AnchorPoint.Y;
	float zOffset = SourcePoint.Z - AnchorPoint.Z;


	FTransform HologramTransform = FSkyline->Builder->Hologram->GetTransform();
	FVector HologramPoint = HologramTransform.GetTranslation();
	float X = HologramPoint.X + xOffset;
	float Y = HologramPoint.Y + yOffset;
	float Z = HologramPoint.Z + zOffset;

	FVector newPoint = FVector(X, Y, Z);
	*/

	FTransform Transform = TransformOperator.Transform(Source->GetTransform());

	//Transform.CopyRotation(Source->GetTransform());
	//Transform.SetTranslation(newPoint);
	

	//Transform.CopyRotation(Source->GetTransform());
	//Transform.CopyTranslation(Source->GetTransform());

	AFGBuildable* Buildable = BuildableSubsystem->BeginSpawnBuildable(Source->GetClass(), Transform);
	AFGBuildableBeam* SourceBuildableBeam = Cast<AFGBuildableBeam>(Source);
	AFGBuildableBeam* BuildableBeam = Cast<AFGBuildableBeam>(Buildable);


	//float num = SourceBuildableBeam->mSize;
	BuildableBeam->mSize = SourceBuildableBeam->mSize;
	BuildableBeam->mDefaultLength = SourceBuildableBeam->mDefaultLength;
	BuildableBeam->mMaxLength = SourceBuildableBeam->mMaxLength;
	BuildableBeam->mLength = SourceBuildableBeam->mLength;

	//AFGBuildableConveyorLift* SourceConveyorLift = Cast<AFGBuildableConveyorLift>(Source);
	//AFGBuildableConveyorLift* TargetConveyorLift = Cast<AFGBuildableConveyorLift>(Buildable);

	TSubclassOf<UFGRecipe> Recipe = SplineHologramFactory->GetRecipeFromClass(Source->GetClass());
	if (!Recipe) Recipe = Source->GetBuiltWithRecipe();
	if (!Recipe) return nullptr;

	Buildable->SetBuiltWithRecipe(Recipe);
	//Buildable->SetBuildingID(Source->GetBuildingID());
	/*
	TargetConveyorLift->mTopTransform = SourceConveyorLift->mTopTransform;
	TargetConveyorLift->mIsReversed = SourceConveyorLift->mIsReversed;
	*/
	//FSkyline->AdaptiveUtil->CopyConveyorLiftAttribute(SourceConveyorLift, TargetConveyorLift);

	Buildable->SetCustomizationData_Implementation(Source->GetCustomizationData_Implementation());
	Buildable->FinishSpawning(Transform);

	//FSkyline->Select->Select(Buildable);
	//FSkyline->Select->Select(Buildable);
	FSBuildable Buildableptr;
	Buildableptr.Buildable = Buildable;
	FSkyline->Select->EnableHightLight(Buildableptr, FSkyline->Select->SelectMaterial);
	FSkyline->Select->DisableHightLight(Buildableptr);
	// none of these methods refresh the meshes
	/*
	// hack to refresh static mesh - This is needed for power indicators and meshes to update properly.
	auto OutlineComp = UFGOutlineComponent::Get(FSkyline->FSCtrl->World);
	OutlineComp->ShowDismantlePendingMaterial(Buildable);
	OutlineComp->HideAllDismantlePendingMaterial();

	//protected method
	OutlineComp->ShowDismantlePendingMaterialOnActor(Buildable);
	OutlineComp->HideDismantlePendingMaterialOnActor(Buildable);

	OutlineComp->ShowOutline(Buildable, EOutlineColor::OC_HOLOGRAM);
	OutlineComp->HideOutline();

	// protected method
	OutlineComp->ShowActorOutline(Buildable, EOutlineColor::OC_HOLOGRAM);
	OutlineComp->HideActorOutline(Buildable);
	*/
	/*
	UFGFactoryLegsComponent* Legs = Buildable->FindComponentByClass<UFGFactoryLegsComponent>();
	if (Legs) {
		TArray< FFeetOffset > feetOffset = Legs->TraceFeetOffsets(Transform, Buildable);
		Legs->SetFeetOffsets(feetOffset);
		Legs->RecreateLegs();
	}
	*/

	//this->BuildableSubsystem->RemoveConveyorFromBucket(TargetConveyorLift);

	return Buildable;
}
