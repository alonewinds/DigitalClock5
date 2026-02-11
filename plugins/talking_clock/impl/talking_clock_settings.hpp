/*
 * SPDX-FileCopyrightText: 2025 Nick Korotysh <nick.korotysh@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#pragma once

#include <QObject>

#include <QTime>
#include <QUrl>

#include "config/config_base.hpp"
#include "config/variant_converters.hpp"

namespace talking_clock {

class TalkingClockPluginConfig : public QObject, public ConfigBase
{
  Q_OBJECT

public:
  using ConfigBase::ConfigBase;

  // hourly announcement - sound
  CONFIG_OPTION(EveryHourSoundEnabled, "every_hour/sound_enabled", bool, false)
  CONFIG_OPTION(EveryHourSignal, "every_hour/signal", QUrl, QUrl("qrc:/talking_clock/hour_signal.wav"))
  CONFIG_OPTION(EveryHourSoundVolume, "every_hour/sound_volume", int, 90)

  // hourly announcement - speech
  CONFIG_OPTION(EveryHourEnabled, "every_hour/enabled", bool, true)
  CONFIG_OPTION(EveryHourFormat, "every_hour/format", QString, QString("'It''s' ha 'o''clock'"))

  // quarter hour announcement - sound
  CONFIG_OPTION(QuarterHourSoundEnabled, "quarter_hour/sound_enabled", bool, false)
  CONFIG_OPTION(QuarterHourSignal, "quarter_hour/signal", QUrl, QUrl("qrc:/talking_clock/quarter_signal.wav"))
  CONFIG_OPTION(QuarterHourSoundVolume, "quarter_hour/sound_volume", int, 90)

  // quarter hour announcement - speech
  CONFIG_OPTION(QuarterHourEnabled, "quarter_hour/enabled", bool, false)
  CONFIG_OPTION(QuarterHourFormat, "quarter_hour/format", QString, QString("h:m ap"))

  // quiet hours
  CONFIG_OPTION(QuietHoursEnabled, "quiet_hours/enabled", bool, false)
  CONFIG_OPTION(QuietHoursStartTime, "quiet_hours/start_time", QTime, QTime(23, 1))
  CONFIG_OPTION(QuietHoursEndTime, "quiet_hours/end_time", QTime, QTime(6, 59))

  // voice synthesis
  CONFIG_OPTION(Volume, "volume", int, 70)

  CONFIG_OPTION(SynthesisRate, "synthesis/rate", int, 0)
  CONFIG_OPTION(SynthesisPitch, "synthesis/pitch", int, 0)
  CONFIG_OPTION(SynthesisEngine, "synthesis/engine", QString, QString("default"))
  CONFIG_OPTION(SynthesisLanguage, "synthesis/language", int, -1)
  CONFIG_OPTION(SynthesisVoice, "synthesis/voice", int, -1)

public slots:
  void commit() { storage()->commit(); }
  void discard() { storage()->discard(); }
};

} // namespace talking_clock
