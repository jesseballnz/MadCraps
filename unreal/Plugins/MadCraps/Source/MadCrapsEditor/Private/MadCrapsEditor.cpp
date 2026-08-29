#include "Modules/ModuleManager.h"

class FMadCrapsEditorModule : public IModuleInterface
{
public:
    virtual void StartupModule() override
    {
        UE_LOG(LogTemp, Warning, TEXT("MadCrapsEditor module started"));
    }

    virtual void ShutdownModule() override
    {
        UE_LOG(LogTemp, Warning, TEXT("MadCrapsEditor module shut down"));
    }
};

IMPLEMENT_MODULE(FMadCrapsEditorModule, MadCrapsEditor);
