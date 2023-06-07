// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class FarmSim : ModuleRules
{
	public FarmSim(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "PhysicsCore", "HeadMountedDisplay", "EnhancedInput", "SimpleInteract", "DLSS", "DLSSBlueprint", "GameplayTags", "BaseController" });

        if (Target.Type == TargetType.Editor)
        {
            PrivateDependencyModuleNames.AddRange(new string[] { "UnrealEd" });
        }

        PublicIncludePaths.AddRange(new string[] 
		{
			"../../FarmSim/Plugins/SimpleInteract/Source/SimpleInteract/",
			"../../FarmSim/Plugins/DLSS/Source",
			"../../FarmSim/Plugins/ColorWheelPlugin",
            "../../FarmSim/Plugins/BaseRebindableController/Source/BaseController/Public",
            "../Source/FarmSim/Public",
            "../Source/FarmSim/Public/AI",
            "../Source/FarmSim/Public/Environment",
            "../Source/FarmSim/Public/Farming",
            "../Source/FarmSim/Public/InventoryAndCrafting",
            "../Source/FarmSim/Public/Player",
            "../Source/FarmSim/Public/Quests",
        });

		PrivateIncludePaths.AddRange(new string[] 
        {
            "../../FarmSim/Plugins/BaseRebindableController/Source/BaseController/Private",
            "../Source/FarmSim/Private/",
            "../Source/FarmSim/Private/AI",
            "../Source/FarmSim/Private/Environment",
            "../Source/FarmSim/Private/Farming",
            "../Source/FarmSim/Private/InventoryAndCrafting",
            "../Source/FarmSim/Private/Player",
            "../Source/FarmSim/Private/Quests",
        });
    }
}
