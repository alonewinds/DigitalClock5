/*
 * SPDX-FileCopyrightText: 2024 Nick Korotysh <nick.korotysh@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#pragma once

#include <QWidget>

#include "impl/lunar_settings.hpp"

namespace plugin::lunar {

namespace Ui {
class SettingsWidget;
}

class SettingsWidget : public QWidget
{
  Q_OBJECT

public:
  SettingsWidget(QWidget* parent = nullptr);
  ~SettingsWidget();

  void initControls(LunarPluginInstanceConfig* icfg);

signals:
  void formatChanged();

private slots:
  void on_simple_rbtn_clicked();
  void on_standard_rbtn_clicked();
  void on_full_rbtn_clicked();

private:
  Ui::SettingsWidget* ui;
  LunarPluginInstanceConfig* cfg = nullptr;
};

} // namespace plugin::lunar
