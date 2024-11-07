// Copyright Pandores Marketplace 2024. All Rights Reserved.

using UnrealBuildTool;

using System.IO;

public class BlueprintHttp : ModuleRules
{
	public BlueprintHttp(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core"
			}
		);
			
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"CoreUObject",
				"Engine",
				"HTTP"
			}
		);
		
		PublicIncludePaths.Add(Path.Combine(ModuleDirectory, "Public"));
	}
}
