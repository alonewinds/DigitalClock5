/*
 * SPDX-FileCopyrightText: 2024 Nick Korotysh <nick.korotysh@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "settings_widget.hpp"
#include "ui_settings_widget.h"

namespace plugin::lunar {

SettingsWidget::SettingsWidget(QWidget* parent)
  : QWidget(parent)
  , ui(new Ui::SettingsWidget)
{
  ui->setupUi(this);
  setWindowTitle(tr("Format"));
}

SettingsWidget::~SettingsWidget()
{
  delete ui;
}

void SettingsWidget::initControls(LunarPluginInstanceConfig* icfg)
{
  cfg = icfg;
  if (!cfg) return;

  switch (cfg->getFormatType()) {
    case LunarPluginInstanceConfig::Simple:
      ui->simple_rbtn->setChecked(true);
      break;
    case LunarPluginInstanceConfig::Standard:
      ui->standard_rbtn->setChecked(true);
      break;
    case LunarPluginInstanceConfig::Full:
      ui->full_rbtn->setChecked(true);
      break;
  }
}

void SettingsWidget::on_simple_rbtn_clicked()
{
  if (!cfg) return;
  cfg->setFormatType(LunarPluginInstanceConfig::Simple);
  emit formatChanged();
}

void SettingsWidget::on_standard_rbtn_clicked()
{
  if (!cfg) return;
  cfg->setFormatType(LunarPluginInstanceConfig::Standard);
  emit formatChanged();
}

void SettingsWidget::on_full_rbtn_clicked()
{
  if (!cfg) return;
  cfg->setFormatType(LunarPluginInstanceConfig::Full);
  emit formatChanged();
}

} // namespace plugin::lunar
