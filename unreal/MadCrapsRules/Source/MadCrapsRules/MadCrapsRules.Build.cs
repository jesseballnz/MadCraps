using System.IO;
using UnrealBuildTool;

public class MadCrapsRules : ModuleRules
{
    public MadCrapsRules(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(
            new string[] { "Core", "CoreUObject", "Engine", "Projects", "Json", "JsonUtilities", "HTTP" }
        );

        PrivateDependencyModuleNames.AddRange(
            new string[] { }
        );

        // ThirdParty library linking (optional)
        string ThirdPartyPath = Path.GetFullPath(Path.Combine(ModuleDirectory, "..", "..", "ThirdParty"));

        // Definitions to allow switching implementations later
        PublicDefinitions.Add("MADCRAPS_USE_THIRDPARTY=0");
    }
}
