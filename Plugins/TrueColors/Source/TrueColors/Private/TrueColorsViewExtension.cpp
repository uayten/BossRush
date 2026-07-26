#include "TrueColorsViewExtension.h"

#include "SceneView.h"
#include "TrueColorsSettings.h"

FTrueColorsViewExtension::FTrueColorsViewExtension(const FAutoRegister& AutoRegister)
	: FSceneViewExtensionBase(AutoRegister)
{
}

void FTrueColorsViewExtension::SetupView(FSceneViewFamily& InViewFamily, FSceneView& InView)
{
	const UTrueColorsSettings* Settings = GetDefault<UTrueColorsSettings>();
	if (!Settings)
	{
		return;
	}

	if (Settings->bDisableExpandGamut)
	{
		InView.FinalPostProcessSettings.ExpandGamut = 0.0f;
	}

	if (Settings->bDisableToneCurve)
	{
		InView.FinalPostProcessSettings.ToneCurveAmount = 0.0f;
	}
}
