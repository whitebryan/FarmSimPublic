// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class FarmSim : ModuleRules
{
	public FarmSim(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "HeadMountedDisplay", "EnhancedInput", "SimpleInteract"});
		PublicIncludePaths.AddRange(new string[] {"D:/GameDev/Unreal/FarmSim/Plugins/SimpleInteract/Source/SimpleInteract/Public"});
	}
}
