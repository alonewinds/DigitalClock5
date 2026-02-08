/*
 * SPDX-FileCopyrightText: 2024 Nick Korotysh <nick.korotysh@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#pragma once

#include <QtCore/QDate>
#include <QtCore/QString>

namespace plugin::lunar {

/**
 * @brief 农历日期信息结构
 */
struct LunarDate {
  int year;           // 农历年份 (1900-2100)
  int month;          // 农历月份 (1-12)
  int day;            // 农历日期 (1-30)
  bool isLeapMonth;   // 是否闰月

  int ganIndex;       // 天干索引 (0-9)
  int zhiIndex;       // 地支索引 (0-11)
  int zodiacIndex;    // 生肖索引 (0-11)
};

/**
 * @brief 中国农历计算类
 *
 * 使用查表法计算1900-2100年间的农历日期
 */
class LunarCalendar
{
public:
  /**
   * @brief 将公历日期转换为农历日期
   * @param date 公历日期
   * @return 农历日期信息
   */
  static LunarDate fromSolar(const QDate& date);

  /**
   * @brief 获取天干名称
   * @param index 天干索引 (0-9)
   */
  static QString ganName(int index);

  /**
   * @brief 获取地支名称
   * @param index 地支索引 (0-11)
   */
  static QString zhiName(int index);

  /**
   * @brief 获取生肖名称
   * @param index 生肖索引 (0-11)
   */
  static QString zodiacName(int index);

  /**
   * @brief 获取农历月份名称
   * @param month 月份 (1-12)
   * @param isLeap 是否闰月
   */
  static QString monthName(int month, bool isLeap = false);

  /**
   * @brief 获取农历日期名称
   * @param day 日期 (1-30)
   */
  static QString dayName(int day);

  /**
   * @brief 获取干支年份字符串
   * @param lunar 农历日期
   */
  static QString ganZhiYear(const LunarDate& lunar);

  /**
   * @brief 检查日期是否在有效范围内
   * @param date 公历日期
   */
  static bool isValidDate(const QDate& date);

private:
  static int lunarInfo(int year);
  static int leapMonth(int year);
  static int leapDays(int year);
  static int monthDays(int year, int month);
  static int yearDays(int year);
};

} // namespace plugin::lunar
