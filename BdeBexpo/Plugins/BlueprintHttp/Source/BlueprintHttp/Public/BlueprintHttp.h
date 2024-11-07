// Copyright Pandores Marketplace 2024. All Rights Reserved.

#pragma once

#include "Modules/ModuleManager.h"

class FBlueprintHttpModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;


};
