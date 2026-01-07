#pragma once
#include <string>
#include <vector>
#include <variant>
#include <map>
#include <iostream>
#include <cstdint>
namespace Tesses::Framework::Serialization::Json
{
    class JArray;
    class JObject;
    using JUndefined = std::monostate;

    using JToken = std::variant<JUndefined,std::nullptr_t,bool,int64_t,double,std::string, JArray, JObject>;

    class JOItem;

      class JArray
    {
        std::vector<JToken> items;
        public:
            JArray();
            JArray(std::initializer_list<JToken> items);
            std::vector<JToken>& GetVector();
            void Add(JToken item);
            void RemoveAt(size_t index);
            size_t Count();
            void SetAt(size_t index, JToken item);
            JToken GetAt(size_t index);
            void Clear();

            std::vector<JToken>::iterator begin();
            std::vector<JToken>::iterator end();
    };
    
  

   

    class JObject {
        std::map<std::string,JToken> map;
        public:
            JObject();
            JObject(std::initializer_list<JOItem> items);
            void SetValue(std::string key, JToken item);
            template<typename T>
            bool TryGetValueAsType(std::string key, T& value)
            {
                auto obj = GetValue(key);
                if(std::holds_alternative<T>(obj))
                {
                    value = std::get<T>(obj);
                    return true;
                }
                return false;       
            }

            JToken GetValue(std::string key);
            void Remove(std::string key);
            std::map<std::string,JToken>& GetMap();
            std::map<std::string,JToken>::iterator begin();
            std::map<std::string,JToken>::iterator end();
    };
     template<typename T>
    bool TryGetJToken(JToken json, T& item)
    {
        if(std::holds_alternative<T>(json))
        {
            item = std::get<T>(json);
            return true;
        }
        return false;
    }
    class JOItem {
        public:
            JOItem();
            JOItem(std::string key, JToken value);
        
            std::string key;
            JToken value;
    };
    
    

    class Json
    {  
            static std::string tab(std::string str);
        public:
            static JToken Decode(std::string str);
            static std::string Encode(JToken tkn, bool indent=true);
            static JArray DocDecode(std::string str);
            static std::string DocEncode(JArray array,bool indent=true);
    };
}
