using UnrealBuildTool;
using System.IO;

public class MadCrapsRules : ModuleRules
{
	public MadCrapsRules(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicIncludePaths.AddRange(
			new string[] {
				Path.Combine(ModuleDirectory, "Public"),
				Path.GetFullPath(Path.Combine(ModuleDirectory, "../../../rules/src"))
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
				"PhysicsCore",
				"RenderCore",
				"Json",
				"JsonUtilities"
			}
		);
	}
}
