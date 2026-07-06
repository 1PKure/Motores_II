// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class Motores_II : ModuleRules
{
	public Motores_II(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] {
			"Core",
			"CoreUObject",
			"Engine",
			"InputCore",
			"EnhancedInput",
			"AIModule",
			"StateTreeModule",
			"GameplayStateTreeModule",
			"UMG",
			"Slate",
			"GameplayAbilities",
			"GameplayTags",
    		"GameplayTasks",
    		"Niagara"
		});

		PrivateDependencyModuleNames.AddRange(new string[] { });

		PublicIncludePaths.AddRange(new string[] {
			"Motores_II",
			"Motores_II/Variant_Platforming",
			"Motores_II/Variant_Platforming/Animation",
			"Motores_II/Variant_Combat",
			"Motores_II/Variant_Combat/AI",
			"Motores_II/Variant_Combat/Animation",
			"Motores_II/Variant_Combat/Gameplay",
			"Motores_II/Variant_Combat/Interfaces",
			"Motores_II/Variant_Combat/UI",
			"Motores_II/Variant_SideScrolling",
			"Motores_II/Variant_SideScrolling/AI",
			"Motores_II/Variant_SideScrolling/Gameplay",
			"Motores_II/Variant_SideScrolling/Interfaces",
			"Motores_II/Variant_SideScrolling/UI"
		});

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
