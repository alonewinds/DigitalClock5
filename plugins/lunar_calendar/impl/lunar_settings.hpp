/*
 * SPDX-FileCopyrightText: 2024 Nick Korotysh <nick.korotysh@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#pragma once

#include "plugin/text/plugin_config.hpp"

namespace plugin::lunar {

class LunarPluginInstanceConfig : public text::PluginInstanceConfig
{
  Q_OBJECT

public:
  using PluginInstanceConfig::PluginInstanceConfig;

  // 格式类型: Simple=简洁, Standard=标准, Full=完整
  enum FormatType { Simple, Standard, Full };
  Q_ENUM(FormatType);

  CONFIG_OPTION(FormatType, "format_type", FormatType, Simple)
};


class LunarPluginConfig : public text::PluginConfig
{
  Q_OBJECT

public:
  using PluginConfig::PluginConfig;

protected:
  std::unique_ptr<text::PluginInstanceConfig>
  createInstanceImpl(std::unique_ptr<SettingsStorage> st) const override
  {
    return std::make_unique<LunarPluginInstanceConfig>(std::move(st));
  }
};

} // namespace plugin::lunar
