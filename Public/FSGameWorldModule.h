

#pragma once

#include "CoreMinimal.h"
#include "Module/GameWorldModule.h"
#include "FactorySkyline/FSkyline.h"

#include "FSGameWorldModule.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class FACTORYSKYLINE_API UFSGameWorldModule : public UGameWorldModule
{
	GENERATED_BODY()
public:
	UFSGameWorldModule();
	virtual void DispatchLifecycleEvent(ELifecyclePhase Phase) override;
	
};
