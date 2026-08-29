#include "Modules/ModuleManager.h"

class FMadCrapsRulesModule : public IModuleInterface
{
public:
    virtual void StartupModule() override
    {
        UE_LOG(LogTemp, Warning, TEXT("MadCrapsRules module started"));
    }

    virtual void ShutdownModule() override
    {
        UE_LOG(LogTemp, Warning, TEXT("MadCrapsRules module shut down"));
    }
};

IMPLEMENT_MODULE(FMadCrapsRulesModule, MadCrapsRules);
