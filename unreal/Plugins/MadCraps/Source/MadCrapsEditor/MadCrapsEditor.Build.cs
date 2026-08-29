using UnrealBuildTool;

public class MadCrapsEditor : ModuleRules
{
    public MadCrapsEditor(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencies.AddRange(new string[] {
            "Core",
            "CoreUObject",
            "Engine",
            "UnrealEd",
            "MadCrapsRules"
        });
    }
}
