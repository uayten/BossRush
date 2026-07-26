#include "TrueColorsViewExtension.h"

#include "Modules/ModuleManager.h"
#include "SceneViewExtension.h"

class FTrueColorsModule : public IModuleInterface
{
public:
	virtual void StartupModule() override
	{
		// Must run after GEngine exists: FSceneViewExtensions::RegisterExtension is guarded by
		// ensure(GEngine) and silently skips registration otherwise, leaving the plugin loaded
		// but inert. Hence LoadingPhase "PostEngineInit" in the .uplugin -- do not lower it.
		ViewExtension = FSceneViewExtensions::NewExtension<FTrueColorsViewExtension>();
	}

	virtual void ShutdownModule() override
	{
		// The extension stays registered for as long as this reference is held.
		ViewExtension.Reset();
	}

private:
	TSharedPtr<FTrueColorsViewExtension, ESPMode::ThreadSafe> ViewExtension;
};

IMPLEMENT_MODULE(FTrueColorsModule, TrueColors)
