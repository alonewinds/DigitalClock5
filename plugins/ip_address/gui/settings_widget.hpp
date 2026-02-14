// SPDX-FileCopyrightText: 2025 Nick Korotysh <nick.korotysh@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QWidget>

#include "impl/ip_address_settings.hpp"
#include "impl/network_interfaces_model.hpp"

namespace plugin::ip {

namespace Ui {
class SettingsWidget;
}

class SettingsWidget : public QWidget {
  Q_OBJECT

public:
  explicit SettingsWidget(QWidget *parent = nullptr);
  ~SettingsWidget();

  void initControls(IpAddressPluginInstanceConfig *icfg);

signals:
  void addressesListChanged();
  void extIPDetectorChanged();

private slots:
  void onInterfacesListChanged(const NetworkInterfacesModel::SelectedIPs &ips);

  void on_show_internal_addr_cbox_clicked(bool checked);
  void on_show_external_addr_cbox_clicked(bool checked);
  void on_show_external_v4_cbox_clicked(bool checked);
  void on_show_external_v6_cbox_clicked(bool checked);

  void on_show_local_addr_cbox_clicked(bool checked);
  void on_show_local_location_cbox_clicked(bool checked);

  void on_ext_addr_detect_cbox_activated(int index);

  void on_display_inline_cbox_clicked(bool checked);
  void on_inline_spacing_spinbox_valueChanged(int value);

private:
  Ui::SettingsWidget *ui;
  NetworkInterfacesModel *iface_model;
  IpAddressPluginInstanceConfig *cfg = nullptr;
};

} // namespace plugin::ip
