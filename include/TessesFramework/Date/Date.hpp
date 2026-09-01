/*
    TessesFramework a library to make C++ easier for me, used in CrossLang:
    https://git.tesses.org/tesses50/crosslang

    Copyright (C) 2026 Mike Nolan
    SPDX-License-Identifier: GPL-3.0-or-later WITH TessesFramework-Exception-1.0

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#pragma once
#include <chrono>
#include <cstdint>
#include <string>

namespace Tesses::Framework::Date {

/**
 * @brief Get the time zone in seconds from UTC
 *
 * @return int the utc distance in seconds, west
 */
int GetTimeZone();
/**
 * @brief Does the timezone support daylight savings
 *
 * @return true yes
 * @return false no
 */
bool TimeZoneSupportDST();
/**
 * @brief Stores a DateTime
 *
 */
class DateTime {
    int year = 1970;
    int month = 1;
    int day = 1;
    int hour = 0;
    int minute = 0;
    int second = 0;
    bool isLocal = false;
    int64_t ToEpochNoConvert() const;
    void FromEpochNoConvert(int64_t gmt);

  public:
    /**
     * @brief Construct a new Date Time object with Jan 1, 1970 at 12:00AM UTC
     *
     */
    DateTime();
    /**
     * @brief Construct a new Date Time object
     *
     * @param year the year
     * @param month the month (1-12)
     * @param day the day (1-31)
     * @param hour the hour (0-23)
     * @param minute the minute (0-59)
     * @param seconds the second (0-59)
     * @param isLocal true: your timezone, false: utc
     */
    DateTime(int year, int month, int day, int hour, int minute, int seconds,
             bool isLocal = true);
    /**
     * @brief Construct a new DateTime object
     *
     * @param epoch seconds since Jan 1, 1970 at 12:00AM UTC
     */
    DateTime(int64_t epoch);
    /**
     * @brief Get the year
     *
     * @return int ex 1992 means the year is 1992
     */
    int Year() const;
    /**
     * @brief Get the month (1-12)
     *
     * @return int ex 8 means august
     */
    int Month() const;
    /**
     * @brief Get the day (1-31)
     *
     * @return int ex 20 means the day is the 20th
     */
    int Day() const;
    /**
     * @brief Get the hour (0-23)
     *
     * @return int ex 12 means noon, 0 means midnight, 18 means 6 PM
     */
    int Hour() const;
    /**
     * @brief Get the minute (0-59)
     *
     * @return int ex 15 means quarter after the hour
     */
    int Minute() const;
    /**
     * @brief Get the second (0-59)
     *
     * @return int the seconds value
     */
    int Second() const;
    /**
     * @brief Get the day of week (0-6)
     *
     * @retval 0 Sunday
     * @retval 1 Monday
     * @retval 2 Tuesday
     * @retval 3 Wednesday
     * @retval 4 Thursday
     * @retval 5 Friday
     * @retval 6 Saturday
     */
    int DayOfWeek() const;
    /**
     * @brief Is local timezone
     *
     * @return true yes
     * @return false no
     */
    bool IsLocal() const;
    /**
     * @brief Get the time as epoch
     *
     * @return int64_t seconds since Jan 1, 1970 at 12:00AM UTC
     */
    int64_t ToEpoch() const;
    /**
     * @brief Convert this time to local time
     *
     * @return DateTime this time in local time
     */
    DateTime ToLocal() const;
    /**
     * @brief Convert this time to utc time
     *
     * @return DateTime this time in UTC time
     */
    DateTime ToUTC() const;
    /**
     * @brief Set this time to local
     *
     */
    void SetToLocal();
    /**
     * @brief Set this time to UTC
     *
     */
    void SetToUTC();
    /**
     * @brief Set the current year
     *
     * @param y year
     */
    void SetYear(int y);
    /**
     * @brief Set the current month (1-12)
     *
     * @param m month
     */
    void SetMonth(int m);
    /**
     * @brief Set the current day (1-31)
     *
     * @param d day
     */
    void SetDay(int d);
    /**
     * @brief Set the current hour (0-23)
     *
     * @param h hour
     */
    void SetHour(int h);
    /**
     * @brief Set the current minute (0-59)
     *
     * @param m minute
     */
    void SetMinute(int m);
    /**
     * @brief Set the current second (0-59)
     *
     * @param s second
     */
    void SetSecond(int s);
    /**
     * @brief Set whether this time is local or utc
     *
     * @param local true set this time to localtime or false set this time to
     * utc
     */
    void SetLocal(bool local);
    /**
     * @brief Set the seconds since Jan 1, 1970 at 12:00AM UTC
     *
     * @param epoch seconds since Jan 1, 1970 at 12:00AM UTC
     */
    void Set(int64_t epoch);
    /**
     * @brief Set the time of this DateTime object
     *
     * @param year the year
     * @param month the month (1-12)
     * @param day the day (1-31)
     * @param hour the hour (0-23)
     * @param minute the minute (0-59)
     * @param seconds the second (0-59)
     * @param isLocal true: your timezone, false: utc
     */
    void Set(int year, int month, int day, int hour, int minute, int seconds,
             bool isLocal = true);
    /**
     * @brief Set this DateTime to right now in your timezone
     *
     */
    void SetToNow();
    /**
     * @brief Set this DateTime to right now in utc
     *
     */
    void SetToNowUTC();
    /**
     * @brief Construct a datetime from right now in your timezone
     *
     * @return DateTime the current time object
     */
    static DateTime Now();
    /**
     * @brief Construct a datetime from right now in your timezone
     *
     * @return DateTime the current time object
     */
    static DateTime NowUTC();
    /**
     * @brief Convert date to string same as ToString("%Y/%m/%d %H:%M:%S")
     *
     * @return std::string the date as string
     */
    std::string ToString() const;
    /**
     * @brief Convert date to string with your own fmt
     *
     * @param fmt the format, see:
     * https://git.tesses.org/tesses50/tessesframework/wiki/DateTime_ToString_Formating
     * for more details
     * @return std::string the date as a string based on fmt
     */
    std::string ToString(std::string fmt) const;
    /**
     * @brief Format as IMF-fixdate (RFC 9110 §5.6.7)
     *
     * Example: "Tue, 01 Sep 2026 18:07:05 GMT"
     */
    std::string ToHttpDate() const;
    /**
     * @brief Tries to parse an IMF-fixdate (RFC 9110 §5.6.7) date into a
     * DateTime
     *
     * @param txt example "Tue, 01 Sep 2026 18:07:05 GMT"
     * @param dt a reference to a datetime
     * @return true we did parse the date correctly
     * @return false we didn't parse the date correctly
     */
    static bool TryParseHttpDate(std::string_view txt, DateTime &dt);
    /**
     * @brief Tries to parse an IMF-fixdate (RFC 9110 §5.6.7) date into the
     * DateTime
     *
     * @param txt example "Tue, 01 Sep 2026 18:07:05 GMT"
     * @return true we did parse the date correctly
     * @return false we didn't parse the date correctly
     */
    bool TryParseHttpDate(std::string_view txt);
};
/**
 * @brief Stores a time offset in seconds
 *
 */
class TimeSpan {
    int64_t totalSeconds;

  public:
    /**
     * @brief Construct a new TimeSpan with 0 seconds
     *
     */
    TimeSpan();
    /**
     * @brief Construct a new TimeSpan object with totalSeconds
     *
     */
    TimeSpan(int64_t totalSeconds);
    /**
     * @brief Construct a new TimeSpan object with hours, minutes and seconds
     *
     */
    TimeSpan(int hours, int minutes, int seconds);
    /**
     * @brief Construct a new TimeSpan object with days, hours, minutes and
     * seconds
     *
     */
    TimeSpan(int days, int hours, int minutes, int seconds);
    /**
     * @brief Set the timespan with new days, hours, minutes and seconds
     *
     */
    void Set(int days, int hours, int minutes, int seconds);
    /**
     * @brief Set the timespan with new hours, minutes and seconds
     *
     */
    void Set(int hours, int minutes, int seconds);
    /**
     * @brief Set the days component (hours, minutes, seconds unchanged)
     */
    void SetDays(int d);
    /**
     * @brief Set the hours component (days, minutes, seconds unchanged)
     */
    void SetHours(int h);
    /**
     * @brief Set the minutes component (days, hours, seconds unchanged)
     */
    void SetMinutes(int m);
    /**
     * @brief Set the seconds component (days, hours, minutes unchanged)
     */
    void SetSeconds(int s);
    /**
     * @brief Get the days of the timespan
     *
     */
    int Days() const;
    /**
     * @brief Get the hours of the timespan
     *
     */
    int Hours() const;
    /**
     * @brief Get the minutes of the timespan
     *
     */
    int Minutes() const;
    /**
     * @brief Get the seconds of the timespan
     *
     */
    int Seconds() const;
    /**
     * @brief Get the total seconds of the timespan
     *
     */
    int64_t TotalSeconds() const;
    /**
     * @brief Get the total minutes of the timespan
     *
     */
    int64_t TotalMinutes() const;
    /**
     * @brief Get the total hours of the timespan
     *
     */
    int64_t TotalHours() const;
    /**
     * @brief Set the total seconds of the timespan, removing any old value
     *
     */
    void SetTotalSeconds(int64_t totalSeconds);
    /**
     * @brief Set the total minutes of the timespan, removing any old value
     *
     */
    void SetTotalMinutes(int64_t totalMinutes);
    /**
     * @brief Set the total hours of the timespan, removing any old value
     *
     */
    void SetTotalHours(int64_t totalHours);
    /**
     * @brief Set the total days of the timespan, removing any old value
     *
     */
    void SetTotalDays(int64_t totalHours);
    /**
     * @brief Add seconds to the timespan
     *
     */
    void AddSeconds(int64_t seconds);
    /**
     * @brief Add minutes to the timespan
     *
     */
    void AddMinutes(int64_t minutes);
    /**
     * @brief Add hours to the timespan
     *
     */
    void AddHours(int64_t hours);
    /**
     * @brief Add days to the timespan
     *
     */
    void AddDays(int64_t days);
    /**
     * @brief Converts the timespan to string
     *
     * @param slim If true, omits leading zeros (e.g. "1:00", "10:00",
     * "1:00:00"). If false, always zero-pads (e.g. "00:00:00") unless there are
     * days.
     */
    std::string ToString(bool slim = true) const;
    /**
     * @brief Try to parse a TimeSpan from a string
     *
     * @param text the string to parse
     * @param span receives the parsed TimeSpan on success
     * @return true we parsed successfully
     * @return false we failed to parse
     */
    static bool TryParse(std::string_view text, TimeSpan &span);
    /**
     * @brief Try to parse this timespan from string
     *
     * @param text the string to parse
     * @return true we parsed successfully
     * @return false we failed to parse
     */
    bool TryParse(std::string_view text);
    /**
     * @brief Create a timespan with seconds, same as ctor
     *
     */
    static TimeSpan FromSeconds(int64_t seconds);
    /**
     * @brief Create a timespan with minutes
     *
     */
    static TimeSpan FromMinutes(int64_t minutes);
    /**
     * @brief Create a timespan with hours
     *
     */
    static TimeSpan FromHours(int64_t hours);
    /**
     * @brief Create a timespan with days
     *
     */
    static TimeSpan FromDays(int64_t days);
};

inline DateTime operator+(const DateTime &dt, const TimeSpan &ts) {
    DateTime dt2(dt.ToEpoch() + ts.TotalSeconds());
    if (dt.IsLocal())
        dt2.SetToLocal();
    return dt2;
}
inline DateTime operator+(const TimeSpan &ts, const DateTime &dt) {
    DateTime dt2(dt.ToEpoch() + ts.TotalSeconds());
    if (dt.IsLocal())
        dt2.SetToLocal();
    return dt2;
}
inline TimeSpan operator+(const TimeSpan &ts, const TimeSpan &ts2) {
    return ts.TotalSeconds() + ts2.TotalSeconds();
}
inline DateTime operator-(const DateTime &dt, const TimeSpan &ts) {
    DateTime dt2(dt.ToEpoch() - ts.TotalSeconds());
    if (dt.IsLocal())
        dt2.SetToLocal();
    return dt2;
}
inline TimeSpan operator-(const DateTime &dt, const DateTime &dt2) {
    return dt.ToEpoch() - dt2.ToEpoch();
}
inline TimeSpan operator-(const TimeSpan &ts, const TimeSpan &ts2) {
    return ts.TotalSeconds() - ts2.TotalSeconds();
}
}; // namespace Tesses::Framework::Date