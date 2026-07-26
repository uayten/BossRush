#include "TrueColorsSettings.h"
#include "TrueColorsSettingsCustomization.h"

#include "Modules/ModuleManager.h"
#include "PropertyEditorModule.h"

class FTrueColorsEditorModule : public IModuleInterface
{
public:
	virtual void StartupModule() override
	{
		FPropertyEditorModule& PropertyModule =
			FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");

		PropertyModule.RegisterCustomClassLayout(
			UTrueColorsSettings::StaticClass()->GetFName(),
			FOnGetDetailCustomizationInstance::CreateStatic(&FTrueColorsSettingsCustomization::MakeInstance));

		PropertyModule.NotifyCustomizationModuleChanged();
	}

	virtual void ShutdownModule() override
	{
		if (FPropertyEditorModule* PropertyModule =
			FModuleManager::GetModulePtr<FPropertyEditorModule>("PropertyEditor"))
		{
			PropertyModule->UnregisterCustomClassLayout(UTrueColorsSettings::StaticClass()->GetFName());
			PropertyModule->NotifyCustomizationModuleChanged();
		}
	}
};

IMPLEMENT_MODULE(FTrueColorsEditorModule, TrueColorsEditor)
