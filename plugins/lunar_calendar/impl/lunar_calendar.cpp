/*
 * SPDX-FileCopyrightText: 2024 Nick Korotysh <nick.korotysh@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "lunar_calendar.hpp"

namespace plugin::lunar {

namespace {

// 农历数据表 (1900-2100)
// 每个数据包含:
// - bit[0-3]: 闰月月份 (0表示无闰月)
// - bit[4]: 闰月大小 (1=30天, 0=29天)
// - bit[5-16]: 1-12月大小 (1=30天, 0=29天)
// - bit[17-20]: 未使用
const unsigned int LUNAR_INFO[] = {
  0x04bd8, 0x04ae0, 0x0a570, 0x054d5, 0x0d260, 0x0d950, 0x16554, 0x056a0, 0x09ad0, 0x055d2, // 1900-1909
  0x04ae0, 0x0a5b6, 0x0a4d0, 0x0d250, 0x1d255, 0x0b540, 0x0d6a0, 0x0ada2, 0x095b0, 0x14977, // 1910-1919
  0x04970, 0x0a4b0, 0x0b4b5, 0x06a50, 0x06d40, 0x1ab54, 0x02b60, 0x09570, 0x052f2, 0x04970, // 1920-1929
  0x06566, 0x0d4a0, 0x0ea50, 0x16a95, 0x05ad0, 0x02b60, 0x186e3, 0x092e0, 0x1c8d7, 0x0c950, // 1930-1939
  0x0d4a0, 0x1d8a6, 0x0b550, 0x056a0, 0x1a5b4, 0x025d0, 0x092d0, 0x0d2b2, 0x0a950, 0x0b557, // 1940-1949
  0x06ca0, 0x0b550, 0x15355, 0x04da0, 0x0a5b0, 0x14573, 0x052b0, 0x0a9a8, 0x0e950, 0x06aa0, // 1950-1959
  0x0aea6, 0x0ab50, 0x04b60, 0x0aae4, 0x0a570, 0x05260, 0x0f263, 0x0d950, 0x05b57, 0x056a0, // 1960-1969
  0x096d0, 0x04dd5, 0x04ad0, 0x0a4d0, 0x0d4d4, 0x0d250, 0x0d558, 0x0b540, 0x0b6a0, 0x195a6, // 1970-1979
  0x095b0, 0x049b0, 0x0a974, 0x0a4b0, 0x0b27a, 0x06a50, 0x06d40, 0x0af46, 0x0ab60, 0x09570, // 1980-1989
  0x04af5, 0x04970, 0x064b0, 0x074a3, 0x0ea50, 0x06b58, 0x05ac0, 0x0ab60, 0x096d5, 0x092e0, // 1990-1999
  0x0c960, 0x0d954, 0x0d4a0, 0x0da50, 0x07552, 0x056a0, 0x0abb7, 0x025d0, 0x092d0, 0x0cab5, // 2000-2009
  0x0a950, 0x0b4a0, 0x0baa4, 0x0ad50, 0x055d9, 0x04ba0, 0x0a5b0, 0x15176, 0x052b0, 0x0a930, // 2010-2019
  0x07954, 0x06aa0, 0x0ad50, 0x05b52, 0x04b60, 0x0a6e6, 0x0a4e0, 0x0d260, 0x0ea65, 0x0d530, // 2020-2029
  0x05aa0, 0x076a3, 0x096d0, 0x04afb, 0x04ad0, 0x0a4d0, 0x1d0b6, 0x0d250, 0x0d520, 0x0dd45, // 2030-2039
  0x0b5a0, 0x056d0, 0x055b2, 0x049b0, 0x0a577, 0x0a4b0, 0x0aa50, 0x1b255, 0x06d20, 0x0ada0, // 2040-2049
  0x14b63, 0x09370, 0x049f8, 0x04970, 0x064b0, 0x168a6, 0x0ea50, 0x06aa0, 0x1a6c4, 0x0aae0, // 2050-2059
  0x092e0, 0x0d2e3, 0x0c960, 0x0d557, 0x0d4a0, 0x0da50, 0x05d55, 0x056a0, 0x0a6d0, 0x055d4, // 2060-2069
  0x052d0, 0x0a9b8, 0x0a950, 0x0b4a0, 0x0b6a6, 0x0ad50, 0x055a0, 0x0aba4, 0x0a5b0, 0x052b0, // 2070-2079
  0x0b273, 0x06930, 0x07337, 0x06aa0, 0x0ad50, 0x14b55, 0x04b60, 0x0a570, 0x054e4, 0x0d160, // 2080-2089
  0x0e968, 0x0d520, 0x0daa0, 0x16aa6, 0x056d0, 0x04ae0, 0x0a9d4, 0x0a2d0, 0x0d150, 0x0f252, // 2090-2099
  0x0d520  // 2100
};

// 1900年1月31日是农历正月初一
const QDate BASE_DATE(1900, 1, 31);
const int BASE_YEAR = 1900;
const int MAX_YEAR = 2100;

// 天干
const QString GAN[] = {
  QStringLiteral("甲"), QStringLiteral("乙"), QStringLiteral("丙"), QStringLiteral("丁"),
  QStringLiteral("戊"), QStringLiteral("己"), QStringLiteral("庚"), QStringLiteral("辛"),
  QStringLiteral("壬"), QStringLiteral("癸")
};

// 地支
const QString ZHI[] = {
  QStringLiteral("子"), QStringLiteral("丑"), QStringLiteral("寅"), QStringLiteral("卯"),
  QStringLiteral("辰"), QStringLiteral("巳"), QStringLiteral("午"), QStringLiteral("未"),
  QStringLiteral("申"), QStringLiteral("酉"), QStringLiteral("戌"), QStringLiteral("亥")
};

// 生肖
const QString ZODIAC[] = {
  QStringLiteral("鼠"), QStringLiteral("牛"), QStringLiteral("虎"), QStringLiteral("兔"),
  QStringLiteral("龙"), QStringLiteral("蛇"), QStringLiteral("马"), QStringLiteral("羊"),
  QStringLiteral("猴"), QStringLiteral("鸡"), QStringLiteral("狗"), QStringLiteral("猪")
};

// 农历月份名称
const QString MONTH_NAME[] = {
  QStringLiteral("正月"), QStringLiteral("二月"), QStringLiteral("三月"), QStringLiteral("四月"),
  QStringLiteral("五月"), QStringLiteral("六月"), QStringLiteral("七月"), QStringLiteral("八月"),
  QStringLiteral("九月"), QStringLiteral("十月"), QStringLiteral("冬月"), QStringLiteral("腊月")
};

// 农历日期名称
const QString DAY_NAME[] = {
  QStringLiteral("初一"), QStringLiteral("初二"), QStringLiteral("初三"), QStringLiteral("初四"), QStringLiteral("初五"),
  QStringLiteral("初六"), QStringLiteral("初七"), QStringLiteral("初八"), QStringLiteral("初九"), QStringLiteral("初十"),
  QStringLiteral("十一"), QStringLiteral("十二"), QStringLiteral("十三"), QStringLiteral("十四"), QStringLiteral("十五"),
  QStringLiteral("十六"), QStringLiteral("十七"), QStringLiteral("十八"), QStringLiteral("十九"), QStringLiteral("二十"),
  QStringLiteral("廿一"), QStringLiteral("廿二"), QStringLiteral("廿三"), QStringLiteral("廿四"), QStringLiteral("廿五"),
  QStringLiteral("廿六"), QStringLiteral("廿七"), QStringLiteral("廿八"), QStringLiteral("廿九"), QStringLiteral("三十")
};

} // namespace

int LunarCalendar::lunarInfo(int year)
{
  if (year < BASE_YEAR || year > MAX_YEAR)
    return 0;
  return LUNAR_INFO[year - BASE_YEAR];
}

int LunarCalendar::leapMonth(int year)
{
  return lunarInfo(year) & 0xf;
}

int LunarCalendar::leapDays(int year)
{
  if (leapMonth(year) == 0)
    return 0;
  return (lunarInfo(year) & 0x10000) ? 30 : 29;
}

int LunarCalendar::monthDays(int year, int month)
{
  if (month < 1 || month > 12)
    return 0;
  return (lunarInfo(year) & (0x10000 >> month)) ? 30 : 29;
}

int LunarCalendar::yearDays(int year)
{
  int sum = 348; // 12个月 * 29天
  for (int i = 0x8000; i > 0x8; i >>= 1) {
    if (lunarInfo(year) & i)
      sum++;
  }
  return sum + leapDays(year);
}

bool LunarCalendar::isValidDate(const QDate& date)
{
  return date.isValid() && date >= BASE_DATE && date.year() <= MAX_YEAR;
}

LunarDate LunarCalendar::fromSolar(const QDate& date)
{
  LunarDate result = {0, 0, 0, false, 0, 0, 0};

  if (!isValidDate(date))
    return result;

  // 计算与基准日期的天数差
  int offset = BASE_DATE.daysTo(date);

  // 计算农历年份
  int lunarYear = BASE_YEAR;
  int daysOfYear = 0;
  while (lunarYear <= MAX_YEAR) {
    daysOfYear = yearDays(lunarYear);
    if (offset < daysOfYear)
      break;
    offset -= daysOfYear;
    lunarYear++;
  }

  // 计算农历月份
  int leap = leapMonth(lunarYear);
  bool isLeap = false;
  int lunarMonth = 1;

  for (int i = 1; i <= 12; i++) {
    int daysOfMonth;

    // 检查是否是闰月
    if (leap > 0 && i == leap + 1 && !isLeap) {
      isLeap = true;
      i--;
      daysOfMonth = leapDays(lunarYear);
    } else {
      daysOfMonth = monthDays(lunarYear, i);
    }

    if (offset < daysOfMonth) {
      lunarMonth = i;
      break;
    }

    offset -= daysOfMonth;

    if (isLeap && i == leap + 1) {
      isLeap = false;
    }
  }

  // 农历日期
  int lunarDay = offset + 1;

  // 计算干支
  int ganIndex = (lunarYear - 4) % 10;
  int zhiIndex = (lunarYear - 4) % 12;

  result.year = lunarYear;
  result.month = lunarMonth;
  result.day = lunarDay;
  result.isLeapMonth = isLeap;
  result.ganIndex = ganIndex;
  result.zhiIndex = zhiIndex;
  result.zodiacIndex = zhiIndex;

  return result;
}

QString LunarCalendar::ganName(int index)
{
  if (index < 0 || index > 9)
    return QString();
  return GAN[index];
}

QString LunarCalendar::zhiName(int index)
{
  if (index < 0 || index > 11)
    return QString();
  return ZHI[index];
}

QString LunarCalendar::zodiacName(int index)
{
  if (index < 0 || index > 11)
    return QString();
  return ZODIAC[index];
}

QString LunarCalendar::monthName(int month, bool isLeap)
{
  if (month < 1 || month > 12)
    return QString();
  QString name = MONTH_NAME[month - 1];
  if (isLeap)
    name = QStringLiteral("闰") + name;
  return name;
}

QString LunarCalendar::dayName(int day)
{
  if (day < 1 || day > 30)
    return QString();
  return DAY_NAME[day - 1];
}

QString LunarCalendar::ganZhiYear(const LunarDate& lunar)
{
  return ganName(lunar.ganIndex) + zhiName(lunar.zhiIndex) + QStringLiteral("年");
}

} // namespace plugin::lunar
