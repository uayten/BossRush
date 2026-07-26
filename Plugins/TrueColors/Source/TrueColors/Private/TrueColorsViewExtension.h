#pragma once

#include "CoreMinimal.h"
#include "SceneViewExtension.h"

/**
 * Overrides ToneCurveAmount and ExpandGamut after all post process settings are resolved.
 *
 * SetupView runs immediately after EndFinalPostprocessSettings on both paths that matter
 * (LocalPlayer.cpp 1006 -> 1011 in game, EditorViewportClient.cpp 1641 -> 1650 in editor),
 * making this the last write before the shader reads the values in
 * PostProcessCombineLUTs.cpp:455. That is why it wins over any Post Process Volume,
 * camera, or blend -- and why it also covers scene captures and thumbnails, which call
 * SetupView through the same path.
 */
class FTrueColorsViewExtension : public FSceneViewExtensionBase
{
public:
	FTrueColorsViewExtension(const FAutoRegister& AutoRegister);

	virtual void SetupView(FSceneViewFamily& InViewFamily, FSceneView& InView) override;
};
