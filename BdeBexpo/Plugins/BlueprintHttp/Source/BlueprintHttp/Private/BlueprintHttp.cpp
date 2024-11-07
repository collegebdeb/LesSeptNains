// Copyright Pandores Marketplace 2024. All Rights Reserved.

#include "BlueprintHttp.h"
#include "HttpModule.h"

#define LOCTEXT_NAMESPACE "BlueprintHttpModule"

void FBlueprintHttpModule::StartupModule()
{
	const FName HttpModuleName = TEXT("HTTP");
	FHttpModule& Module = FModuleManager::LoadModuleChecked<FHttpModule>(HttpModuleName);
}

void FBlueprintHttpModule::ShutdownModule()
{

}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FBlueprintHttpModule, BlueprintHttp)