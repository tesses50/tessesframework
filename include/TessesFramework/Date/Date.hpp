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
        int64_t ToEpochNoConvert();
        void FromEpochNoConvert(int64_t gmt);
        public:
        DateTime();
        DateTime(int year, int month, int day, int hour, int minute, int seconds, bool isLocal=true);
        DateTime(int64_t epoch);
        int Year();
        int Month();
        int Day();
        int Hour();
        int Minute();
        int Second();
        int DayOfWeek();
        bool IsLocal();
        int64_t ToEpoch();
        DateTime ToLocal();
        DateTime ToUTC();
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

        std::string ToString();
        std::string ToString(std::string fmt);


        std::string ToHttpDate();
        static bool TryParseHttpDate(std::string txt, DateTime& dt);


        
    };

};