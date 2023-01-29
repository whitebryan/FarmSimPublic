// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class FarmSim : ModuleRules
{
	public FarmSim(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "PhysicsCore", "HeadMountedDisplay", "EnhancedInput", "SimpleInteract", "DLSS", "DLSSBlueprint"});
		PublicIncludePaths.AddRange(new string[] {"D:/GameDev/Unreal/Projects/FarmSim/Plugins/SimpleInteract/Source/SimpleInteract/Public"});
		PublicIncludePaths.AddRange(new string[] {"D:/GameDev/Unreal/Projects/FarmSim/Plugins/DLSS/Source"});
    }
}
