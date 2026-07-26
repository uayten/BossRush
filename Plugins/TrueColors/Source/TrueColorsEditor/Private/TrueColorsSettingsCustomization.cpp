#include "TrueColorsSettingsCustomization.h"

#include "DetailCategoryBuilder.h"
#include "DetailLayoutBuilder.h"
#include "DetailWidgetRow.h"
#include "Engine/RendererSettings.h"
#include "TrueColorsSettings.h"
#include "Widgets/Input/SCheckBox.h"
#include "Widgets/Text/STextBlock.h"

#define LOCTEXT_NAMESPACE "TrueColors"

TSharedRef<IDetailCustomization> FTrueColorsSettingsCustomization::MakeInstance()
{
	return MakeShareable(new FTrueColorsSettingsCustomization());
}

void FTrueColorsSettingsCustomization::CustomizeDetails(IDetailLayoutBuilder& DetailBuilder)
{
	// Both categories would otherwise sit at ECategoryPriority::Default and sort alphabetically,
	// putting Exposure above Tonemapping. Pin the order explicitly: Tonemapping is what the
	// plugin is for, the exposure shortcuts are an aside.
	DetailBuilder.EditCategory(TEXT("Tonemapping"), FText::GetEmpty(), ECategoryPriority::Important);

	IDetailCategoryBuilder& Category = DetailBuilder.EditCategory(
		TEXT("PluginRecommendedExposure"),
		LOCTEXT("ExposureCategory", "Plugin Recommended Exposure (Optional)"),
		ECategoryPriority::Uncommon);

	AddRendererCheckBox(Category,
		LOCTEXT("AutoExpOn", "Enable Auto Exposure"),
		LOCTEXT("AutoExpOnTip",
			"r.DefaultFeature.AutoExposure. Unchecking does not switch exposure off -- it pins "
			"Min and Max EV100 to the same value, which is how the engine fixes exposure."),
		[]()
		{
			return GetDefault<URendererSettings>()->bDefaultFeatureAutoExposure != 0;
		},
		[](bool bChecked)
		{
			ApplyRendererSetting(
				GET_MEMBER_NAME_CHECKED(URendererSettings, bDefaultFeatureAutoExposure),
				[bChecked](URendererSettings& Settings) { Settings.bDefaultFeatureAutoExposure = bChecked ? 1 : 0; });
		});

	AddRendererCheckBox(Category,
		LOCTEXT("AutoExpManual", "Manual Auto Exposure Method"),
		LOCTEXT("AutoExpManualTip",
			"r.DefaultFeature.AutoExposure.Method. Unchecking reverts to Histogram, the engine default."),
		[]()
		{
			// Mind the engine's naming: DefaultFeatureAutoExposure (no 'b') is the method,
			// bDefaultFeatureAutoExposure is the on/off switch.
			return GetDefault<URendererSettings>()->DefaultFeatureAutoExposure == EAutoExposureMethodUI::AEM_Manual;
		},
		[](bool bChecked)
		{
			ApplyRendererSetting(
				GET_MEMBER_NAME_CHECKED(URendererSettings, DefaultFeatureAutoExposure),
				[bChecked](URendererSettings& Settings)
				{
					Settings.DefaultFeatureAutoExposure = bChecked
						? EAutoExposureMethodUI::AEM_Manual
						: EAutoExposureMethodUI::AEM_Histogram;
				});
		});

	AddRendererCheckBox(Category,
		LOCTEXT("RecommendedBias", "Use Recommended Exposure Bias Value"),
		LOCTEXT("RecommendedBiasTip",
			"r.DefaultFeature.AutoExposure.Bias. Checked writes 10.0, which suits a fixed Manual "
			"exposure with an extended luminance range; unchecking reverts to 1.0, the engine default."),
		[]()
		{
			return FMath::IsNearlyEqual(
				GetDefault<URendererSettings>()->DefaultFeatureAutoExposureBias,
				UTrueColorsSettings::RecommendedExposureBias);
		},
		[](bool bChecked)
		{
			ApplyRendererSetting(
				GET_MEMBER_NAME_CHECKED(URendererSettings, DefaultFeatureAutoExposureBias),
				[bChecked](URendererSettings& Settings)
				{
					Settings.DefaultFeatureAutoExposureBias = bChecked
						? UTrueColorsSettings::RecommendedExposureBias
						: 1.0f;
				});
		});
}

void FTrueColorsSettingsCustomization::AddRendererCheckBox(IDetailCategoryBuilder& Category, const FText& Label,
	const FText& Tooltip, TFunction<bool()> IsChecked, TFunction<void(bool)> OnChanged)
{
	Category.AddCustomRow(Label)
		.NameContent()
		[
			SNew(STextBlock)
			.Text(Label)
			.ToolTipText(Tooltip)
			.Font(IDetailLayoutBuilder::GetDetailFont())
		]
		.ValueContent()
		[
			SNew(SCheckBox)
			.ToolTipText(Tooltip)
			// Re-evaluated every frame, so the box mirrors the current value on its own --
			// including edits made over in Rendering > Default Settings.
			.IsChecked_Lambda([IsChecked]()
			{
				return IsChecked() ? ECheckBoxState::Checked : ECheckBoxState::Unchecked;
			})
			.OnCheckStateChanged_Lambda([OnChanged](ECheckBoxState NewState)
			{
				OnChanged(NewState == ECheckBoxState::Checked);
			})
		];
}

void FTrueColorsSettingsCustomization::ApplyRendererSetting(FName PropertyName,
	TFunctionRef<void(URendererSettings&)> Mutator)
{
	URendererSettings* Settings = GetMutableDefault<URendererSettings>();
	Mutator(*Settings);

	// PostEditChangeProperty calls ExportValuesToConsoleVariables (which is protected), and that
	// is what mirrors the property onto the live cvar -- without it the change would only take
	// effect on the next restart.
	FProperty* ChangedProperty = URendererSettings::StaticClass()->FindPropertyByName(PropertyName);
	FPropertyChangedEvent ChangedEvent(ChangedProperty);
	Settings->PostEditChangeProperty(ChangedEvent);

	Settings->TryUpdateDefaultConfigFile();
}

#undef LOCTEXT_NAMESPACE
