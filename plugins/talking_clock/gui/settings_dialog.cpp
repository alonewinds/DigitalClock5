/*
 * SPDX-FileCopyrightText: 2016-2025 Nick Korotysh <nick.korotysh@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "settings_dialog.hpp"
#include "ui_settings_dialog.h"

#include "sound_dialog.hpp"
#include "voice_config_dialog.hpp"


#define CURRENT_TIME() QDateTime::currentDateTime().toTimeZone(tz).time()

namespace {

static constexpr const char *const EVERY_HOUR_LAST_FILE =
    "last_every_hour_file";
static constexpr const char *const QUARTER_HOUR_LAST_FILE =
    "last_quarter_hour_file";

QString refined_period(int hour) {
  if (hour < 6)
    return QStringLiteral("凌晨");
  if (hour < 9)
    return QStringLiteral("早上");
  if (hour < 12)
    return QStringLiteral("上午");
  if (hour < 13)
    return QStringLiteral("中午");
  if (hour < 18)
    return QStringLiteral("下午");
  return QStringLiteral("晚上");
}

QString refine_time_period(const QString &text, int hour) {
  QString result = text;
  result.replace(QStringLiteral("上午"), refined_period(hour));
  result.replace(QStringLiteral("下午"), refined_period(hour));
  result.replace(QLatin1String("AM"), refined_period(hour));
  result.replace(QLatin1String("PM"), refined_period(hour));
  return result;
}

} // namespace

namespace talking_clock {

SettingsDialog::SettingsDialog(TalkingClockPluginConfig &icfg,
                               SettingsStorage &st, const QTimeZone &tz,
                               QWidget *parent)
    : QDialog(parent), ui(new Ui::SettingsDialog), cfg(icfg), state(st),
      tz(tz) {
  ui->setupUi(this);

  QSignalBlocker _(this);

  // hourly
  ui->hour_sound_cb->setChecked(cfg.getEveryHourSoundEnabled());
  ui->hour_speech_cb->setChecked(cfg.getEveryHourEnabled());
  ui->hour_format_edit->setText(cfg.getEveryHourFormat());
  ui->hour_format_edit->setToolTip(
      refine_time_period(CURRENT_TIME().toString(ui->hour_format_edit->text()),
                         CURRENT_TIME().hour()));

  // quarter hour
  ui->quarter_sound_cb->setChecked(cfg.getQuarterHourSoundEnabled());
  ui->quarter_speech_cb->setChecked(cfg.getQuarterHourEnabled());
  ui->quarter_format_edit->setText(cfg.getQuarterHourFormat());
  ui->quarter_format_edit->setToolTip(refine_time_period(
      CURRENT_TIME().toString(ui->quarter_format_edit->text()),
      CURRENT_TIME().hour()));

  // quiet hours
  ui->quiet_hours_enabled->setChecked(cfg.getQuietHoursEnabled());
  ui->q_time_start->setTime(cfg.getQuietHoursStartTime());
  ui->q_time_end->setTime(cfg.getQuietHoursEndTime());
}

SettingsDialog::~SettingsDialog() { delete ui; }

void SettingsDialog::on_voice_config_btn_clicked() {
  VoiceConfigDialog dlg(this);

  dlg.setVolume(cfg.getVolume());

  dlg.setRate(cfg.getSynthesisRate());
  dlg.setPitch(cfg.getSynthesisPitch());
  dlg.setEngine(cfg.getSynthesisEngine());
  dlg.setLanguage(cfg.getSynthesisLanguage());
  dlg.setVoice(cfg.getSynthesisVoice());

  if (dlg.exec() == QDialog::Rejected)
    return;
  Q_ASSERT(dlg.result() == QDialog::Accepted);

  cfg.setVolume(dlg.volume());

  cfg.setSynthesisRate(dlg.rate());
  cfg.setSynthesisPitch(dlg.pitch());
  cfg.setSynthesisEngine(dlg.engine());
  cfg.setSynthesisLanguage(dlg.language());
  cfg.setSynthesisVoice(dlg.voice());
}

// --- hourly ---

void SettingsDialog::on_hour_sound_cb_clicked(bool checked) {
  cfg.setEveryHourSoundEnabled(checked);
}

void SettingsDialog::on_hour_sound_btn_clicked() {
  StateStorage st(state);
  SoundDialog dlg(this);
  dlg.setSound(cfg.getEveryHourSignal());
  dlg.setVolume(cfg.getEveryHourSoundVolume());
  dlg.setLastFile(st.value(EVERY_HOUR_LAST_FILE, QString()).toString());
  if (dlg.exec() == QDialog::Rejected)
    return;
  Q_ASSERT(dlg.result() == QDialog::Accepted);
  cfg.setEveryHourSignal(dlg.sound());
  cfg.setEveryHourSoundVolume(dlg.volume());
  if (dlg.sound().scheme() == "file")
    st.setValue(EVERY_HOUR_LAST_FILE, dlg.sound().toLocalFile());
}

void SettingsDialog::on_hour_speech_cb_clicked(bool checked) {
  cfg.setEveryHourEnabled(checked);
}

void SettingsDialog::on_hour_format_edit_textEdited(const QString &arg1) {
  ui->hour_format_edit->setToolTip(
      refine_time_period(CURRENT_TIME().toString(arg1), CURRENT_TIME().hour()));
  cfg.setEveryHourFormat(arg1);
}

// --- quarter hour ---

void SettingsDialog::on_quarter_sound_cb_clicked(bool checked) {
  cfg.setQuarterHourSoundEnabled(checked);
}

void SettingsDialog::on_quarter_sound_btn_clicked() {
  StateStorage st(state);
  SoundDialog dlg(this);
  dlg.setSound(cfg.getQuarterHourSignal());
  dlg.setVolume(cfg.getQuarterHourSoundVolume());
  dlg.setLastFile(st.value(QUARTER_HOUR_LAST_FILE, QString()).toString());
  if (dlg.exec() == QDialog::Rejected)
    return;
  Q_ASSERT(dlg.result() == QDialog::Accepted);
  cfg.setQuarterHourSignal(dlg.sound());
  cfg.setQuarterHourSoundVolume(dlg.volume());
  if (dlg.sound().scheme() == "file")
    st.setValue(QUARTER_HOUR_LAST_FILE, dlg.sound().toLocalFile());
}

void SettingsDialog::on_quarter_speech_cb_clicked(bool checked) {
  cfg.setQuarterHourEnabled(checked);
}

void SettingsDialog::on_quarter_format_edit_textEdited(const QString &arg1) {
  ui->quarter_format_edit->setToolTip(
      refine_time_period(CURRENT_TIME().toString(arg1), CURRENT_TIME().hour()));
  cfg.setQuarterHourFormat(arg1);
}

// --- quiet hours ---

void SettingsDialog::on_quiet_hours_enabled_clicked(bool checked) {
  cfg.setQuietHoursEnabled(checked);
}

void SettingsDialog::on_q_time_start_timeChanged(const QTime &time) {
  cfg.setQuietHoursStartTime(time);
}

void SettingsDialog::on_q_time_end_timeChanged(const QTime &time) {
  cfg.setQuietHoursEndTime(time);
}

} // namespace talking_clock
