// ILikeBanas


#include "FSBuildableLightsControlPanelOperator.h"
#include "Buildables/FGBuildable.h"
#include "Buildables/FGBuildableConveyorLift.h"
#include "Buildables/FGBuildableLightsControlPanel.h"
#include "Hologram/FGConveyorLiftHologram.h"
#include "FactorySkyline/FSkyline.h"
#include "Buildables/FGBuildableLightSource.h"
/*
void UFSBuildableSignOperator::UpdateHologramState(const FHitResult& Hit, AFGHologram* Hologram, bool& ShouldShow, bool& Valid)
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
/*
AFGHologram* UFSBuildableSignOperator::HologramCopy(FTransform& RelativeTransform)
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
*/

AFGBuildable* UFSBuildableLightsControlPanelOperator::CreateCopy(const FSTransformOperator& TransformOperator)
{
	AFSkyline* FSkyline = AFSkyline::Get(this);

	FTransform Transform = TransformOperator.Transform(Source->GetTransform());

	AFGBuildable* Buildable = BuildableSubsystem->BeginSpawnBuildable(Source->GetClass(), Transform);
	AFGBuildableLightsControlPanel* SourceBuildableLightsControlPanel = Cast<AFGBuildableLightsControlPanel>(Source);
	AFGBuildableLightsControlPanel* BuildableLightsControlPanel = Cast<AFGBuildableLightsControlPanel>(Buildable);

	/*
	BuildableSign->mSignTypeDescriptor = SourceBuildableSign->mSignTypeDescriptor;
	BuildableSign->mGainSignificanceDistance = SourceBuildableSign->mGainSignificanceDistance;
	BuildableSign->mTextElementToDataMap = SourceBuildableSign->mTextElementToDataMap;
	BuildableSign->mIconElementToDataMap = SourceBuildableSign->mIconElementToDataMap;
	BuildableSign->mWidgetMaterial = SourceBuildableSign->mWidgetMaterial;
	BuildableSign->mActivePrefabLayout = SourceBuildableSign->mActivePrefabLayout;
	BuildableSign->mPrefabTextElementSaveData = SourceBuildableSign->mPrefabTextElementSaveData;
	BuildableSign->mPrefabIconElementSaveData = SourceBuildableSign->mPrefabIconElementSaveData;
	BuildableSign->mForegroundColor = SourceBuildableSign->mForegroundColor;
	BuildableSign->mBackgroundColor = SourceBuildableSign->mBackgroundColor;
	BuildableSign->mAuxilaryColor = SourceBuildableSign->mAuxilaryColor;
	BuildableSign->mEmissive = SourceBuildableSign->mEmissive;
	BuildableSign->mGlossiness = SourceBuildableSign->mGlossiness;
	BuildableSign->mDataVersion = SourceBuildableSign->mDataVersion;
	*/
	BuildableLightsControlPanel->PasteSettings_Implementation(SourceBuildableLightsControlPanel->CopySettings_Implementation());
	BuildableLightsControlPanel->SetLightDataOnControlledLights(SourceBuildableLightsControlPanel->GetLightControlData());
	BuildableLightsControlPanel->SetLightEnabled(SourceBuildableLightsControlPanel->IsLightEnabled());
	BuildableLightsControlPanel->mLightControlData = SourceBuildableLightsControlPanel->mLightControlData;
	BuildableLightsControlPanel->mIsEnabled = SourceBuildableLightsControlPanel->mIsEnabled;

	FLightSourceControlData controlData = FLightSourceControlData();
	controlData.ColorSlotIndex = SourceBuildableLightsControlPanel->mLightControlData.ColorSlotIndex;
	controlData.Intensity = SourceBuildableLightsControlPanel->mLightControlData.Intensity;
	controlData.IsTimeOfDayAware = SourceBuildableLightsControlPanel->mLightControlData.IsTimeOfDayAware;

	BuildableLightsControlPanel->mLightControlData = controlData;
	BuildableLightsControlPanel->OnRep_IsEnabled();
	//BuildableSign->OnRep_SignDataDirty();
	//BuildableBeam->mDefaultLength = SourceBuildableBeam->mDefaultLength;
	//BuildableBeam->mMaxLength = SourceBuildableBeam->mMaxLength;
	//BuildableBeam->mLength = SourceBuildableBeam->mLength;

	TSubclassOf<UFGRecipe> Recipe = SplineHologramFactory->GetRecipeFromClass(Source->GetClass());
	if (!Recipe) Recipe = Source->GetBuiltWithRecipe();
	if (!Recipe) return nullptr;

	Buildable->SetBuiltWithRecipe(Recipe);

	Buildable->SetCustomizationData_Implementation(Source->GetCustomizationData_Implementation());
	Buildable->FinishSpawning(Transform);

	return Buildable;
}
