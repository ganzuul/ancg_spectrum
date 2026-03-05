#pragma once

#include "options/options.hpp"

namespace detail::alpha_spectrum {

using namespace options;

struct settings_alpha_spectrum : opts
{
    value<bool> adaptive_mode;
    value<bool> ema_enabled;
    value<bool> brownian_enabled;
    value<bool> predictive_enabled;
    value<bool> mtm_enabled;

    value<slider_value> rot_alpha_min;
    value<slider_value> rot_alpha_max;
    value<slider_value> rot_curve;

    value<slider_value> pos_alpha_min;
    value<slider_value> pos_alpha_max;
    value<slider_value> pos_curve;

    value<slider_value> rot_deadzone;
    value<slider_value> pos_deadzone;
    value<slider_value> brownian_head_gain;
    value<slider_value> adaptive_threshold_lift;
    value<slider_value> predictive_head_gain;
    value<slider_value> mtm_shoulder_base;
    value<slider_value> ngc_kappa;
    value<slider_value> ngc_nominal_z;

    settings_alpha_spectrum() :
        opts("alpha-spectrum-filter"),
        adaptive_mode(b, "adaptive-mode", true),
        ema_enabled(b, "ema-enabled", true),
        brownian_enabled(b, "brownian-enabled", true),
        predictive_enabled(b, "predictive-enabled", true),
        mtm_enabled(b, "mtm-enabled", true),
        rot_alpha_min(b, "rot-alpha-min", { .05, .005, .4 }),
        rot_alpha_max(b, "rot-alpha-max", { .249, .02, 1.0 }),
        rot_curve(b, "rot-curve", { 5.57, .2, 8.0 }),
        pos_alpha_min(b, "pos-alpha-min", { .051, .005, .4 }),
        pos_alpha_max(b, "pos-alpha-max", { .222, .02, 1.0 }),
        pos_curve(b, "pos-curve", { 5.74, .2, 8.0 }),
        rot_deadzone(b, "rot-deadzone", { .143, 0.0, .3 }),
        pos_deadzone(b, "pos-deadzone", { .760, 0.0, 2.0 }),
        brownian_head_gain(b, "brownian-head-gain", { 1.0, 0.0, 2.0 }),
        adaptive_threshold_lift(b, "adaptive-threshold-lift", { 0.15, 0.0, 0.6 }),
        predictive_head_gain(b, "predictive-head-gain", { 1.0, 0.0, 2.0 }),
        mtm_shoulder_base(b, "mtm-shoulder-base", { 0.5, 0.0, 1.0 }),
        ngc_kappa(b, "ngc-kappa", { 0.078, 0.0, 0.3 }),
        ngc_nominal_z(b, "ngc-nominal-z", { 0.85, 0.3, 2.0 })
    {
    }
};

} // ns detail::alpha_spectrum

using detail::alpha_spectrum::settings_alpha_spectrum;
