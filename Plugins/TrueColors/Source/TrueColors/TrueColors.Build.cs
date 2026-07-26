using UnrealBuildTool;

public class TrueColors : ModuleRules
{
	public TrueColors(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[]
		{
			"Core",
			"CoreUObject",
			"Engine",
			"RenderCore",
			"DeveloperSettings"
		});
	}
}
