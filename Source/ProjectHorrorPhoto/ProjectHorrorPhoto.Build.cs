// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class ProjectHorrorPhoto : ModuleRules
{
	public ProjectHorrorPhoto(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput","RenderCore" });

		PrivateDependencyModuleNames.AddRange(new string[] { "GameplayAbilities",  "EnhancedInput", "GameplayTags", "CommonUI", "GameplayTasks", "CommonUI", "CommonInput", "UMG", "CableComponent", "UnrealMvvm", "FCTween",});

		// Uncomment if you are using Slate UI
		PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });
		
		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
