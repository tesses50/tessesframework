/*
    TessesFramework a library to make C++ easier for me, used in CrossLang:
   https://git.tesses.org/tesses50/crosslang Copyright (C) 2026 Mike Nolan

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

#include "TessesFramework/Serialization/Bencode.hpp"

namespace Tesses::Framework::Serialization::Bencode {
BeToken BeDictionary::GetValue(BeString key) const {
    for (auto item : this->tokens)
        if (item.first == key)
            return item.second;
    return BeUndefined();
}
void BeDictionary::SetValue(BeString key, BeToken value) {
    if (std::holds_alternative<BeUndefined>(value)) {
        for (auto idx = this->tokens.begin(); idx != this->tokens.end();
             idx++) {
            if (idx->first == key) {
                this->tokens.erase(idx);
                break;
            }
        }
    } else {
        for (auto &item : this->tokens) {
            if (item.first == key) {
                item.second = value;
                return;
            }
        }
        this->tokens.emplace_back(key, value);
    }
}

BeString::BeString() {}
BeString::BeString(const std::string &text) {
    this->data.insert(this->data.end(), text.cbegin(), text.cend());
}
BeString::BeString(const char *text) {
    size_t len = strlen(text);
    this->data.insert(this->data.end(), text, text + len);
}
BeString::BeString(const std::vector<uint8_t> &data) : data(data) {}

BeString::operator std::string() const {
    return std::string(data.cbegin(), data.cend());
}

bool operator==(const BeString &lStr, const BeString &rStr) {
    return lStr.data == rStr.data;
}
bool operator==(const BeString &lStr, const std::string &rStr) {
    if (lStr.data.size() != rStr.size())
        return false;
    return std::equal(lStr.data.cbegin(), lStr.data.cend(), rStr.cbegin());
}
bool operator==(const std::string &lStr, const BeString &rStr) {
    if (lStr.size() != rStr.data.size())
        return false;
    return std::equal(lStr.cbegin(), lStr.cend(), rStr.data.cbegin());
}
bool operator==(const BeString &lStr, const char *rStr) {
    size_t len = strlen(rStr);
    if (lStr.data.size() != len)
        return false;
    return std::equal(lStr.data.cbegin(), lStr.data.cend(), rStr);
}
bool operator==(const char *lStr, const BeString &rStr) {
    size_t len = strlen(lStr);
    if (rStr.data.size() != len)
        return false;

    return std::equal(lStr, lStr + len, rStr.data.cbegin());
}

bool operator!=(const BeString &lStr, const BeString &rStr) {
    return !(lStr == rStr);
}
bool operator!=(const BeString &lStr, const std::string &rStr) {
    return !(lStr == rStr);
}
bool operator!=(const std::string &lStr, const BeString &rStr) {
    return !(lStr == rStr);
}
bool operator!=(const BeString &lStr, const char *rStr) {
    return !(lStr == rStr);
}
bool operator!=(const char *lStr, const BeString &rStr) {
    return !(lStr == rStr);
}

void Bencode::Save(std::shared_ptr<Tesses::Framework::Streams::Stream> strm,
                   const BeToken &value) {
    if (std::holds_alternative<BeArray>(value)) {
        auto &array = std::get<BeArray>(value);
        strm->WriteByte((uint8_t)'l');
        for (auto &item : array.tokens) {
            Save(strm, item);
        }
        strm->WriteByte((uint8_t)'e');
    } else if (std::holds_alternative<BeDictionary>(value)) {
        auto &dict = std::get<BeDictionary>(value);
        strm->WriteByte((uint8_t)'d');
        for (auto &item : dict.tokens) {
            Save(strm, item.first);
            Save(strm, item.second);
        }
        strm->WriteByte((uint8_t)'e');
    } else if (std::holds_alternative<BeString>(value)) {
        auto &str = std::get<BeString>(value);
        std::string prefix = std::to_string(str.data.size()) + ":";
        strm->WriteBlock((const uint8_t *)prefix.data(), prefix.size());
        strm->WriteBlock(str.data.data(), str.data.size());
    } else if (std::holds_alternative<int64_t>(value)) {
        int64_t val = std::get<int64_t>(value);
        std::string str = "i" + std::to_string(val) + "e";
        strm->WriteBlock((const uint8_t *)str.data(), str.size());
    }
}
BeToken
Bencode::Load(std::shared_ptr<Tesses::Framework::Streams::Stream> strm) {
    auto chr = strm->ReadByte();
    switch (chr) {
    case 'i': {
        std::string no;
        while (true) {
            chr = strm->ReadByte();
            if (chr == -1)
                throw std::out_of_range("End of file");
            if (chr == 'e')
                break;
            no.push_back((char)chr);
        }
        return std::stoll(no);
    } break;
    case 'd': {
        BeDictionary dict;
        while (true) {
            auto key = Load(strm);
            if (std::holds_alternative<BeUndefined>(key))
                break;
            if (!std::holds_alternative<BeString>(key))
                throw std::runtime_error("Key must be a string");
            auto value = Load(strm);
            if (std::holds_alternative<BeUndefined>(key))
                throw std::runtime_error("Incomplete dictionary entry");
            dict.tokens.emplace_back(std::get<BeString>(key), value);
        }
        return dict;
    } break;
    case 'l': {
        BeArray array;
        while (true) {
            auto tkn = Load(strm);
            if (std::holds_alternative<BeUndefined>(tkn))
                break;
            array.tokens.push_back(tkn);
        }
        return array;
    } break;
    case 'e':
        return BeUndefined();
    case -1:
        throw std::out_of_range("End of file");
    default: {
        std::string no({(char)chr});
        while (true) {
            chr = strm->ReadByte();
            if (chr == -1)
                throw std::out_of_range("End of file");
            if (chr == ':')
                break;
            no.push_back((char)chr);
        }
        auto len = std::stoll(no);
        if (len < 0)
            throw std::out_of_range("Less than zero byte string");
        BeString str;
        str.data.resize((size_t)len);

        size_t result = strm->ReadBlock(str.data.data(), str.data.size());
        if (result != str.data.size() || result != (size_t)len)
            throw std::out_of_range("Didn't read entire string");
        return str;

    } break;
    }
}
Json::JToken Bencode::ToJson(const BeToken &tkn) {
    if (std::holds_alternative<BeDictionary>(tkn)) {
        auto &dict = std::get<BeDictionary>(tkn);
        Json::JObject o;
        for (auto &itm : dict.tokens) {
            o.SetValue(itm.first, ToJson(itm.second));
        }
        return o;
    }
    if (std::holds_alternative<BeArray>(tkn)) {
        auto &array = std::get<BeArray>(tkn);
        Json::JArray a;
        for (auto &itm : array.tokens) {
            a.Add(ToJson(itm));
        }
        return a;
    }
    if (std::holds_alternative<BeString>(tkn)) {
        return (std::string)std::get<BeString>(tkn);
    }
    if (std::holds_alternative<int64_t>(tkn)) {
        return std::get<int64_t>(tkn);
    }
    return Json::JUndefined();
}
void Bencode::Print(
    std::shared_ptr<Tesses::Framework::TextStreams::TextWriter> writer,
    BeToken tkn) {
    writer->WriteLine(Json::Json::Encode(ToJson(tkn), true));
}
} // namespace Tesses::Framework::Serialization::Bencode