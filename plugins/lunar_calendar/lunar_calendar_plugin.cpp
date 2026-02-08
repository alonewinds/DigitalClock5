/*
 * SPDX-FileCopyrightText: 2024 Nick Korotysh <nick.korotysh@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "lunar_calendar_plugin.hpp"

#include "impl/lunar_calendar.hpp"

#include "gui/settings_widget.hpp"

using namespace plugin::lunar;

LunarCalendarPlugin::LunarCalendarPlugin(const LunarPluginInstanceConfig* cfg)
  : TextPluginInstanceBase(*cfg)
  , _cfg(cfg)
{
}

void LunarCalendarPlugin::startup()
{
  _last_date = QDateTime::currentDateTime();
  updateLunarStr();
  TextPluginInstanceBase::startup();
}

void LunarCalendarPlugin::update(const QDateTime& dt)
{
  if (dt.date() != _last_date.date()) {
    _last_date = dt;
    updateLunarStr();
  }
  TextPluginInstanceBase::update(dt);
}

void LunarCalendarPlugin::onFormatChanged()
{
  updateLunarStr();
  repaintWidget();
}

void LunarCalendarPlugin::pluginReloadConfig()
{
  updateLunarStr();
}

void LunarCalendarPlugin::updateLunarStr()
{
  QDate today = _last_date.date();

  if (!LunarCalendar::isValidDate(today)) {
    _lunar_str = tr("Date out of range");
    return;
  }

  LunarDate lunar = LunarCalendar::fromSolar(today);

  QString monthDay = LunarCalendar::monthName(lunar.month, lunar.isLeapMonth)
                   + LunarCalendar::dayName(lunar.day);

  switch (_cfg->getFormatType()) {
    case LunarPluginInstanceConfig::Simple:
      // 简洁: 正月十五
      _lunar_str = monthDay;
      break;

    case LunarPluginInstanceConfig::Standard:
      // 标准: 丙午年 正月十五
      _lunar_str = LunarCalendar::ganZhiYear(lunar)
                 + QStringLiteral(" ")
                 + monthDay;
      break;

    case LunarPluginInstanceConfig::Full:
      // 完整: 丙午年【马】正月十五
      _lunar_str = LunarCalendar::ganZhiYear(lunar)
                 + QStringLiteral("【")
                 + LunarCalendar::zodiacName(lunar.zodiacIndex)
                 + QStringLiteral("】")
                 + monthDay;
      break;
  }
}


QString LunarCalendarPluginFactory::description() const
{
  return tr("Displays Chinese lunar calendar date.");
}

QVector<QWidget*> LunarCalendarPluginFactory::configPagesBeforeCommonPages()
{
  using plugin::lunar::SettingsWidget;
  auto page = new SettingsWidget();
  connect(this, &LunarCalendarPluginFactory::instanceSwitched, page, [this, page](size_t idx) {
    page->initControls(qobject_cast<LunarPluginInstanceConfig*>(instanceConfig(idx)));
    if (!hasInstances()) return;
    disconnect(page, &SettingsWidget::formatChanged, nullptr, nullptr);
    auto inst = qobject_cast<LunarCalendarPlugin*>(instance(idx));
    connect(page, &SettingsWidget::formatChanged, inst, &LunarCalendarPlugin::onFormatChanged);
  });
  return { page };
}
