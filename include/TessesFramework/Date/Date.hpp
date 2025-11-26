#pragma once
#include <string>
#include <chrono>
#include <cstdint>

namespace Tesses::Framework::Date
{
    int GetTimeZone();
    bool TimeZoneSupportDST();
    
    class DateTime {
        int year=1970;
        int month=1;
        int day=1;
        int hour=0;
        int minute=0;
        int second=0;
        bool isLocal=false;
        int64_t ToEpochNoConvert() const;
        void FromEpochNoConvert(int64_t gmt);
        public:
        DateTime();
        DateTime(int year, int month, int day, int hour, int minute, int seconds, bool isLocal=true);
        DateTime(int64_t epoch);
        int Year() const;
        int Month() const;
        int Day() const;
        int Hour() const;
        int Minute() const;
        int Second() const;
        int DayOfWeek() const;
        bool IsLocal() const;
        int64_t ToEpoch() const;
        DateTime ToLocal() const;
        DateTime ToUTC() const;
        void SetToLocal();
        void SetToUTC();
        void SetYear(int y);
        void SetMonth(int m);
        void SetDay(int d);
        void SetHour(int h);
        void SetMinute(int m);
        void SetSecond(int s);
        void SetLocal(bool local);
        void Set(int64_t epoch);
        void Set(int year, int month, int day, int hour, int minute, int seconds, bool isLocal=true);

        

        void SetToNow();
        void SetToNowUTC();
        static DateTime Now();
        static DateTime NowUTC();

        std::string ToString() const;
        std::string ToString(std::string fmt) const;


        std::string ToHttpDate() const;
        static bool TryParseHttpDate(std::string txt, DateTime& dt);

    };
    class TimeSpan {
        int64_t totalSeconds;
        public:
            TimeSpan();
            TimeSpan(int64_t totalSeconds);
            TimeSpan(int hours, int minutes, int seconds);
            TimeSpan(int days,int hours, int minutes, int seconds);
            
            
            void Set(int days, int hours, int minutes, int seconds);
            void Set(int hours, int minutes, int seconds);

            void SetDays(int d);
            void SetHours(int h);
            void SetMinutes(int m);
            void SetSeconds(int s);
            
            
            int Days() const;
            int Hours() const;
            int Minutes() const;
            int Seconds() const;


            int64_t TotalSeconds() const;
            int64_t TotalMinutes() const;
            int64_t TotalHours() const;


            void SetTotalSeconds(int64_t totalSeconds);
            void SetTotalMinutes(int64_t totalMinutes);
            void SetTotalHours(int64_t totalHours);
            
            void AddSeconds(int64_t seconds);
            void AddMinutes(int64_t minutes);
            void AddHours(int64_t hours);
            void AddDays(int64_t days);

            std::string ToString(bool slim=true) const;

            static bool TryParse(std::string text, TimeSpan& span);

            
            
    };

    inline DateTime operator+(const DateTime& dt, const TimeSpan& ts)
    {
        DateTime dt2(dt.ToEpoch() + ts.TotalSeconds());
        if(dt.IsLocal())
            dt2.SetToLocal();
        return dt2;
    }
    inline DateTime operator+(const TimeSpan& ts,const DateTime& dt)
    {
        DateTime dt2(dt.ToEpoch() + ts.TotalSeconds());
        if(dt.IsLocal())
            dt2.SetToLocal();
        return dt2;
    }
    inline TimeSpan operator+(const TimeSpan& ts, const TimeSpan& ts2)
    {
        return ts.TotalSeconds() + ts2.TotalSeconds();
    }
    inline DateTime operator-(const DateTime& dt, const TimeSpan& ts)
    {
        DateTime dt2(dt.ToEpoch() - ts.TotalSeconds());
        if(dt.IsLocal())
            dt2.SetToLocal();
        return dt2;
    }
    inline TimeSpan operator-(const DateTime& dt, const DateTime& dt2)
    {
        return dt.ToEpoch() - dt2.ToEpoch();
    }
    
};