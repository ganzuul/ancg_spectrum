# Alpha Spectrum filter tuning guide

This filter adapts smoothing strength from recent motion energy per axis.

- Low motion -> alpha stays near **Min** (more smoothing, less jitter)
- High motion -> alpha moves toward **Max** (faster response)

`Adaptive Mode` adds a second stage: when sustained motion is detected, it temporarily boosts responsiveness beyond the base Min/Max/Curve trajectory.

The runtime pipeline is now split into independently toggleable components:

- `EMA head`: applies smoothing update to output
- `Brownian interface`: contributes Brownian signal into adaptive drive and MTM measurement
- `Rényi MTM shoulders`: contributes mode-probability expectation and posterior updates

Internally, each axis is updated as:

- `output += alpha * (input - output)`
- `alpha = min + (max - min) * motion^curve`

where `motion` is a normalized innovation-energy estimate in `[0, 1]`.

## Parameters

## UI layout

The settings panel is split into two columns:

- **Left: Hydra / NGC controls**
  - Brownian Head Gain
  - Adaptive Threshold Lift
  - Predictive Head Gain
  - MTM Shoulder Base
  - NGC Kappa
  - NGC Nominal Z
- **Right: Core EMA controls**
  - Rotation/Translation Min, Max, Curve, Deadzone

### Adaptive Mode

- Default: `enabled`
- Effect:
  - **Enabled**: adds a motion-history responsiveness boost during sustained movement.
  - **Disabled**: uses only the base Min/Max/Curve mapping.
- When to use:
  - Enable for fast gameplay and frequent large turns.
  - Disable for predictable "always same" feel or precision slow-scan use.

### Component toggles

- **Enable EMA smoothing head**
  - **On**: uses `output += alpha * (input - output)`.
  - **Off**: pure pass-through (`output = input`) while telemetry still updates.
- **Enable Brownian interface contribution**
  - **On**: Brownian term contributes to both adaptive drive and MTM measurement.
  - **Off**: Brownian telemetry still computes, but Brownian contribution to control is disabled.
- **Enable Rényi MTM shoulder composition**
  - **On**: mode diffusion + posterior update influence adaptive responsiveness.
  - **Off**: MTM influence is fully removed (`mode_e` does not affect response).

### Reset to defaults

- Use **Reset sliders to defaults** in the settings dialog to restore factory values.
- This action writes default values to the active profile settings immediately (not just in-memory reload).

### Rotation Min / Max

- Range: `0.005 .. 0.4` (Min), `0.02 .. 1.0` (Max)
- Default: `0.04` (Min), `0.65` (Max)
- Effect:
  - Higher **Rotation Min**: less rotational lag at rest, but more visible micro-jitter.
  - Lower **Rotation Min**: steadier view when still, but softer initial response.
  - Higher **Rotation Max**: snappier turns and faster catch-up.

### Rotation Curve

- Range: `0.2 .. 8.0`
- Default: `1.4`
- Effect:
  - Higher curve (>1): keeps heavy smoothing longer, then opens up late during stronger motion.
  - Lower curve (<1): opens up earlier, feeling more immediate.

### Rotation Deadzone

- Range: `0.0 .. 0.3` degrees
- Default: `0.03` degrees
- Effect:
  - Suppresses tiny rotational deltas before adaptation.
  - Too high can make tiny head movements feel ignored.

### Translation Min / Max

- Range: `0.005 .. 0.4` (Min), `0.02 .. 1.0` (Max)
- Default: `0.05` (Min), `0.75` (Max)
- Effect:
  - Higher **Translation Min**: less positional lag but more shake.
  - Lower **Translation Min**: calmer center position but softer micro-movement response.
  - Higher **Translation Max**: faster positional response on larger movement.

### Translation Curve

- Range: `0.2 .. 8.0`
- Default: `1.2`
- Effect:
  - Same shape behavior as rotation curve, for X/Y/Z translation.

### Translation Deadzone

- Range: `0.0 .. 2.0` mm
- Default: `0.1` mm
- Effect:
  - Suppresses tiny translational jitter.
  - Too high can create a sticky center feel.

## Practical tuning workflow

Use this order to avoid chasing interactions:

1. Enable **Adaptive Mode**.
2. Observe telemetry while moving and while still.
3. Tune one family at a time (rotation, then translation).
4. Optionally fine-tune one or two sliders for personal preference.

Make small moves (`~5-10%` of slider range), then test with:

- still head pose (jitter check)
- slow pan
- fast snap turn
- lean in/out

### Two-column quick-start (recommended)

Use this order with the current two-column layout:

1. **Left column (Hydra / NGC)**
  - Start with defaults.
  - Tune **MTM Shoulder Base** first for overall trust level.
  - Tune **Brownian Head Gain** to balance jitter suppression vs response.
  - Tune **Adaptive Threshold Lift** to control when adaptive boosting engages.
  - Tune **Predictive Head Gain** while watching **Predictive error (rot/pos)**.
  - Tune **NGC Kappa** and **NGC Nominal Z** last for translational coupling behavior.
2. **Right column (Core EMA)**
  - Set rotation Min/Max/Curve/Deadzone for desired rotational feel.
  - Set translation Min/Max/Curve/Deadzone for positional feel.
3. **Validation pass**
  - Re-test still pose, slow pan, fast snap turn, and lean in/out.
  - If tuning regresses, use **Reset sliders to defaults** and repeat in the same order.

## Recommended starting profiles

### Stable / simulator focus

- Rotation Min/Max: `0.03 / 0.55`
- Rotation Curve: `1.8`
- Rotation Deadzone: `0.04°`
- Translation Min/Max: `0.04 / 0.65`
- Translation Curve: `1.5`
- Translation Deadzone: `0.12 mm`

### Balanced (default-like)

- Rotation Min/Max: `0.04 / 0.65`
- Rotation Curve: `1.4`
- Rotation Deadzone: `0.03°`
- Translation Min/Max: `0.05 / 0.75`
- Translation Curve: `1.2`
- Translation Deadzone: `0.10 mm`

### Responsive / fast action

- Rotation Min/Max: `0.06 / 0.85`
- Rotation Curve: `0.9`
- Rotation Deadzone: `0.02°`
- Translation Min/Max: `0.07 / 0.90`
- Translation Curve: `0.8`
- Translation Deadzone: `0.06 mm`

## Troubleshooting

- **View jitters while still**
  - Increase Min or deadzone slightly and watch brownian damped telemetry.
- **Feels laggy during turns**
  - Raise Max, lower Curve, or keep Adaptive Mode enabled.
- **Feels too twitchy**
  - Lower Max, raise Curve, or disable Adaptive Mode.
- **Small intentional movement is ignored**
  - Reduce deadzone.

## Notes

- Rotation wrap-around is handled (`±180°` crossing is unwrapped).
- Recenter resets internal state on the next frame.
- Brownian telemetry reports `raw / filtered / delta / damped` where `delta = raw - filtered`.
- Contribution telemetry reports per-family drives: `EMA`, `Brownian`, `Adaptive`, `Predictive`, `MTM`.

## Telemetry status format

The status line uses a compact fixed-width format to avoid UI resizing jitter:

- `Mon|E1 B1 A1 P1 M1|rE0.350 rP0.350 pE0.534 pP0.500 k0.000`

Legend:

- `Mon`: telemetry active
- `E/B/A/P/M`: EMA, Brownian, Adaptive, Predictive, MTM toggles (`1`=on, `0`=off)
- `rE`, `rP`: rotation mode expectation and peak
- `pE`, `pP`: translation mode expectation and peak
- `k`: live NGC coupling residual

M-style mode mixing across explicit motion regimes
