#include "TessesFramework/Serialization/Json.hpp"
#include "TessesFramework/Http/HttpUtils.hpp"
#include <functional>

#include "TessesFramework/Common.hpp"
using namespace Tesses::Framework::Http;
namespace Tesses::Framework::Serialization::Json
{
    void JObject::Remove(std::string key)
    {
        map.erase(key);
    }
    void JObject::SetValue(std::string key, JToken item)
    {
        if(!std::holds_alternative<JUndefined>(item))
            map[key] = item;
        else
            Remove(key);
    }
    JToken JObject::GetValue(std::string key)
    {
        if(map.count(key)>0)
            return map[key];
        else
            return JUndefined();
    }
    std::map<std::string,JToken>& JObject::GetMap()
    {
        return map;
    }
    std::map<std::string,JToken>::iterator JObject::begin()
    {
        return map.begin();
    }
    std::map<std::string,JToken>::iterator JObject::end()
    {
        return map.end();
    }
    JArray::JArray()
    {

    }
    JArray::JArray(std::initializer_list<JToken> tokens)
    {
        this->items = tokens;
    }
    std::vector<JToken>& JArray::GetVector()
    {
        return items;
    }
    void JArray::Add(JToken item)
    {
        if(!std::holds_alternative<JUndefined>(item))
        items.push_back(item);
    }
    void JArray::RemoveAt(size_t index)
    {
        if(index < items.size())
        items.erase(items.begin()+index);
    }
    size_t JArray::Count()
    {
        return items.size();
    }
    void JArray::SetAt(size_t index, JToken item)
    {
        if(index < items.size() && !std::holds_alternative<JUndefined>(item))
            items[index] = item;
    }
    JToken JArray::GetAt(size_t index)
    {
        if(index < items.size())
            return items.at(index);
        return JUndefined();
    }
    void JArray::Clear()
    {
        items.clear();
    }

    std::vector<JToken>::iterator JArray::begin()
    {
        return items.begin();
    }
    std::vector<JToken>::iterator JArray::end()
    {
        return items.end();
    }
    JOItem::JOItem()
    {

    }
    JOItem::JOItem(std::string key, JToken value)
    {
        this->key = key;
        this->value = value;
    }

    JObject::JObject()
    {

    }
    JObject::JObject(std::initializer_list<JOItem> items)
    {
        for(auto item : items)
            this->map[item.key] = item.value;
    }

    std::string Json::tab(std::string txt)
    {
        std::string newStr="\t";
        for(auto item : txt)
        {
            if(item == '\n')
            {
                newStr.append("\n\t");
            }
            else
            {
                newStr.push_back(item);
            }
        }
        return newStr;
    }
    
    JToken Json::Decode(std::string str)
    {
        std::vector<std::pair<std::string,bool>> tokens;

        std::string buff={};

        std::function<void()> flush = [&buff,&tokens]()->void {
            if(!buff.empty())
            {
                    tokens.push_back(std::pair<std::string,bool>(buff,false));
                    buff={};
            }
        };

        for(size_t i = 0; i < str.size(); i++)
        {
            switch(str[i])
            {
                case '\"':
                {
                    flush();
                    i++;
                    std::string str2={};
                    for(; i < str.size() && str[i] != '\"'; i++)
                    {
                        if(str[i] == '\\')
                        {
                            i++;
                            if(i < str.size())
                            {
                                if(str[i] == 'n')
                                {
                                    str2.push_back('\n');
                                }
                                else if(str[i] == 'r')
                                {
                                    str2.push_back('\r');
                                }
                                else if(str[i] == 't')
                                {
                                    str2.push_back('\t');
                                }
                                else if(str[i] == 'f')
                                {
                                    str2.push_back('\f');
                                }
                                else if(str[i] == 'b')
                                {
                                    str2.push_back('\b');
                                }
                                else if(str[i] == 'u')
                                {
                                    i++;
                                    //we need to get four of these
                                    uint16_t v = 0;
                                    if(i + 4 <= str.size())
                                    {
                                        for(size_t i2 = 0; i2 < 4; i2++,i++)
                                        {
                                            v |= HttpUtils::HexToNibble(str[i]) << ((3-i2) * 4);
                                        }
                                        

                                        uint32_t v2 = v;

                                        if((v & 0xFC00) == 0xD800) 
                                        {

                                            v2 = (v & 0x03FF) << 10;
                                            if(i + 6 <= str.size() && str.substr(i,2) == "\\u")
                                            {
                                                i+=2;
                                                v=0;

                                                for(size_t i2 = 0; i2 < 4; i2++, i++)
                                                {
                                                    v |= HttpUtils::HexToNibble(str[i]) << ((3-i2) * 4);
                                                }
                                                if((v & 0xFC00) != 0xDC00) 
                                                {
                                                    throw std::runtime_error("Not a lower utf-16 surrogate pair.");
                                                }
                                                

                                                v2 |= (v & 0x03FF);

                                                v2 += 0x10000;
                                            }
                                            else
                                            {
                                               
                                                throw std::runtime_error("Could not read lower utf-16 surrogate pair.");
                                            }
                                            if(v2 <= 0x7F)
                                            {
                                                str2.push_back((char)v2);
                                            }
                                            else if(v2 >= 0x80 && v2 <= 0x7FF)
                                            {
                                                uint8_t high = 0b11000000 | ((v2 >> 6) & 0b00011111);
                                                uint8_t low = 0b10000000 | (v2 & 0b00111111);
                                                str2.push_back((char)high);
                                                str2.push_back((char)low);
                                            }
                                            else if(v2 >= 0x800 && v2 <= 0xFFFF)
                                            {
                                                uint8_t highest = 0b11100000 | ((v2 >> 12) & 0b00001111);
                                                uint8_t high = 0b10000000 | ((v2 >> 6) & 0b00111111);
                                                uint8_t low = 0b10000000 | (v2 & 0b00111111);
                                                str2.push_back((char)highest);
                                                str2.push_back((char)high);
                                                str2.push_back((char)low);
                                            }
                                            else if(v2 >= 0x010000 && v2 <= 0x10FFFF)
                                            {
                                                uint8_t highest = 0b11110000 | ((v2 >> 18) & 0b00000111);
                                                uint8_t high = 0b10000000 | ((v2 >> 12) & 0b00111111);
                                                uint8_t low = 0b10000000 | ((v2 >> 6) & 0b00111111);
                                                uint8_t lowest = 0b10000000 | (v2 & 0b00111111);
                                                str2.push_back((char)highest);
                                                str2.push_back((char)high);
                                                str2.push_back((char)low);
                                                str2.push_back((char)lowest);
                                            }
                                        }
                                        

                                    }
                                }
                                else
                                {
                                        str2.push_back(str[i]);
                                }
                            }
                            else break;
                        }
                        else
                        {
                                str2.push_back(str[i]);
                        }
                    }
                    tokens.push_back(std::pair<std::string,bool>(str2,true));
                }
                    break;
                case ',':
                case '[':
                case ']':
                case '(':
                case ')':
                case '{':
                case '}':
                case ':':
                    flush();
                    tokens.push_back(std::pair<std::string,bool>(str.substr(i,1),false));
                    break;
                case ' ':
                case '\n':
                case '\r':
                case '\t':
                    flush();
                    break;
                default:
                    buff.push_back(str[i]);
                    break;

            }
        }

        flush();
        if(tokens.size() == 0) return JUndefined();

        size_t tokenIndex=0;

        std::function<std::pair<std::string,bool>()> pop_token=[&tokens,&tokenIndex]()->std::pair<std::string,bool> {
            if(tokenIndex >= tokens.size()) throw std::runtime_error("Json tokens out of bounds.");
            return tokens[tokenIndex++];
        };

        std::function<JToken()> read_token;
        read_token= [&pop_token,&read_token,&tokenIndex,&tokens]()->JToken{
            auto tkn = pop_token();
            if(tkn.second) 
            {
                return tkn.first;
            }
            else
            {
                if(tkn.first == "{")
                {
                        
                    JObject obj;
                    while(true)
                    {
                        begin_dict:
                        tkn = pop_token();
                        if(tkn.second)
                        {
                            std::string key = tkn.first;
                            tkn = pop_token();
                            if(tkn.second || tkn.first != ":") 
                            {
                                throw std::runtime_error("Invalid JSON key value seperator.");
                            }
                                
                            auto value = read_token();
                            obj.SetValue(key,value);
                        }
                        else if(tkn.first == ",")
                        {
                            goto begin_dict;
                        }
                        else if(tkn.first == "}")
                        {
                            break;
                        }   
                        else
                        {
                            
                            throw std::runtime_error("Invalid JSON object.");
                        }
                    }
                    return obj;
                }   
                else if(tkn.first == "[")
                {
                    JArray arr;
                    while(tokenIndex < tokens.size())
                    {
                        auto tkn = tokens[tokenIndex];
                        if(!tkn.second)
                        {
                            if(tkn.first == ",") tokenIndex++;
                            if(tkn.first == "]") {tokenIndex++; break;}
                        }
                        arr.Add(read_token());
                    }
                    return arr;
                }
                else if(tkn.first == "true")
                {
                    return true;
                }
                else if(tkn.first == "false")
                {
                    return false;
                }
                else if(tkn.first == "null")
                {
                    return nullptr;
                }
                else {
                        //number
                    if(tkn.first.find(".") != std::string::npos)
                    {
                        return std::stod(tkn.first);
                    }
                    else
                    {
                        return (int64_t)std::stoll(tkn.first);
                    }
                }
                    
            }
        };

        return read_token();
    }
    std::string Json::Encode(JToken tkn, bool indent)
    {
        int64_t i64;
        double f64;
        std::string str;
        bool b;
        JArray ls;
        JObject dict;
        if(std::holds_alternative<std::nullptr_t>(tkn)) return "null";
        if(TryGetJToken(tkn,b)) return b ? "true" : "false";
        if(TryGetJToken(tkn,f64))
        {
            auto str = std::to_string(f64);
                
            if(str.find(".") != std::string::npos)
            {
                size_t i;
                for(i = str.size()-1; str[i] != '.'; i--)
                {
                    if(str[i] != '0')
                    {
                        i++;
                        break;
                    }
                }
                return str.substr(0,i);
            }
            return str;
        }
        if(TryGetJToken(tkn,i64)) return std::to_string(i64);
        if(TryGetJToken(tkn,str))
        {
            std::string str2 = "\"";
            for(auto c : str)
            {
                if(c == '\"')
                {
                    str2.append("\\\"");
                }
                else if(c == '\n')
                {
                    str2.append("\\n");
                }
                else if(c == '\t')
                {
                    str2.append("\\t");
                }
                else if(c == '\r')
                {
                    str2.append("\\r");
                }
                else if(c == '/')
                {
                    str2.append("\\/");
                }
                else if(c == '\\')
                {
                    str2.append("\\\\");
                }
                else if(c == '\f')
                {
                    str2.append("\\f");
                }
                else if(c == '\b')
                {
                    str2.append("\\b");
                }
                else if((c >= 0 && c < 32) || c == 127 )
                {
                    str2.append("\\u00");
                    uint8_t c2 = (uint8_t)c;

                    str2.push_back(HttpUtils::NibbleToHex((c2 >> 4) & 0x0F));
                    str2.push_back(HttpUtils::NibbleToHex(c2 & 0x0F));
                }
                else
                {
                    str2.push_back(c);
                }
            }
            str2.push_back('\"');
            return str2;
        }
        if(TryGetJToken(tkn,dict))
        {

            std::string str="{";
            bool first=true;
            for(auto item : dict)
            {
                if(!first) {
                    str.push_back(',');
                }
                if(indent) {
                    str.append("\n");
                    str.append(tab(Encode(item.first,true) + ": " + Encode(item.second,true)));
                }
                else
                {
                    str.append(Encode(item.first,false)+":"+Encode(item.second,false));
                }
                first=false;
            }
            if(indent) str.append("\n");
            str.push_back('}');
            return str;
        }
        if(TryGetJToken(tkn,ls))
        {
            std::string str="[";
            bool first=true;
            for(auto item : ls)
            {
                    
                if(!first) {
                    str.push_back(',');
                }
                if(indent) {
                    str.append("\n");
                    str.append(tab(Encode(item,true)));
                }
                else
                {
                        str.append(Encode(item,false));
                }
                first=false;
            }
            if(indent) str.append("\n");
            str.push_back(']');
            return str;
        }
        return "";
    }

    JArray Json::DocDecode(std::string str)
    {
        JArray data;
        std::string builder = "";
        bool inSplit = false;
        size_t i=0;
        auto flush_builder = [&]()->void{
            if(builder.empty()) return;
            data.Add(builder);
            builder.clear();
        };
        auto consume_white_space = [&]()->void {
            for(; i < str.size(); i++)
            {
                switch(str[i])
                {
                    case '\t':
                    case '\r':
                    case '\n':
                    case ' ':
                        break;
                    default:
                        return;
                }
            }
        };
        auto is_keyword_of_and_consume = [&](std::string text)->bool {
            if(i + text.size() <= str.size()) {
                if(str.substr(i,text.size()) == text)
                {
                    i+=text.size();
                    return true;
                }
            }
            return false;
        };
        std::function<JToken()> json_decode_token;
        json_decode_token = [&]() -> JToken {
            if(i >= str.size()) return JUndefined();
            if(is_keyword_of_and_consume("true")) return true;
            if(is_keyword_of_and_consume("false")) return false;
            if(is_keyword_of_and_consume("null")) return nullptr;
            
            

            if(str[i] == '[')
            {
                i++;
                JArray array;
                //array
                while(i < str.size())
                {
                    consume_white_space();
                    if(i < str.size() && str[i] ==',')
                    {
                        i++;
                    }

                    consume_white_space();
                    if(i < str.size() && str[i] ==']')
                    {
                        i++;
                        return array;
                    }
                    auto val = json_decode_token();
                    array.Add(val);
                }   
                return array;
            }
            if(str[i] == '{')
            {
                //dictionary
                i++;
                JObject dict;
                while(i < str.size())
                {
                    consume_white_space();
                    if(i < str.size() && str[i] ==',')
                    {
                        i++;
                    }

                    consume_white_space();
                    if(i < str.size() && str[i] =='}')
                    {
                        i++;
                        return dict;
                    }
                    auto key = json_decode_token();
                    consume_white_space();
                    if(i < str.size() && str[i] ==':')
                    {
                        i++;
                    }
                    consume_white_space();
                    auto value = json_decode_token();
                    std::string keyStr;
                    if(TryGetJToken(key,keyStr) && !std::holds_alternative<JUndefined>(value))
                    {
                        dict.SetValue(keyStr,value);   
                    }
                }   
                return dict;
            }
            if(str[i] == '\"')
            {
                i++;
                std::string str2 = "";
                //string
                for(; i < str.size() && str[i] != '\"'; i++)
                    {
                        if(str[i] == '\\')
                        {
                            i++;
                            if(i < str.size())
                            {
                                if(str[i] == 'n')
                                {
                                    str2.push_back('\n');
                                }
                                else if(str[i] == 'r')
                                {
                                    str2.push_back('\r');
                                }
                                else if(str[i] == 't')
                                {
                                    str2.push_back('\t');
                                }
                                else if(str[i] == 'f')
                                {
                                    str2.push_back('\f');
                                }
                                else if(str[i] == 'b')
                                {
                                    str2.push_back('\b');
                                }
                                else if(str[i] == 'u')
                                {
                                    i++;
                                    //we need to get four of these
                                    uint16_t v = 0;
                                    if(i + 4 <= str.size())
                                    {
                                        for(size_t i2 = 0; i2 < 4; i2++,i++)
                                        {
                                            v |= HttpUtils::HexToNibble(str[i]) << ((3-i2) * 4);
                                        }
                                        

                                        uint32_t v2 = v;

                                        if((v & 0xFC00) == 0xD800) 
                                        {

                                            v2 = (v & 0x03FF) << 10;
                                            if(i + 6 <= str.size() && str.substr(i,2) == "\\u")
                                            {
                                                i+=2;
                                                v=0;

                                                for(size_t i2 = 0; i2 < 4; i2++, i++)
                                                {
                                                    v |= HttpUtils::HexToNibble(str[i]) << ((3-i2) * 4);
                                                }
                                                if((v & 0xFC00) != 0xDC00) 
                                                {
                                                    throw std::runtime_error("Not a lower utf-16 surrogate pair.");
                                                }
                                                

                                                v2 |= (v & 0x03FF);

                                                v2 += 0x10000;
                                            }
                                            else
                                            {
                                               
                                                throw std::runtime_error("Could not read lower utf-16 surrogate pair.");
                                            }
                                            if(v2 <= 0x7F)
                                            {
                                                str2.push_back((char)v2);
                                            }
                                            else if(v2 >= 0x80 && v2 <= 0x7FF)
                                            {
                                                uint8_t high = 0b11000000 | ((v2 >> 6) & 0b00011111);
                                                uint8_t low = 0b10000000 | (v2 & 0b00111111);
                                                str2.push_back((char)high);
                                                str2.push_back((char)low);
                                            }
                                            else if(v2 >= 0x800 && v2 <= 0xFFFF)
                                            {
                                                uint8_t highest = 0b11100000 | ((v2 >> 12) & 0b00001111);
                                                uint8_t high = 0b10000000 | ((v2 >> 6) & 0b00111111);
                                                uint8_t low = 0b10000000 | (v2 & 0b00111111);
                                                str2.push_back((char)highest);
                                                str2.push_back((char)high);
                                                str2.push_back((char)low);
                                            }
                                            else if(v2 >= 0x010000 && v2 <= 0x10FFFF)
                                            {
                                                uint8_t highest = 0b11110000 | ((v2 >> 18) & 0b00000111);
                                                uint8_t high = 0b10000000 | ((v2 >> 12) & 0b00111111);
                                                uint8_t low = 0b10000000 | ((v2 >> 6) & 0b00111111);
                                                uint8_t lowest = 0b10000000 | (v2 & 0b00111111);
                                                str2.push_back((char)highest);
                                                str2.push_back((char)high);
                                                str2.push_back((char)low);
                                                str2.push_back((char)lowest);
                                            }
                                        }
                                        

                                    }
                                }
                                else
                                {
                                        str2.push_back(str[i]);
                                }
                            }
                            else break;
                        }
                        else
                        {
                                str2.push_back(str[i]);
                        }
                    }
                i++;
                return str2;
            }
            if((str[i] >= '0' && str[i] <= '9' ) || str[i] == '-')
            {
                //number
                std::string noStr = "";
                bool hasPt = false;
                bool hasExponent=false;
                bool hasNeg = false;
                for(;i<str.size(); i++)
                {
                    if(str[i] == 'E' || str[i] == 'e') 
                    {
                        if(hasExponent) break;
                        hasExponent=true;
                        hasNeg=false;
                        noStr += str[i];
                    }
                    else if(str[i] == '.')
                    {
                        if(hasPt) break;
                        hasPt=true;
                        noStr += str[i];
                    }
                    else if(str[i] == '-')
                    {
                        if(hasNeg) break;
                        hasNeg = true;
                        noStr += str[i];
                    }
                    else if(str[i] == '+')
                    {
                        if(!hasExponent) break;
                        if(hasNeg) break;
                        hasNeg=true;
                        noStr += str[i];
                    }
                    else if(str[i] >= '0' && str[i] <= '9')
                    {
                        noStr += str[i];
                    }
                    else break;
                }
                if(noStr.find(".") != std::string::npos)
                {
                    return std::stod(noStr);
                }
                else
                {
                    return (int64_t)std::stoll(noStr);
                }
            }
            return JUndefined();
        };


        for(; i < str.size(); i++)
        {
            if(inSplit)
            {
                consume_white_space();
                std::string key = "";
                for(; i < str.size() && str[i] != ' '; i++)
                {
                    key += str[i];
                }
                consume_white_space();
                auto value = json_decode_token();
                if(!std::holds_alternative<JUndefined>(value))
                {
                    data.Add(JObject({
                        JOItem("key",key),
                        JOItem("value", value)
                    }));
                }
                inSplit=false;
                i--;
            }
            else {
                if(str[i] == '@')
                {
                    if(i+1<str.size() && str[i+1] == '@')
                    {
                        i++;
                        builder.push_back('@');
                        continue;
                    }
                    inSplit=true;
                    flush_builder();
                    continue;
                }
                builder.push_back(str[i]);
            }
        }
        flush_builder();
        return data;
    }
    std::string Json::DocEncode(JArray array,bool indent)
    {
        /*
            The beautiful text
            @name "John"
            @list ["A","B"]
            @number 42
            @dict {"a": 5, "b": true}
        */

        /*
            [
                "\tThe beautiful text\n\t",
                {
                    "key": "name",
                    "value": ["A","B"]
                },
                "\n\t",
                {
                    "key": "number",
                    "value": 42
                },
                "\n\t",
                {
                    "key": "dict",
                    "value": {"a": 5, "b": true}
                },
                "\n"
            ]
        */
        std::string text = "";

        for(auto& item : array.GetVector())
        {
            std::string str;
            JObject obj;
            if(TryGetJToken(item,str)) text += HttpUtils::Replace(str,"@","@@");
            if(TryGetJToken(item,obj))
            {
                auto value = obj.GetValue("value");
                if(obj.TryGetValueAsType("key", str) && !std::holds_alternative<JUndefined>(value))
                {
                    text += "@" + str + " " + Json::Encode(value,indent);
                }
            }
        }

        return text;
    }
}
