using UnrealBuildTool;
using System.IO;

public class MadCrapsRules : ModuleRules
{
    public MadCrapsRules(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencies.AddRange(new string[] {
            "Core",
            "CoreUObject",
            "Engine",
            "Slate",
            "SlateCore"
        });
    }
}
