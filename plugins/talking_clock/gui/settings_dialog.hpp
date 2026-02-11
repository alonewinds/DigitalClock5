/*
 * SPDX-FileCopyrightText: 2016-2025 Nick Korotysh <nick.korotysh@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#pragma once

#include <QDialog>

#include "impl/talking_clock_settings.hpp"
#include "config/settings_storage.hpp"

namespace talking_clock {

namespace Ui {
class SettingsDialog;
}

class SettingsDialog : public QDialog
{
  Q_OBJECT

public:
  SettingsDialog(TalkingClockPluginConfig& icfg,
                 SettingsStorage& state,
                 const QTimeZone& tz,
                 QWidget* parent = nullptr);
  ~SettingsDialog();

private slots:
  void on_voice_config_btn_clicked();

  // hourly
  void on_hour_sound_cb_clicked(bool checked);
  void on_hour_sound_btn_clicked();
  void on_hour_speech_cb_clicked(bool checked);
  void on_hour_format_edit_textEdited(const QString& arg1);

  // quarter hour
  void on_quarter_sound_cb_clicked(bool checked);
  void on_quarter_sound_btn_clicked();
  void on_quarter_speech_cb_clicked(bool checked);
  void on_quarter_format_edit_textEdited(const QString& arg1);

  // quiet hours
  void on_quiet_hours_enabled_clicked(bool checked);
  void on_q_time_start_timeChanged(const QTime& time);
  void on_q_time_end_timeChanged(const QTime& time);

private:
  Ui::SettingsDialog* ui;
  TalkingClockPluginConfig& cfg;
  SettingsStorage& state;
  QTimeZone tz;
};

} // namespace talking_clock
