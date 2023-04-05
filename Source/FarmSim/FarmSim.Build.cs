// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class FarmSim : ModuleRules
{
	public FarmSim(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "PhysicsCore", "HeadMountedDisplay", "EnhancedInput", "SimpleInteract", "DLSS", "DLSSBlueprint", "GameplayTags" });
        PrivateDependencyModuleNames.AddRange(new string[] { "UnrealEd" });
        PublicIncludePaths.AddRange(new string[] {"../../FarmSim/Plugins/SimpleInteract/Source/SimpleInteract/Public"});
		PublicIncludePaths.AddRange(new string[] {"../../FarmSim/Plugins/DLSS/Source" });
		PublicIncludePaths.AddRange(new string[] {"../../FarmSim/Plugins/ColorWheelPlugin" });
		PublicIncludePaths.AddRange(new string[] {"./FarmSim/Quests"});
    }
}
