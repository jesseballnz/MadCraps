using System.IO;
using UnrealBuildTool;

public class MadCrapsRules : ModuleRules
{
    public MadCrapsRules(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(
            new string[] { "Core", "CoreUObject", "Engine", "Projects", "Json", "JsonUtilities" }
        );

        PrivateDependencyModuleNames.AddRange(
            new string[] { }
        );

        // ThirdParty library linking (optional)
        string ThirdPartyPath = Path.GetFullPath(Path.Combine(ModuleDirectory, "..", "..", "ThirdParty"));
        // Expect prebuilt static libs at: ThirdParty/madcraps_rules/lib/<Platform>/ and headers at ThirdParty/madcraps_rules/include

        // Example: Add additional include paths and link libraries here when you place the prebuilt artifacts.
        // PublicIncludePaths.Add(Path.Combine(ThirdPartyPath, "madcraps_rules", "include"));
        // PublicAdditionalLibraries.Add(Path.Combine(ThirdPartyPath, "madcraps_rules", "lib", "Win64", "libmadcraps_rules.a"));

        // If you prefer, compile the rules engine into this module directly by adding the sources to this target.

        // Definitions to allow switching implementations later
        PublicDefinitions.Add("MADCRAPS_USE_THIRDPARTY=0");
    }
}
