// ILikeBanas


#include "FSTrainPlatformCargoOperator.h"
#include "Buildables/FGBuildableTrainPlatform.h"
#include "Buildables/FGBuildableRailroadStation.h"
#include "Buildables/FGBuildableTrainPlatformCargo.h"


void UFSTrainPlatformCargoOperator::ApplySettingsTo(AFGBuildable* Buildable)
{
	Super::ApplySettingsTo(Buildable);

	AFGBuildableTrainPlatformCargo* SourcePlatform = nullptr;

	if (Source.Buildable) {
		SourcePlatform = Cast<AFGBuildableTrainPlatformCargo>(Source.Buildable);
	}

	AFGBuildableTrainPlatformCargo* TargetPlatform = Cast<AFGBuildableTrainPlatformCargo>(Buildable);

	TargetPlatform->mIsInLoadMode = SourcePlatform->mIsInLoadMode;
	TargetPlatform->mShouldExecuteLoadOrUnload = SourcePlatform->mShouldExecuteLoadOrUnload;
	//SML::Logging::info((int)SourcePlatform->mIsInLoadMode);
	//SML::Logging::info((int)TargetPlatform->mIsInLoadMode);
}
