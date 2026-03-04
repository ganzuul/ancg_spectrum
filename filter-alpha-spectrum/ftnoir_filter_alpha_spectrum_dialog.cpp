#include "ftnoir_filter_alpha_spectrum.h"

#include <QCheckBox>
#include <QPushButton>
#include <QSlider>
#include <QSignalBlocker>
#include <QTimer>
#include <QWidget>

namespace {
    static void set_slider_from_value(QSlider* slider, double value, double min, double max)
    {
        const options::slider_value sv{value, min, max};
        const int pos = sv.to_slider_pos(slider->minimum(), slider->maximum());
        slider->setValue(pos);
    }
}

dialog_alpha_spectrum::dialog_alpha_spectrum()
{
    ui.setupUi(this);

    detail::alpha_spectrum::shared_calibration_telemetry().ui_open.store(true, std::memory_order_relaxed);

    connect(ui.buttonBox, SIGNAL(accepted()), this, SLOT(doOK()));
    connect(ui.buttonBox, SIGNAL(rejected()), this, SLOT(doCancel()));

    tie_setting(s.adaptive_mode, ui.adaptive_mode_check);
    tie_setting(s.ema_enabled, ui.ema_enabled_check);
    tie_setting(s.brownian_enabled, ui.brownian_enabled_check);
    tie_setting(s.mtm_enabled, ui.mtm_enabled_check);

    tie_setting(s.rot_alpha_min, ui.rot_min_slider);
    tie_setting(s.rot_alpha_max, ui.rot_max_slider);
    tie_setting(s.rot_curve, ui.rot_curve_slider);
    tie_setting(s.pos_alpha_min, ui.pos_min_slider);
    tie_setting(s.pos_alpha_max, ui.pos_max_slider);
    tie_setting(s.pos_curve, ui.pos_curve_slider);
    tie_setting(s.rot_deadzone, ui.rot_deadzone_slider);
    tie_setting(s.pos_deadzone, ui.pos_deadzone_slider);

    tie_setting(s.rot_alpha_min, ui.rot_min_label,
                [](double x) { return QStringLiteral("%1%").arg(x * 100.0, 0, 'f', 1); });
    tie_setting(s.rot_alpha_max, ui.rot_max_label,
                [](double x) { return QStringLiteral("%1%").arg(x * 100.0, 0, 'f', 1); });
    tie_setting(s.rot_curve, ui.rot_curve_label,
                [](double x) { return QStringLiteral("%1").arg(x, 0, 'f', 2); });
    tie_setting(s.pos_alpha_min, ui.pos_min_label,
                [](double x) { return QStringLiteral("%1%").arg(x * 100.0, 0, 'f', 1); });
    tie_setting(s.pos_alpha_max, ui.pos_max_label,
                [](double x) { return QStringLiteral("%1%").arg(x * 100.0, 0, 'f', 1); });
    tie_setting(s.pos_curve, ui.pos_curve_label,
                [](double x) { return QStringLiteral("%1").arg(x, 0, 'f', 2); });
    tie_setting(s.rot_deadzone, ui.rot_deadzone_label,
                [](double x) { return tr("%1°").arg(x, 0, 'f', 3); });
    tie_setting(s.pos_deadzone, ui.pos_deadzone_label,
                [](double x) { return tr("%1mm").arg(x, 0, 'f', 3); });

    connect(ui.rot_min_slider, &QSlider::valueChanged, this,
            [&](int v) { if (ui.rot_max_slider->value() < v) ui.rot_max_slider->setValue(v); });
    connect(ui.rot_max_slider, &QSlider::valueChanged, this,
            [&](int v) { if (ui.rot_min_slider->value() > v) ui.rot_min_slider->setValue(v); });

    connect(ui.pos_min_slider, &QSlider::valueChanged, this,
            [&](int v) { if (ui.pos_max_slider->value() < v) ui.pos_max_slider->setValue(v); });
    connect(ui.pos_max_slider, &QSlider::valueChanged, this,
            [&](int v) { if (ui.pos_min_slider->value() > v) ui.pos_min_slider->setValue(v); });

    connect(ui.reset_defaults_button, &QPushButton::clicked, this,
            [this] { reset_to_defaults(); });

    auto* calib_timer = new QTimer(this);
    calib_timer->setInterval(100);
    connect(calib_timer, &QTimer::timeout, this, [this] {
        pull_telemetry_into_ui(false);
    });
    calib_timer->start();
}

dialog_alpha_spectrum::~dialog_alpha_spectrum()
{
    detail::alpha_spectrum::shared_calibration_telemetry().ui_open.store(false, std::memory_order_relaxed);
}

void dialog_alpha_spectrum::pull_telemetry_into_ui(bool commit_to_settings)
{
    const auto& telemetry = detail::alpha_spectrum::shared_calibration_telemetry();

    if (!telemetry.ui_open.load(std::memory_order_relaxed) && !telemetry.active.load(std::memory_order_relaxed) && !commit_to_settings)
        return;

    const double rot_min = telemetry.rot_alpha_min.load(std::memory_order_relaxed);
    const double rot_max = telemetry.rot_alpha_max.load(std::memory_order_relaxed);
    const double rot_curve = telemetry.rot_curve.load(std::memory_order_relaxed);
    const double rot_deadzone = telemetry.rot_deadzone.load(std::memory_order_relaxed);
    const double pos_min = telemetry.pos_alpha_min.load(std::memory_order_relaxed);
    const double pos_max = telemetry.pos_alpha_max.load(std::memory_order_relaxed);
    const double pos_curve = telemetry.pos_curve.load(std::memory_order_relaxed);
    const double pos_deadzone = telemetry.pos_deadzone.load(std::memory_order_relaxed);
    const double rot_jitter = telemetry.rot_jitter.load(std::memory_order_relaxed);
    const double pos_jitter = telemetry.pos_jitter.load(std::memory_order_relaxed);
    const double rot_objective = telemetry.rot_objective.load(std::memory_order_relaxed);
    const double pos_objective = telemetry.pos_objective.load(std::memory_order_relaxed);
    const double rot_brownian_raw = telemetry.rot_brownian_raw.load(std::memory_order_relaxed);
    const double rot_brownian_filtered = telemetry.rot_brownian_filtered.load(std::memory_order_relaxed);
    const double rot_brownian_delta = telemetry.rot_brownian_delta.load(std::memory_order_relaxed);
    const double rot_brownian_damped = telemetry.rot_brownian_damped.load(std::memory_order_relaxed);
    const double pos_brownian_raw = telemetry.pos_brownian_raw.load(std::memory_order_relaxed);
    const double pos_brownian_filtered = telemetry.pos_brownian_filtered.load(std::memory_order_relaxed);
    const double pos_brownian_delta = telemetry.pos_brownian_delta.load(std::memory_order_relaxed);
    const double pos_brownian_damped = telemetry.pos_brownian_damped.load(std::memory_order_relaxed);
    const double rot_ema_drive = telemetry.rot_ema_drive.load(std::memory_order_relaxed);
    const double rot_brownian_drive = telemetry.rot_brownian_drive.load(std::memory_order_relaxed);
    const double rot_adaptive_drive = telemetry.rot_adaptive_drive.load(std::memory_order_relaxed);
    const double rot_mtm_drive = telemetry.rot_mtm_drive.load(std::memory_order_relaxed);
    const double pos_ema_drive = telemetry.pos_ema_drive.load(std::memory_order_relaxed);
    const double pos_brownian_drive = telemetry.pos_brownian_drive.load(std::memory_order_relaxed);
    const double pos_adaptive_drive = telemetry.pos_adaptive_drive.load(std::memory_order_relaxed);
    const double pos_mtm_drive = telemetry.pos_mtm_drive.load(std::memory_order_relaxed);
    const double rot_mode_expectation = telemetry.rot_mode_expectation.load(std::memory_order_relaxed);
    const double pos_mode_expectation = telemetry.pos_mode_expectation.load(std::memory_order_relaxed);
    const double rot_mode_peak = telemetry.rot_mode_peak.load(std::memory_order_relaxed);
    const double pos_mode_peak = telemetry.pos_mode_peak.load(std::memory_order_relaxed);
    const double ngc_coupling_residual = telemetry.ngc_coupling_residual.load(std::memory_order_relaxed);
    const bool active = telemetry.active.load(std::memory_order_relaxed);

    {
        const QSignalBlocker b1(ui.rot_min_slider);
        const QSignalBlocker b2(ui.rot_max_slider);
        const QSignalBlocker b3(ui.rot_curve_slider);
        const QSignalBlocker b4(ui.rot_deadzone_slider);
        const QSignalBlocker b5(ui.pos_min_slider);
        const QSignalBlocker b6(ui.pos_max_slider);
        const QSignalBlocker b7(ui.pos_curve_slider);
        const QSignalBlocker b8(ui.pos_deadzone_slider);

        set_slider_from_value(ui.rot_min_slider, rot_min, 0.005, 0.4);
        set_slider_from_value(ui.rot_max_slider, rot_max, 0.02, 1.0);
        set_slider_from_value(ui.rot_curve_slider, rot_curve, 0.2, 8.0);
        set_slider_from_value(ui.rot_deadzone_slider, rot_deadzone, 0.0, 0.3);
        set_slider_from_value(ui.pos_min_slider, pos_min, 0.005, 0.4);
        set_slider_from_value(ui.pos_max_slider, pos_max, 0.02, 1.0);
        set_slider_from_value(ui.pos_curve_slider, pos_curve, 0.2, 8.0);
        set_slider_from_value(ui.pos_deadzone_slider, pos_deadzone, 0.0, 2.0);
    }

    ui.rot_min_label->setText(QStringLiteral("%1%").arg(rot_min * 100.0, 0, 'f', 1));
    ui.rot_max_label->setText(QStringLiteral("%1%").arg(rot_max * 100.0, 0, 'f', 1));
    ui.rot_curve_label->setText(QStringLiteral("%1").arg(rot_curve, 0, 'f', 2));
    ui.rot_deadzone_label->setText(tr("%1°").arg(rot_deadzone, 0, 'f', 3));
    ui.pos_min_label->setText(QStringLiteral("%1%").arg(pos_min * 100.0, 0, 'f', 1));
    ui.pos_max_label->setText(QStringLiteral("%1%").arg(pos_max * 100.0, 0, 'f', 1));
    ui.pos_curve_label->setText(QStringLiteral("%1").arg(pos_curve, 0, 'f', 2));
    ui.pos_deadzone_label->setText(tr("%1mm").arg(pos_deadzone, 0, 'f', 3));

    ui.telemetry_rot_value->setText(QStringLiteral("%1 / %2")
                                        .arg(rot_jitter, 0, 'f', 4)
                                        .arg(rot_objective, 0, 'f', 4));
    ui.telemetry_pos_value->setText(QStringLiteral("%1 / %2")
                                        .arg(pos_jitter, 0, 'f', 4)
                                        .arg(pos_objective, 0, 'f', 4));
    ui.telemetry_rot_brownian_value->setText(
        QStringLiteral("%1 / %2 / Δ%3 / %4%")
            .arg(rot_brownian_raw, 0, 'f', 4)
            .arg(rot_brownian_filtered, 0, 'f', 4)
            .arg(rot_brownian_delta, 0, 'f', 4)
            .arg(rot_brownian_damped * 100.0, 0, 'f', 1));
    ui.telemetry_pos_brownian_value->setText(
        QStringLiteral("%1 / %2 / Δ%3 / %4%")
            .arg(pos_brownian_raw, 0, 'f', 4)
            .arg(pos_brownian_filtered, 0, 'f', 4)
            .arg(pos_brownian_delta, 0, 'f', 4)
            .arg(pos_brownian_damped * 100.0, 0, 'f', 1));
    ui.telemetry_rot_contrib_value->setText(
        QStringLiteral("EMA:%1 Br:%2 Ad:%3 MTM:%4")
            .arg(rot_ema_drive, 0, 'f', 3)
            .arg(rot_brownian_drive, 0, 'f', 3)
            .arg(rot_adaptive_drive, 0, 'f', 3)
            .arg(rot_mtm_drive, 0, 'f', 3));
    ui.telemetry_pos_contrib_value->setText(
        QStringLiteral("EMA:%1 Br:%2 Ad:%3 MTM:%4")
            .arg(pos_ema_drive, 0, 'f', 3)
            .arg(pos_brownian_drive, 0, 'f', 3)
            .arg(pos_adaptive_drive, 0, 'f', 3)
            .arg(pos_mtm_drive, 0, 'f', 3));
    ui.telemetry_status_value->setText(
        active ?
            tr("Monitoring | EMA:%1 Brownian:%2 Adaptive:%3 MTM:%4 | rot E:%5 peak:%6 pos E:%7 peak:%8 κ:%9")
                .arg(*s.ema_enabled ? tr("on") : tr("off"))
                .arg(*s.brownian_enabled ? tr("on") : tr("off"))
                .arg(*s.adaptive_mode ? tr("on") : tr("off"))
                .arg(*s.mtm_enabled ? tr("on") : tr("off"))
                .arg(rot_mode_expectation, 0, 'f', 3)
                .arg(rot_mode_peak, 0, 'f', 3)
                .arg(pos_mode_expectation, 0, 'f', 3)
                .arg(pos_mode_peak, 0, 'f', 3)
                .arg(ngc_coupling_residual, 0, 'f', 3)
            : tr("Idle"));

    if (commit_to_settings)
    {
        const auto rot_min_cfg = *s.rot_alpha_min;
        const auto rot_max_cfg = *s.rot_alpha_max;
        const auto rot_curve_cfg = *s.rot_curve;
        const auto rot_deadzone_cfg = *s.rot_deadzone;
        const auto pos_min_cfg = *s.pos_alpha_min;
        const auto pos_max_cfg = *s.pos_alpha_max;
        const auto pos_curve_cfg = *s.pos_curve;
        const auto pos_deadzone_cfg = *s.pos_deadzone;

        s.rot_alpha_min = options::slider_value{rot_min, rot_min_cfg.min(), rot_min_cfg.max()};
        s.rot_alpha_max = options::slider_value{rot_max, rot_max_cfg.min(), rot_max_cfg.max()};
        s.rot_curve = options::slider_value{rot_curve, rot_curve_cfg.min(), rot_curve_cfg.max()};
        s.rot_deadzone = options::slider_value{rot_deadzone, rot_deadzone_cfg.min(), rot_deadzone_cfg.max()};
        s.pos_alpha_min = options::slider_value{pos_min, pos_min_cfg.min(), pos_min_cfg.max()};
        s.pos_alpha_max = options::slider_value{pos_max, pos_max_cfg.min(), pos_max_cfg.max()};
        s.pos_curve = options::slider_value{pos_curve, pos_curve_cfg.min(), pos_curve_cfg.max()};
        s.pos_deadzone = options::slider_value{pos_deadzone, pos_deadzone_cfg.min(), pos_deadzone_cfg.max()};
    }
}

void dialog_alpha_spectrum::reset_to_defaults()
{
    s.rot_alpha_min.set_to_default();
    s.rot_alpha_max.set_to_default();
    s.rot_curve.set_to_default();
    s.rot_deadzone.set_to_default();
    s.pos_alpha_min.set_to_default();
    s.pos_alpha_max.set_to_default();
    s.pos_curve.set_to_default();
    s.pos_deadzone.set_to_default();
    s.adaptive_mode.set_to_default();
    s.ema_enabled.set_to_default();
    s.brownian_enabled.set_to_default();
    s.mtm_enabled.set_to_default();
    reload();
}

void dialog_alpha_spectrum::doOK()
{
    save();
    close();
}

void dialog_alpha_spectrum::doCancel()
{
    close();
}

void dialog_alpha_spectrum::save()
{
    s.b->save();
}

void dialog_alpha_spectrum::reload()
{
    s.b->reload();
}

void dialog_alpha_spectrum::set_buttons_visible(bool x)
{
    ui.buttonBox->setVisible(x);
}
