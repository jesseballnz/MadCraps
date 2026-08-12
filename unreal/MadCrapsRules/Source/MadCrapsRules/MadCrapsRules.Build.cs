using UnrealBuildTool;

public class MadCrapsRules : ModuleRules
{
	public MadCrapsRules(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicIncludePaths.AddRange(
			new string[] {
				" MadCrapsRules/Public"
			}
		);

		PrivateIncludePaths.AddRange(
			new string[] {
				"MadCrapsRules/Private"
			}
		);

		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				"CoreUObject",
				"Engine"
			}
		);

		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				// ... add any private deps
			}
		);
	}
}