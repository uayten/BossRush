#pragma once

#include "CoreMinimal.h"
#include "IDetailCustomization.h"

class IDetailCategoryBuilder;
class URendererSettings;

/**
 * Adds the optional exposure shortcuts to Project Settings > Plugins > True Colors.
 *
 * These are proxies: they own no state, they read and write URendererSettings directly, so
 * they always agree with Project Settings > Rendering > Default Settings.
 *
 * This has to be an IDetailCustomization rather than UFUNCTION(CallInEditor) or plain
 * properties: Project Settings shows the class CDO, a CDO carries RF_ArchetypeObject
 * (Class.cpp:5091), and FObjectDetails::AddCallInEditorMethods drops objects with that flag
 * before building any widgets (ObjectDetails.cpp:96).
 */
class FTrueColorsSettingsCustomization : public IDetailCustomization
{
public:
	static TSharedRef<IDetailCustomization> MakeInstance();

	virtual void CustomizeDetails(IDetailLayoutBuilder& DetailBuilder) override;

private:
	static void AddRendererCheckBox(IDetailCategoryBuilder& Category, const FText& Label, const FText& Tooltip,
		TFunction<bool()> IsChecked, TFunction<void(bool)> OnChanged);

	/** Writes the property, mirrors it onto the live cvar, and saves to DefaultEngine.ini. */
	static void ApplyRendererSetting(FName PropertyName, TFunctionRef<void(URendererSettings&)> Mutator);
};
