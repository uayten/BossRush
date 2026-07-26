# True Colors (Tonemap Disabler)

Render colors exactly as authored. No Post Process Volume required.

## The problem

Drop a sprite, a texture, or any unlit material into an Unreal scene and the colors come
out washed out compared to the source file. Disabling auto exposure doesn't fix it. The
cause is the ACES filmic tone curve, which Unreal applies to every view — the same thing
Blender's "Filmic" / "AgX" view transforms do.

Fixing it properly needs **two** values set to zero, not one:

| | |
|---|---|
| `Tone Curve Amount` | the filmic curve itself |
| `Expand Gamut` | pushes saturated highlights outside the working gamut *before* the curve |

Setting only the first gets you most of the way, which is why it's the usual advice — but
it leaves gamut expansion running on saturated colors.

## Why a plugin

You can set both on a Post Process Volume. But a volume only covers the level it sits in,
you have to remember it for every new level, and anything that blends on top can override
it.

There is no project-wide alternative:

- `Tone Curve Amount` has a show flag (`ShowFlag.ToneCurve`), so it can go in `[SystemSettings]`.
- **`Expand Gamut` has no console variable and no show flag.** It exists solely as a
  `PostProcessSettings` property. There is no config file, project setting, or console
  command that reaches it.

This plugin registers an `ISceneViewExtension` and writes both values in `SetupView`,
which the engine calls *after* all post process settings are resolved — after volumes,
after cameras, after blending. That makes it the last write before the tonemapping shader
reads the values, so it applies everywhere and can't be overridden:

- every level, automatically
- editor viewports, play-in-editor, and packaged builds
- scene captures, reflection captures, and asset thumbnails

It uses the engine's public extension API. It does not modify engine files, and does not
replace or patch engine shaders.

## Install

Copy the `TrueColors` folder into your project's `Plugins/` directory and rebuild.
The plugin is enabled automatically.

## Settings

**Project Settings → Plugins → True Colors**

### Tonemapping

| Setting | Default |
|---|---|
| Disable Expand Gamut | on |
| Disable Tone Curve | on |

Both take effect immediately in the viewport — no restart, no recompile. Turn either one
off to compare against stock Unreal.

### Plugin Recommended Exposure (Optional)

Shortcuts, not features. Washed-out colors and wrong exposure are separate problems that
tend to show up together, so these save a trip to another settings page — but every one of
them is an ordinary engine setting you can reach yourself at
**Project Settings → Rendering → Default Settings**.

| Setting | Writes |
|---|---|
| Enable Auto Exposure | `r.DefaultFeature.AutoExposure` |
| Manual Auto Exposure Method | `r.DefaultFeature.AutoExposure.Method` |
| Use Recommended Exposure Bias Value | `r.DefaultFeature.AutoExposure.Bias` = 10.0 |

"Recommended" here means recommended by this plugin, not by Epic: 10.0 suits a fixed Manual
exposure with an extended luminance range, where the physical camera defaults otherwise land
very dark. The engine default is 1.0, and unchecking restores it. If your setup differs, set
the bias yourself in Rendering → Default Settings and leave this box alone.

These checkboxes hold no state of their own — they read the engine settings directly and
tick themselves to match, so they always agree with Rendering → Default Settings no matter
which page you edit from.

One piece of engine trivia worth knowing: unchecking "Enable Auto Exposure" does not switch
exposure off. It pins Min and Max EV100 to the same value, which is how the engine fixes
exposure at a constant.

## What this plugin does not do

It does not replace the tonemapper. If you want a *different* tone curve rather than none
— AgX, GT, Khronos PBR Neutral, Reinhard — this is the wrong tool; look for a tonemapper
replacement plugin instead.

It does not change any exposure value on its own. The Exposure section only writes when you
click it, and nothing under it is touched by default.

## Compatibility

Unreal Engine 5.8.

The call ordering this plugin depends on (`SetupView` running after
`EndFinalPostprocessSettings`) is stable engine behavior, but is worth re-verifying on
major engine upgrades. The source comments point at the exact files and lines to check.

## License

MIT
