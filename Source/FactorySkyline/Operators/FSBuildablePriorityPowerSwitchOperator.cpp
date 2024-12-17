// ILikeBanas


#include "FSBuildablePriorityPowerSwitchOperator.h"
#include "Buildables/FGBuildable.h"
#include "Buildables/FGBuildableConveyorLift.h"
#include "Buildables/FGBuildablePriorityPowerSwitch.h"
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

AFGBuildable* UFSBuildablePriorityPowerSwitchOperator::CreateCopy(const FSTransformOperator& TransformOperator)
{
	AFSkyline* FSkyline = AFSkyline::Get(this);

	//FTransform Transform = TransformOperator.Transform(Source->GetTransform());

	FTransform Transform;

	if (Source.Buildable) {
		Transform = TransformOperator.Transform(Source.Buildable->GetTransform());
	}

	AFGBuildable* Buildable = nullptr;
	AFGBuildablePriorityPowerSwitch* SourceBuildablePriorityPowerSwitch = nullptr;

	if (Source.Buildable) {
		Buildable = BuildableSubsystem->BeginSpawnBuildable(Source.Buildable->GetClass(), Transform);
		SourceBuildablePriorityPowerSwitch = Cast<AFGBuildablePriorityPowerSwitch>(Source.Buildable);
	}

	AFGBuildablePriorityPowerSwitch* BuildablePriorityPowerSwitch = Cast<AFGBuildablePriorityPowerSwitch>(Buildable);


	BuildablePriorityPowerSwitch->SetPriority(SourceBuildablePriorityPowerSwitch->GetPriority());
	BuildablePriorityPowerSwitch->SetSwitchOn(SourceBuildablePriorityPowerSwitch->IsSwitchOn());
	BuildablePriorityPowerSwitch->SetBuildingTag_Implementation(SourceBuildablePriorityPowerSwitch->GetBuildingTag_Implementation());
	BuildablePriorityPowerSwitch->SetHasBuildingTag_Implementation(SourceBuildablePriorityPowerSwitch->HasBuildingTag_Implementation());

	//BuildablePriorityPowerSwitch->mPriority = SourceBuildablePriorityPowerSwitch->mPriority;

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
	/*
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
	*/
	//BuildableSign->OnRep_SignDataDirty();
	//BuildableBeam->mDefaultLength = SourceBuildableBeam->mDefaultLength;
	//BuildableBeam->mMaxLength = SourceBuildableBeam->mMaxLength;
	//BuildableBeam->mLength = SourceBuildableBeam->mLength;

	TSubclassOf<UFGRecipe> Recipe;

	if (Source.Buildable) {
		Recipe = SplineHologramFactory->GetRecipeFromClass(Source.Buildable->GetClass());
	}

	if (Source.Buildable) {
		if (!Recipe) Recipe = Source.Buildable->GetBuiltWithRecipe();
	}
	if (!Recipe) return nullptr;

	Buildable->SetBuiltWithRecipe(Recipe);

	if (Source.Buildable) {
		Buildable->SetCustomizationData_Implementation(Source.Buildable->GetCustomizationData_Implementation());
	}
	Buildable->FinishSpawning(Transform);

	return Buildable;
}
