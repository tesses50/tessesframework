#include "TessesFramework/Date/Date.hpp"
#include "TessesFramework/Http/HttpUtils.hpp"
#include "../HowardHinnant_date/date.h"
#if defined(__FreeBSD__)
#include <sys/time.h>
#endif
#include <iomanip>
#include <iostream>
#include <sstream>
using namespace std::chrono;
using namespace date;
namespace Tesses::Framework::Date
{
    int GetTimeZone()
    {
        

        #if defined(__SWITCH__) || defined(_WIN32) || defined(GEKKO) || defined(__PS2__)
        return (int)(-_timezone);
        #elif defined(__FreeBSD__)
        struct timeval tv;
        struct timezone tz;
        gettimeofday(&tv,&tz);
        return -(tz.tz_minuteswest/60);
	    #else
        return (int)(-timezone);
	    #endif
    }
    bool TimeZoneSupportDST()
    {
        #if defined(__SWITCH__) || defined(_WIN32) || defined(GEKKO) || defined(__PS2__)
        return _daylight == 1;
        #elif defined(__FreeBSD__)
        struct timeval tv;
        struct timezone tz;
        gettimeofday(&tv,&tz);
        return tz.tz_dsttime!=0;
        #else
        return daylight == 1;
        #endif
    }
    DateTime::DateTime()
    {
        
    }
    static int64_t div_floor(int64_t a, int64_t b)
    {
        int64_t v = a / b;
        if(a < 0 && (a % b) != 0) v--;
        return v;
    }
    void DateTime::FromEpochNoConvert(int64_t gmt)
    {
        
        auto epoch = date::sys_days{date::January/1/1970};
        epoch += date::days(div_floor(gmt,86400));


        //date::days<int64_t> sys_days_since_epoch = date::days<int64_t>(gmt);


        // Convert sys_days to year_month_day
        date::year_month_day ymd = date::year_month_day(epoch);
        int64_t secs = gmt % 86400;
        if(secs < 0) secs += 86400;


        second = secs % 60;
        secs /= 60;
        minute = secs % 60;
        secs /= 60;
        hour = secs % 24;

        day = (int)(unsigned)ymd.day();
        month = (int)(unsigned)ymd.month();
        year = (int)ymd.year();
    }
    DateTime::DateTime(int64_t epoch)
    {
        this->isLocal=false;
        this->FromEpochNoConvert(epoch);
    }
    void DateTime::Set(int64_t epoch)
    {
        this->isLocal=false;
        this->FromEpochNoConvert(epoch);
    }
    void DateTime::Set(int year, int month, int day, int hour, int minute, int seconds, bool isLocal)
    {
        this->year = year;
        this->month = month;
        this->day = day;
        this->hour = hour;
        this->minute = minute;
        this->second = seconds;
        this->isLocal = isLocal;
    }
    DateTime::DateTime(int year, int month, int day, int hour, int minute, int seconds, bool isLocal)
    {
        this->year = year;
        this->month = month;
        this->day = day;
        this->hour = hour;
        this->minute = minute;
        this->second = seconds;
        this->isLocal = isLocal;
    }
    int DateTime::Year()
    {
        return this->year;
    }
    int DateTime::Month()
    {
        return this->month;
    }
    int DateTime::Day()
    {
        return this->day;
    }
    int DateTime::Hour()
    {
        return this->hour;
    }
    int DateTime::Minute()
    {
        return this->minute;
    }
    int DateTime::Second()
    {
        return this->second;
    }
    bool DateTime::IsLocal()
    {
        return this->isLocal;
    }
    int DateTime::DayOfWeek()
    {
        date::year_month_day ymd(date::year(year),date::month((uint32_t)month),date::day((uint32_t)day));
        date::sys_days d = ymd;
        date::year_month_weekday ymw(d);
        return ymw.weekday().c_encoding() % 7;

    }
    void DateTime::SetToLocal()
    {
        if(this->isLocal) return;
        auto local = this->ToEpochNoConvert();
        local += GetTimeZone();
        if(TimeZoneSupportDST())
        {
            
                auto epoch = date::sys_days{date::January/1/1970};
                epoch += date::days(div_floor(local,86400));
    
                bool isDST = false;
    
                date::year_month_day ymd(epoch);
                
                auto month = (unsigned)ymd.month();
    
                if(month > 3 && month < 11)
                {
                    isDST=true;
                }
                else if(month == 3)
                {
                    auto day = (unsigned)ymd.day();
                    if(day > 14) isDST=true;
                    else if(day >= 8 && day <= 14)
                    {
                        date::year_month_weekday ymw(epoch);
                        auto dow=ymw.weekday().c_encoding();
                        auto secondSunday = day - dow;
                        if(secondSunday < 8) secondSunday+=7;
    
                        if(day > secondSunday) isDST=true;
                        else if(day == secondSunday)
                        {
                            int64_t secs = local % 86400;
                            if(secs < 0) secs += 86400;

                            secs /= 3600;
                            auto _hours = secs % 24;
                            if(_hours >= 2) isDST=true;
                        }
                    }
                }    
                else if(month == 11)
                {
                    auto day = (unsigned)ymd.day();
                    if(day >= 1 && day <= 7)
                    {
                        date::year_month_weekday ymw(epoch);
                        auto dow=ymw.weekday().c_encoding();
                        int32_t firstSunday = (int32_t)day - (int32_t)dow;
                        if(firstSunday < 1) firstSunday+=7;
    
                        if(day < firstSunday) isDST=true;
                        else if(day == firstSunday)
                        {
                            int64_t secs = local % 86400;
                            if(secs < 0) secs += 86400;

                            secs /= 3600;
                            auto _hours = secs % 24;
                            if(_hours <=1) isDST=true;
                        }
                    }
                }
                    
                if(isDST) local += 3600;
            
        }
        this->isLocal=true;
        this->FromEpochNoConvert(local);
    }
    DateTime DateTime::ToLocal()
    {
        DateTime dt = *this;
        dt.SetToLocal();
        return dt;
    }
    void DateTime::SetToUTC()
    {
        if(!this->isLocal) return;
        auto local = this->ToEpochNoConvert();
        local -= GetTimeZone();
        if(TimeZoneSupportDST())
        {
            {
                auto epoch = date::sys_days{date::January/1/1970};
                epoch += date::days(div_floor(local,86400));
    
                bool isDST = false;
    
                date::year_month_day ymd(epoch);
                
                auto month = (unsigned)ymd.month();
    
                if(month > 3 && month < 11)
                {
                    isDST=true;
                }
                else if(month == 3)
                {
                    auto day = (unsigned)ymd.day();
                    if(day > 14) isDST=true;
                    else if(day >= 8 && day <= 14)
                    {
                        date::year_month_weekday ymw(epoch);
                        auto dow=ymw.weekday().c_encoding();
                        auto secondSunday = day - dow;
                        if(secondSunday < 8) secondSunday+=7;
    
                        if(day > secondSunday) isDST=true;
                        else if(day == secondSunday)
                        {
                            int64_t secs = local % 86400;
                            if(secs < 0) secs += 86400;

                            secs /= 3600;
                            auto _hours = secs % 24;
                            if(_hours >= 2) isDST=true;
                        }
                    }
                }    
                else if(month == 11)
                {
                    auto day = (unsigned)ymd.day();
                    if(day >= 1 && day <= 7)
                    {
                        date::year_month_weekday ymw(epoch);
                        auto dow=ymw.weekday().c_encoding();
                        int32_t firstSunday = (int32_t)day - (int32_t)dow;
                        if(firstSunday < 1) firstSunday+=7;
    
                        if(day < firstSunday) isDST=true;
                        else if(day == firstSunday)
                        {
                            int64_t secs = local % 86400;
                            if(secs < 0) secs += 86400;

                            secs /= 3600;
                            auto _hours = secs % 24;
                            if(_hours <=1) isDST=true;
                        }
                    }
                }
                    
                if(isDST) local -= 3600;
            }
        }
       this->isLocal=false;
       this->FromEpochNoConvert(local);
        
    }
    DateTime DateTime::ToUTC()
    {
        DateTime dt = *this;
        dt.SetToUTC();
        return dt;
    }
    int64_t DateTime::ToEpoch()
    {
        if(this->isLocal)
        {
            DateTime dt = this->ToUTC();
            return dt.ToEpochNoConvert();
        }
        return this->ToEpochNoConvert();
    }
    int64_t DateTime::ToEpochNoConvert()
    {
        date::year y = (date::year)year;
        date::month m  = (date::month)month;
        date::day d = (date::day)day;
        date::year_month_day ymd(y,m,d);
        std::chrono::duration hr = hours(this->hour) + minutes(this->minute) + seconds(this->second);
        sys_days sd = ymd;
        auto res = sd - date::sys_days{date::January/1/1970};
        return (res.count() * 86400) + hr.count(); //bad

    }
    DateTime DateTime::NowUTC()
    {
        DateTime theTime((int64_t)time(NULL));
        return theTime;
    }
    void DateTime::SetToNowUTC()
    {
        this->Set((int64_t)time(NULL));
    }
    void DateTime::SetYear(int y)
    {
        this->year = y;
    }
    void DateTime::SetMonth(int m)
    {
        this->month = m;
    }
    void DateTime::SetDay(int d)
    {
        this->day = d;
    }
    void DateTime::SetHour(int h)
    {
        this->hour = h;
    }
    void DateTime::SetMinute(int m)
    {
        this->minute = m;
    }
    void DateTime::SetSecond(int s)
    {
        this->second = s;
    }
    void DateTime::SetLocal(bool local)
    {
        this->isLocal=local;
    }
    void DateTime::SetToNow()
    {
        int64_t local = (int64_t)time(NULL);
        local += GetTimeZone();
        if(TimeZoneSupportDST())
        {
            
                auto epoch = date::sys_days{date::January/1/1970};
                epoch += date::days(div_floor(local,86400));
    
                bool isDST = false;
    
                date::year_month_day ymd(epoch);
                
                auto month = (unsigned)ymd.month();
    
                if(month > 3 && month < 11)
                {
                    isDST=true;
                }
                else if(month == 3)
                {
                    auto day = (unsigned)ymd.day();
                    if(day > 14) isDST=true;
                    else if(day >= 8 && day <= 14)
                    {
                        date::year_month_weekday ymw(epoch);
                        auto dow=ymw.weekday().c_encoding();
                        auto secondSunday = day - dow;
                        if(secondSunday < 8) secondSunday+=7;
    
                        if(day > secondSunday) isDST=true;
                        else if(day == secondSunday)
                        {
                            int64_t secs = local % 86400;
                            if(secs < 0) secs += 86400;

                            secs /= 3600;
                            auto _hours = secs % 24;
                            if(_hours >= 2) isDST=true;
                        }
                    }
                }    
                else if(month == 11)
                {
                    auto day = (unsigned)ymd.day();
                    if(day >= 1 && day <= 7)
                    {
                        date::year_month_weekday ymw(epoch);
                        auto dow=ymw.weekday().c_encoding();
                        int32_t firstSunday = (int32_t)day - (int32_t)dow;
                        if(firstSunday < 1) firstSunday+=7;
    
                        if(day < firstSunday) isDST=true;
                        else if(day == firstSunday)
                        {
                            int64_t secs = local % 86400;
                            if(secs < 0) secs += 86400;

                            secs /= 3600;
                            auto _hours = secs % 24;
                            if(_hours <=1) isDST=true;
                        }
                    }
                }
                    
                if(isDST) local += 3600;
            
        }
        this->isLocal=true;
        this->FromEpochNoConvert(local);
    }
    DateTime DateTime::Now()
    {
        DateTime dt;
        dt.SetToNow();
        return dt;
        
    }
  
    const char* weekday_short[] = {
        "Sun",
        "Mon",
        "Tue",
        "Wed",
        "Thu",
        "Fri",
        "Sat"
    };
    const char* months_short[] = {
        "Jan",
        "Feb",
        "Mar",
        "Apr",
        "May",
        "Jun",
        "Jul",
        "Aug",
        "Sep",
        "Oct",
        "Nov",
        "Dec"
    };
    const char* weekday_long[] = {
        "Sunday",
        "Monday",
        "Tuesday",
        "Wednesday",
        "Thursday",
        "Friday",
        "Saturday"
    };
    const char* months_long[] = {
        "January",
        "Febuary",
        "March",
        "April",
        "May",
        "June",
        "July",
        "August",
        "September",
        "October",
        "November",
        "December"
    };
    bool DateTime::TryParseHttpDate(std::string txt, DateTime& dt)
    {
        //Mon, 24 Jul 2018 11:00:00 GMT
        auto split = Http::HttpUtils::SplitString(txt,", ",2);
        if(split.size() != 2) return false;
        bool validDay = false;
        for(size_t i = 0; i < 7; i++)
        {
            std::string_view d = weekday_short[i];
            if(d == split[0]) validDay=true;
        }
        if(!validDay) return false;
        //DAY MON YEAR HH:MM:SS GMT
        split = Http::HttpUtils::SplitString(split[1]," ",5);
        if(split.size() != 5) return false;
        if(split[4] != "GMT") return false;
        int day=0;
        int mon=0;
        int year=0;
        int hour=0;
        int minute=0;
        int second=0;

        try {
            day = std::stoi(split[0]);
            if(day < 1 && day > 31) return false;
            
        } catch(std::exception& ex) {
            return false;
        }
        for(int i = 0; i < 12; mon++)
        {
            std::string_view d = months_short[i];
            if(d == split[1]) 
            {
                mon = i + 1;
                break;
            }
        }
        if(mon == 0) return false;
        try {
            year = std::stoi(split[2]);
            
        } catch(std::exception& ex) {
            return false;
        }
        split = Http::HttpUtils::SplitString(split[3],":",3);
        if(split.size() != 3) return false;
        try {
            hour = std::stoi(split[0]);

            minute = std::stoi(split[1]);

            second = std::stoi(split[2]);
            
        } catch(std::exception& ex) {
            return false;
        }

        dt.day = day;
        dt.month = mon;
        dt.year = year;
        dt.hour = hour;
        dt.minute = minute;
        dt.second = second;
        return true;
    }
    std::string DateTime::ToHttpDate()
    {
        auto utc=this->ToUTC();
        std::string weekday=weekday_short[utc.DayOfWeek()];
        std::string month = months_short[utc.month-1];
        std::stringstream strm(std::ios_base::out);
        strm << weekday << ", " << std::setfill('0') << std::setw(2) << utc.day;
        strm << " " << month << " " << std::setfill('0') << std::setw(4) << utc.year;
        strm << " "  << std::setfill('0') << std::setw(2) << utc.hour;
        strm << ":"  << std::setfill('0') << std::setw(2) << utc.minute;
        strm << ":"  << std::setfill('0') << std::setw(2) << utc.second;
        strm << " GMT";

        return strm.str();
       
    }
    std::string DateTime::ToString()
    {
        return ToString("%Y/%m/%d %H:%M:%S");
    }
    
    std::string DateTime::ToString(std::string fmt)
    {
        auto weekday = this->DayOfWeek();
      

       std::string text = "";
       for(size_t i = 0; i < fmt.size(); i++)
       {
            if(fmt[i]=='%')
            {
                i++;
                if(i < fmt.size())
                switch(fmt[i])
                {
                    case 'a':
                        text.append(weekday_short[weekday]);
                        break;
                    case 'A':
                        text.append(weekday_long[weekday]);
                        break;
                    case 'S':
                        text.append(Http::HttpUtils::LeftPad(std::to_string(second),2,'0'));
                        break;
                    case 'm':
                        text.append(Http::HttpUtils::LeftPad(std::to_string(month),2,'0'));
                        break;
                    case 'd':
                        text.append(Http::HttpUtils::LeftPad(std::to_string(day),2,'0'));
                        break;
                    case 'e':
                        text.append(Http::HttpUtils::LeftPad(std::to_string(day),2,' '));
                        break;
                    case 'M':
                        text.append(Http::HttpUtils::LeftPad(std::to_string(minute),2,'0'));
                        break;
                    case 'H':
                        text.append(Http::HttpUtils::LeftPad(std::to_string(hour),2,'0'));
                        break;
                    case 'F':
                        text.append(Http::HttpUtils::LeftPad(std::to_string(year),4,'0'));
                        text.push_back('-');
                        text.append(Http::HttpUtils::LeftPad(std::to_string(month),2,'0'));
                        text.push_back('-');
                        text.append(Http::HttpUtils::LeftPad(std::to_string(day),2,'0'));
                        break;
                    case 'D':
                        text.append(Http::HttpUtils::LeftPad(std::to_string(month),2,'0'));
                        text.push_back('/');
                        text.append(Http::HttpUtils::LeftPad(std::to_string(day),2,'0'));
                        text.push_back('/');
                        text.append(Http::HttpUtils::LeftPad(std::to_string(year % 100),2,'0'));
                        break;
                    case 'y':
                        text.append(Http::HttpUtils::LeftPad(std::to_string(year % 100),2,'0'));
                    break;
                    case 'r':
                    {
                        auto hours = hour % 12;
                        if(hours == 0) hours=12;
                        text.append(Http::HttpUtils::LeftPad(std::to_string(hours),2,'0'));
                        text.push_back(':');
                        text.append(Http::HttpUtils::LeftPad(std::to_string(minute),2,'0'));
                        text.push_back(':');
                        text.append(Http::HttpUtils::LeftPad(std::to_string(second),2,'0'));
                        text.append(hour >= 12 ? " PM" : " AM");
                    }
                    break;
                    case 'R':
                    {
                        text.append(Http::HttpUtils::LeftPad(std::to_string(hour),2,'0'));
                        text.push_back(':');
                        text.append(Http::HttpUtils::LeftPad(std::to_string(minute),2,'0'));
                    }
                    break;
                    case 'T':
                    {
                        
                        text.append(Http::HttpUtils::LeftPad(std::to_string(hour),2,'0'));
                        text.push_back(':');
                        text.append(Http::HttpUtils::LeftPad(std::to_string(minute),2,'0'));
                        text.push_back(':');
                        text.append(Http::HttpUtils::LeftPad(std::to_string(second),2,'0'));
                        
                    }
                
                    break;
                    case 'u':
                    {
                        int dow = weekday + 6;
                        dow %= 7;
                        text.append(std::to_string(dow+1));
                    }
                    break;
                    case 'w':
                    {
                        
                        text.append(std::to_string(weekday));
                    }
                    break;
                    case 'c':
                    {
                        text.append(weekday_short[weekday]);
                        text.push_back(' ');
                        text.append(months_short[month]);
                        text.push_back(' ');
                        text.append(Http::HttpUtils::LeftPad(std::to_string(day),2,'0'));
                        text.push_back(' ');
                        text.append(Http::HttpUtils::LeftPad(std::to_string(hour),2,'0'));
                        text.push_back(':');
                        text.append(Http::HttpUtils::LeftPad(std::to_string(minute),2,'0'));
                        text.push_back(':');
                        text.append(Http::HttpUtils::LeftPad(std::to_string(second),2,'0'));
                        text.push_back(' ');
                        text.append(Http::HttpUtils::LeftPad(std::to_string(year),4,'0'));
                    
                    
                    }
                        break;
                    case 'C':
                        text.append(Http::HttpUtils::LeftPad(std::to_string(year / 100),2,'0'));
                    break;
                    case 'Y':
                        text.append(Http::HttpUtils::LeftPad(std::to_string(year),4,'0'));
                    break;
                    case 'p':
                        text.append(hour >= 12 ? "PM" : "AM");
                        break;
                    case 'I':
                    {
                        auto hours = hour % 12;
                        if(hours == 0) hours=12;
                        text.append(Http::HttpUtils::LeftPad(std::to_string(hours),2,'0'));
                    }   
                        break;
                    case 'h':
                    case 'b':
                        text.append(months_short[month-1]);
                        break;
                    case 'B':
                        text.append(months_long[month-1]);
                        break;
                    case '%':
                        text.push_back('%');
                        break;
                    case 'n':
                        text.push_back('\n');
                        break;
                    case 't':
                        text.push_back('\t');
                        break;
                    
                }
            }
            else
            {
                text.push_back(fmt[i]);
            }
       }
       return text;
    }
}
