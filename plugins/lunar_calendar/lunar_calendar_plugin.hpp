/*
 * SPDX-FileCopyrightText: 2024 Nick Korotysh <nick.korotysh@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#pragma once

#include "plugin/text/skinned_text_plugin_base.hpp"

#include "impl/lunar_settings.hpp"

using namespace plugin::text;
using plugin::lunar::LunarPluginInstanceConfig;

class LunarCalendarPlugin : public TextPluginInstanceBase
{
  Q_OBJECT

public:
  LunarCalendarPlugin(const LunarPluginInstanceConfig* cfg);

public slots:
  void startup() override;

  void update(const QDateTime& dt) override;

  void onFormatChanged();

protected:
  QString text() const override { return _lunar_str; }

  void pluginReloadConfig() override;

private:
  void updateLunarStr();

private:
  const LunarPluginInstanceConfig* _cfg;
  QDateTime _last_date;
  QString _lunar_str;
};


class LunarCalendarPluginFactory : public TextPluginBase
{
  Q_OBJECT
  Q_PLUGIN_METADATA(IID ClockPlugin_IId FILE "lunar_calendar.json")
  Q_INTERFACES(ClockPlugin)

public:
  QString name() const override { return tr("Lunar Calendar"); }
  QString description() const override;

protected:
  Instance createInstance(size_t idx) const override
  {
    auto cfg = qobject_cast<LunarPluginInstanceConfig*>(instanceConfig(idx));
    Q_ASSERT(cfg);
    return std::make_unique<LunarCalendarPlugin>(cfg);
  }

  std::unique_ptr<PluginConfig> createConfig(
    std::unique_ptr<PluginSettingsBackend> b) const override
  {
    return std::make_unique<plugin::lunar::LunarPluginConfig>(std::move(b));
  }

  QVector<QWidget*> configPagesBeforeCommonPages() override;
};
