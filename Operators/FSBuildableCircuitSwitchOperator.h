// ILikeBanas

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Buildables/FGBuildable.h"
#include "FSBuildableOperator.h"
#include "FSConveyorOperator.h"
#include "FSBuildableCircuitSwitchOperator.generated.h"

/**
 *
 */
UCLASS()
class FACTORYSKYLINE_API UFSBuildableCircuitSwitchOperator : public UFSBuildableOperator
{
	GENERATED_BODY()
public:

	//virtual void UpdateHologramState(const FHitResult& Hit, AFGHologram* Hologram, bool& ShouldShow, bool& Valid);
	//virtual AFGHologram* HologramCopy(FTransform& RelativeTransform);
	virtual AFGBuildable* CreateCopy(const FSTransformOperator& TransformOperator);

	UFGFactoryConnectionComponent* HitConnection;
};