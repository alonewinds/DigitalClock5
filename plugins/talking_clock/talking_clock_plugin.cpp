/*
 * SPDX-FileCopyrightText: 2025 Nick Korotysh <nick.korotysh@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "talking_clock_plugin.hpp"

#include <QAudioOutput>
#include <QMediaPlayer>
#include <QTextToSpeech>

#include "gui/settings_dialog.hpp"

namespace {

bool is_quiet_time(const QTime& c_time, const QTime& s_time, const QTime& e_time)
{
  if (s_time <= e_time)
    return (s_time <= c_time) && (c_time <= e_time);

  return ((s_time <= c_time) && (c_time <= QTime(23, 59, 59, 999))) ||
         ((QTime(0, 0) <= c_time) && (c_time <= e_time));
}

} // namespace

TalkingClockPlugin::TalkingClockPlugin(const TalkingClockPluginConfig& cfg)
  : ClockPluginInstance()
  , _cfg(cfg)
{
  _time_zone = QTimeZone::systemTimeZone();
}

TalkingClockPlugin::~TalkingClockPlugin() = default;

void TalkingClockPlugin::startup()
{
  initSpeechEngine();

  _player = std::make_unique<QMediaPlayer>();
  auto ao = new QAudioOutput(_player.get());
  _player->setAudioOutput(ao);

  connect(_player.get(), &QMediaPlayer::playbackStateChanged, this,
    [this](QMediaPlayer::PlaybackState state) {
      if (state == QMediaPlayer::StoppedState)
        onSoundFinished();
    });
}

void TalkingClockPlugin::shutdown()
{
  if (_speech) {
    _speech->stop();
    _speech.reset();
  }
  if (_player) {
    _player->stop();
    _player->setSource(QUrl());
    _player.reset();
  }
}

void TalkingClockPlugin::update(const QDateTime& dt)
{
  if (!_speech)
    return;

  // don't interrupt ongoing playback
  if (_player && _player->playbackState() == QMediaPlayer::PlayingState)
    return;
  if (_speech->state() == QTextToSpeech::Speaking)
    return;

  if (_time_zone != dt.timeZone())
    _time_zone = dt.timeZone();

  auto cur_time = dt.time();

  if (isQuietTime(cur_time)) {
    _playback_allowed = (cur_time.minute() % 15 != 0);
    return;
  }

  // hourly trigger
  if (cur_time.minute() == 0 && _playback_allowed) {
    bool soundEnabled = _cfg.getEveryHourSoundEnabled();
    bool speechEnabled = _cfg.getEveryHourEnabled();

    if (soundEnabled || speechEnabled) {
      _pending_speech = speechEnabled
        ? _speech->locale().toString(cur_time, _cfg.getEveryHourFormat())
        : QString();

      if (soundEnabled) {
        playSound(_cfg.getEveryHourSignal(), _cfg.getEveryHourSoundVolume());
      } else if (speechEnabled) {
        _speech->say(_pending_speech);
        _pending_speech.clear();
      }
      _playback_allowed = false;
    }
  }

  // quarter hour trigger
  if (cur_time.minute() % 15 == 0 && cur_time.minute() != 0 && _playback_allowed) {
    bool soundEnabled = _cfg.getQuarterHourSoundEnabled();
    bool speechEnabled = _cfg.getQuarterHourEnabled();

    if (soundEnabled || speechEnabled) {
      _pending_speech = speechEnabled
        ? _speech->locale().toString(cur_time, _cfg.getQuarterHourFormat())
        : QString();

      if (soundEnabled) {
        playSound(_cfg.getQuarterHourSignal(), _cfg.getQuarterHourSoundVolume());
      } else if (speechEnabled) {
        _speech->say(_pending_speech);
        _pending_speech.clear();
      }
      _playback_allowed = false;
    }
  }

  _playback_allowed = (cur_time.minute() % 15 != 0);
}

void TalkingClockPlugin::initSpeechEngine()
{
  QString engine_name = _cfg.getSynthesisEngine();
  if (engine_name == "default" || !QTextToSpeech::availableEngines().contains(engine_name))
    _speech = std::make_unique<QTextToSpeech>();
  else
    _speech = std::make_unique<QTextToSpeech>(engine_name);

  _speech->setVolume(_cfg.getVolume() / 100.0);
  _speech->setRate(_cfg.getSynthesisRate() / 100.0);
  _speech->setPitch(_cfg.getSynthesisPitch() / 100.0);

  const auto locales = _speech->availableLocales();
  const auto locale_index = _cfg.getSynthesisLanguage();
  if (locale_index >= 0 && locale_index < locales.size())
    _speech->setLocale(locales.at(locale_index));

  const auto voices = _speech->availableVoices();
  const auto voice_index = _cfg.getSynthesisVoice();
  if (voice_index >= 0 && voice_index < voices.size())
    _speech->setVoice(voices.at(voice_index));
}

bool TalkingClockPlugin::isQuietTime(const QTime& t) const
{
  if (_cfg.getQuietHoursEnabled())
    return is_quiet_time(t, _cfg.getQuietHoursStartTime(), _cfg.getQuietHoursEndTime());
  return false;
}

void TalkingClockPlugin::playSound(const QUrl& url, int volume)
{
  if (!_player) return;
  _player->setSource(QUrl());
  _player->setLoops(1);
  _player->setSource(url);
  _player->audioOutput()->setVolume(volume / 100.);
  _player->play();
}

void TalkingClockPlugin::onSoundFinished()
{
  if (!_pending_speech.isEmpty() && _speech) {
    _speech->say(_pending_speech);
    _pending_speech.clear();
  }
}


void TalkingClockPluginFactory::init(Context&& ctx)
{
  Q_ASSERT(!ctx.active_instances.empty());
  _first_idx = ctx.active_instances.front();
  _cfg = std::make_unique<TalkingClockPluginConfig>(std::move(ctx.settings));
  _state = std::move(ctx.state);
}

QString TalkingClockPluginFactory::description() const
{
  return tr("Announces time with selected period.");
}

ClockPluginInstance* TalkingClockPluginFactory::instance(size_t idx)
{
  if (idx != _first_idx)
    return nullptr;

  if (!_inst)
    _inst = std::make_unique<TalkingClockPlugin>(std::ref(*_cfg));

  return _inst.get();
}

void TalkingClockPluginFactory::configure(QWidget* parent, size_t idx)
{
  Q_UNUSED(idx);
  using talking_clock::SettingsDialog;
  auto tz = _inst ? _inst->tz() : QTimeZone::systemTimeZone();
  auto d = new SettingsDialog(*_cfg, *_state, tz, parent);

  connect(d, &SettingsDialog::accepted, _cfg.get(), &TalkingClockPluginConfig::commit);
  connect(d, &SettingsDialog::rejected, _cfg.get(), &TalkingClockPluginConfig::discard);

  if (_inst) {
    connect(d, &SettingsDialog::accepted, _inst.get(), &TalkingClockPlugin::initSpeechEngine);
    connect(d, &SettingsDialog::rejected, _inst.get(), &TalkingClockPlugin::initSpeechEngine);
  }

  connect(d, &SettingsDialog::finished, d, &QObject::deleteLater);

  d->show();
}
