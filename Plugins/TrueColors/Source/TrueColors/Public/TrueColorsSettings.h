#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "TrueColorsSettings.generated.h"

// Implementation note, deliberately kept away from the UCLASS: UHT turns the doc comment
// directly above a UCLASS into its tooltip, and the Project Settings panel renders that
// tooltip as the section description. Anything written there is user-facing -- keep internal
// detail out of it.
//
// The tonemapping values below are read per-view by FTrueColorsViewExtension::SetupView,
// which is why toggling them applies with no restart. See TrueColorsViewExtension.h for why
// that hook is the one that works.

/**
 * Renders colors exactly as authored, with no Post Process Volume required.
 *
 * Unreal applies an ACES filmic tone curve to every view, which is what makes sprites and
 * unlit materials look washed out compared to the source file. These settings switch it off
 * across all levels, in editor viewports and in packaged builds alike. Changes apply
 * immediately -- no restart needed.
 */
UCLASS(config = Engine, defaultconfig, meta = (DisplayName = "True Colors"))
class TRUECOLORS_API UTrueColorsSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	/**
	 * Forces ExpandGamut to 0 on every view.
	 *
	 * A view extension is the only way to do this globally: ExpandGamut has no console
	 * variable and no show flag, it exists solely as a PostProcessSettings property.
	 */
	UPROPERTY(config, EditAnywhere, Category = "Tonemapping", meta = (
		DisplayName = "Disable Expand Gamut",
		ToolTip = "Stops saturated highlights from being pushed outside the working gamut before the tone curve."))
	bool bDisableExpandGamut = true;

	/**
	 * Forces ToneCurveAmount to 0 on every view.
	 *
	 * Equivalent to ShowFlag.ToneCurve=0, but kept here so both halves of the tone curve
	 * live behind one switch. The engine only considers the curve fully off when
	 * ToneCurveAmount and ExpandGamut are both 0 (see the comment on FPostProcessSettings).
	 */
	UPROPERTY(config, EditAnywhere, Category = "Tonemapping", meta = (
		DisplayName = "Disable Tone Curve",
		ToolTip = "Removes the ACES filmic tone curve. Equivalent to Blender's 'Standard' view transform."))
	bool bDisableToneCurve = true;

	/**
	 * Value written by the "Use Recommended Exposure Bias Value" shortcut.
	 *
	 * Note this is not an engine default: UE templates ship 1.0, and the cvar default is
	 * also 1.0. 10.0 suits a fixed Manual exposure with an extended luminance range, where
	 * the physical camera defaults otherwise land very dark.
	 */
	static constexpr float RecommendedExposureBias = 10.0f;

	virtual FName GetContainerName() const override { return TEXT("Project"); }
	virtual FName GetCategoryName() const override { return TEXT("Plugins"); }
	virtual FName GetSectionName() const override { return TEXT("True Colors"); }
};
