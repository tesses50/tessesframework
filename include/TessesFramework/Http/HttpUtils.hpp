#pragma once
#include "../Common.hpp"
#include "../Date/Date.hpp"
#include <algorithm>

namespace Tesses::Framework::Http
{
    typedef enum StatusCode {
    Continue=100,
    SwitchingProtocols=101,
    Processing=102,
    EarlyHints=103,
    OK=200,
    Created=201,
    Accepted=202,
    NonAuthoritativeInformation=203,
    NoContent=204,
    ResetContent=205,
    PartialContent=206,
    MultiStatus=207,
    AlreadyReported=208,
    IMUsed=226,
    MultipleChoices=300,
    MovedPermanently=301,
    Found=302,
    SeeOther=303,
    NotModified=304,
    UseProxy=305,
    TemporaryRedirect=307,
    PermanentRedirect=308,
    BadRequest=400,
    Unauthorized=401,
    PaymentRequired=402,
    Forbidden=403,
    NotFound=404,
    MethodNotAllowed=405,
    NotAcceptable=406,
    ProxyAuthenticationRequired=407,
    RequestTimeout=408,
    Conflict=409,
    Gone=410,
    LengthRequired=411,
    PreconditionFailed=412,
    PayloadTooLarge=413,
    URITooLong=414,
    UnsupportedMediaType=415,
    RangeNotSatisfiable=416,
    ExpectationFailed=417,
    ImATeapot=418,
    MisdirectedRequest=421,
    UnprocessableContent=422,
    Locked=423,
    FailedDependency=424,
    TooEarly=425,
    UpgradeRequired=426,
    PreconditionRequired=428,
    TooManyRequests=429,
    RequestHeaderFieldsTooLarge=431,
    UnavailableForLegalReasons=451,
    InternalServerError=500,
    NotImplemented=501,
    BadGateway=502,
    ServiceUnavailable=503,
    GatewayTimeout=504,
    HTTPVersionNotSupported=505,
    VariantAlsoNegotiates=506,
    InsufficientStorage=507,
    LoopDetected=508,
    NotExtended=510,
    NetworkAuthenticationRequired=511
} StatusCode;

struct CaseInsensitiveLess {
    CaseInsensitiveLess(const CaseInsensitiveLess& str);
    CaseInsensitiveLess();
    CaseInsensitiveLess* offset;
    bool caseSensitive;
     bool operator() (const std::string& s1, const std::string& s2) const;
};
    
    class HttpDictionary {
        public:
            HttpDictionary();
            HttpDictionary(bool caseSensitive);
            std::map<std::string,std::vector<std::string>,CaseInsensitiveLess> kvp;
            void SetCaseSensitive(bool isCaseSensitive);
            void Clear();
            void Clear(std::string key, bool kvpExistsAfter);
            void SetValue(std::string key, std::string value);
            void SetValue(std::string key, int64_t v);
            void SetValue(std::string key, double v);
            void SetValue(std::string key, Date::DateTime v);
            void SetValue(std::string key, std::vector<std::string> value);
            template<typename Itterator>
            void SetValue(std::string key, Itterator begin, Itterator end)
            {
                Clear(key,true);
                AddValue(key, begin, end);
            }
            void AddValue(std::string key, std::string value);
            void AddValue(std::string key, int64_t v);
            void AddValue(std::string key, double v);
            void AddValue(std::string key, Date::DateTime v);
            void AddValue(std::string key, std::vector<std::string> value);
            
            template<typename Itterator>
            void AddValue(std::string key, Itterator begin, Itterator end)
            {
                auto& ls = kvp[key];
                ls.insert(ls.end(), begin, end);
            }

            bool TryGetFirst(std::string key, std::string& value);

            bool TryGetFirstInt(std::string key, int64_t& value);

            bool TryGetFirstDouble(std::string key, double& value);
            bool TryGetFirstDate(std::string key, Date::DateTime& value);

            bool GetFirstBoolean(std::string key);

            bool AnyEquals(std::string key, std::string value);
    };

    class Uri {
        public:
            Uri();
            std::string GetQuery();
            std::string GetPathAndQuery();
            uint16_t GetPort();
            std::string HostPort();
            bool Relative(std::string url, Uri& uri);
            std::string ToString();
            static bool TryParse(std::string url, Uri& uri);
            std::string scheme;
            std::string host;
            uint16_t port;
            std::string path;
            HttpDictionary query;
            std::string hash;
    };

    class HttpUtils
    {
        public:
        static char NibbleToHex(uint8_t nibble);
        static uint8_t HexToNibble(char c);
        static std::string MimeType(std::filesystem::path p);
        static bool Invalid(char c);
        static std::string Sanitise(std::string text);
        static void QueryParamsDecode(HttpDictionary& dict,std::string query);
        static std::string Join(std::string joinStr, std::vector<std::string> ents);
        static std::string QueryParamsEncode(HttpDictionary& dict);
        static std::string UrlDecode(std::string v);
        static std::string UrlEncode(std::string v);
        static std::string UrlPathDecode(std::string v);
        static std::string UrlPathEncode(std::string v, bool ignoreSpace=false);
        static std::string HtmlEncode(std::string v);
        static std::string HtmlP(std::string text);
        static std::string HtmlDecodeOnlyEntityNumber(std::string v);
        static std::vector<std::string> SplitString(std::string text, std::string delimiter,std::size_t maxCnt = std::string::npos);
        static std::string Replace(std::string str, std::string find, std::string replace);
        static std::string StatusCodeString(StatusCode code);
        static std::string ToLower(std::string str);
        static std::string ToUpper(std::string str);
        static std::string LeftPad(std::string text, int count, char c);
    };

 

    
    
}