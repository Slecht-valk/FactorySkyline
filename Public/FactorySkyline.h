// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"
//#include "PixelFormat.h"
//#include "FactoryTick.h"
//#include <C:\Users\game_\OneDrive\Documents\Satisfactory Modding\U8\sf240610Loader2\Source\FactoryGame\Public\FactoryTick.h>

class FFactorySkylineModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};
