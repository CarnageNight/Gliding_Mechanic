// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class FairyLocked : ModuleRules
{
	public FairyLocked(ReadOnlyTargetRules Target) : base(Target)
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
			"Slate"
		});

		PrivateDependencyModuleNames.AddRange(new string[] { });

		PublicIncludePaths.AddRange(new string[] {
			"FairyLocked",
			"FairyLocked/Variant_Platforming",
			"FairyLocked/Variant_Platforming/Animation",
			"FairyLocked/Variant_Combat",
			"FairyLocked/Variant_Combat/AI",
			"FairyLocked/Variant_Combat/Animation",
			"FairyLocked/Variant_Combat/Gameplay",
			"FairyLocked/Variant_Combat/Interfaces",
			"FairyLocked/Variant_Combat/UI",
			"FairyLocked/Variant_SideScrolling",
			"FairyLocked/Variant_SideScrolling/AI",
			"FairyLocked/Variant_SideScrolling/Gameplay",
			"FairyLocked/Variant_SideScrolling/Interfaces",
			"FairyLocked/Variant_SideScrolling/UI"
		});

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
