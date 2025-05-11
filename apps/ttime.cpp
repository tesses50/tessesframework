#include "TessesFramework/TessesFramework.hpp"

using namespace Tesses::Framework;
using namespace Tesses::Framework::Date;

int main(int argc, char** argv)
{
    TF_Init();
    DateTime dt=DateTime::Now();
    //dt = dt.ToLocal();
    if(argc > 1)
    {
        if(strcmp(argv[1],"--help") == 0)
        {
            /*
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
            */
            std::cout << argv[0] << " <fmt>" << std::endl;
            std::cout << "The fmt is for DateTime::ToString(fmt) just like strftime but just these are supported:" << std::endl;
            std::cout << "%a: weekday short eg " << dt.ToString("%a") << std::endl;
            std::cout << "%A: weekday long eg " << dt.ToString("%A") << std::endl;
            std::cout << "%S: seconds with leading zeros eg " << dt.ToString("%S") << std::endl;
            std::cout << "%m: month with leading zeros eg " << dt.ToString("%m") << std::endl;
            std::cout << "%d: day with leading zeros eg " << dt.ToString("%d") << std::endl;
            std::cout << "%e: day with leading spaces eg " << dt.ToString("%e") << std::endl;
            std::cout << "%M: minute with leading zeros eg " << dt.ToString("%M") << std::endl;
            std::cout << "%H: 24 hour with leading zeros eg " << dt.ToString("%H") << std::endl;
            std::cout << "%F: %Y-%m-%d like this " << dt.ToString("%F") << std::endl;
            std::cout << "%D: %m/%d/%y like this " << dt.ToString("%D") << std::endl;
            std::cout << "%y: year two digits eg " << dt.ToString("%y") << std::endl;
            std::cout << "%Y: year four digits eg " << dt.ToString("%Y") << std::endl;
            return 1;
        }
        std::cout << dt.ToString(argv[1]) << std::endl;
    }
    else
    std::cout << dt.ToString() << std::endl;
    return 0;
}
