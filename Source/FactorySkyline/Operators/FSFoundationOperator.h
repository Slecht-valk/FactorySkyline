// ILikeBanas

#pragma once

#include "CoreMinimal.h"

#include "FSBuildingOperator.h"
#include "UObject/NoExportTypes.h"
#include "Buildables/FGBuildable.h"
#include "FactorySkyline/Operators/FSBuildableOperator.h"
#include "FSFoundationOperator.generated.h"

/**
 * 
 */
UCLASS()
class FACTORYSKYLINE_API UFSFoundationOperator : public UFSBuildingOperator
{
	GENERATED_BODY()
public:

	virtual void UpdateHologramState(const FHitResult& Hit, AFGHologram* Hologram, bool& ShouldShow, bool& Valid);
	virtual void GetSelectConnectList(FSBuildable* Buildable, TArray<TWeakObjectPtr<AFGBuildable> >& List) const;

};
