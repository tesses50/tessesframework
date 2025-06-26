#include "TessesFramework/Text/StringConverter.hpp"

namespace Tesses::Framework::Text::StringConverter {
	void UTF8::FromUTF16(std::basic_string<char>& utf8, const std::basic_string<char16_t>& utf16)
	{
		for (size_t i=0; i < utf16.size();i++)
		{
			char32_t c = utf16[i];
			if ((c & 0xFC00) == 0xD800)
			{
				c = (c & 0x03FF) << 10;
				i++;
				if (i >= utf16.size()) return;


				char32_t c2 = utf16[i];
				if ((c2 & 0xFC00) != 0xDC00) continue;


				c |= (c2 & 0x03FF);

				c += 0x10000;
			}

            if (c <= 0x7F)
            {
                utf8.push_back((char)c);
            }
            else if (c >= 0x80 && c <= 0x7FF)
            {
                uint8_t high = 0b11000000 | ((c >> 6) & 0b00011111);
                uint8_t low = 0b10000000 | (c & 0b00111111);
                utf8.push_back((char)high);
                utf8.push_back((char)low);
            }
            else if (c >= 0x800 && c <= 0xFFFF)
            {
                uint8_t highest = 0b11100000 | ((c >> 12) & 0b00001111);
                uint8_t high = 0b10000000 | ((c >> 6) & 0b00111111);
                uint8_t low = 0b10000000 | (c & 0b00111111);
                utf8.push_back((char)highest);
                utf8.push_back((char)high);
                utf8.push_back((char)low);
            }
            else if (c >= 0x010000 && c <= 0x10FFFF)
            {
                uint8_t highest = 0b11110000 | ((c >> 18) & 0b00000111);
                uint8_t high = 0b10000000 | ((c >> 12) & 0b00111111);
                uint8_t low = 0b10000000 | ((c >> 6) & 0b00111111);
                uint8_t lowest = 0b10000000 | (c & 0b00111111);
                utf8.push_back((char)highest);
                utf8.push_back((char)high);
                utf8.push_back((char)low);
                utf8.push_back((char)lowest);
            }
		
        }
	}
    void UTF8::FromUTF32(std::basic_string<char>& utf8, const std::basic_string<char32_t>& utf32)
    {
        for (auto c : utf32)
        {
            if (c <= 0x7F)
            {
                utf8.push_back((char)c);
            }
            else if (c >= 0x80 && c <= 0x7FF)
            {
                uint8_t high = 0b11000000 | ((c >> 6) & 0b00011111);
                uint8_t low = 0b10000000 | (c & 0b00111111);
                utf8.push_back((char)high);
                utf8.push_back((char)low);
            }
            else if (c >= 0x800 && c <= 0xFFFF)
            {
                uint8_t highest = 0b11100000 | ((c >> 12) & 0b00001111);
                uint8_t high = 0b10000000 | ((c >> 6) & 0b00111111);
                uint8_t low = 0b10000000 | (c & 0b00111111);
                utf8.push_back((char)highest);
                utf8.push_back((char)high);
                utf8.push_back((char)low);
            }
            else if (c >= 0x010000 && c <= 0x10FFFF)
            {
                uint8_t highest = 0b11110000 | ((c >> 18) & 0b00000111);
                uint8_t high = 0b10000000 | ((c >> 12) & 0b00111111);
                uint8_t low = 0b10000000 | ((c >> 6) & 0b00111111);
                uint8_t lowest = 0b10000000 | (c & 0b00111111);
                utf8.push_back((char)highest);
                utf8.push_back((char)high);
                utf8.push_back((char)low);
                utf8.push_back((char)lowest);
            }

        }
    }

    void UTF16::FromUTF8(std::basic_string<char16_t>& utf16, const std::basic_string<char>& utf8)
    {
        for (size_t i = 0; i < utf8.size();i++)
        {
            uint8_t c = (uint8_t)utf8[i];
            char32_t cres = 0;
            if (c <= 127)
            {
                cres = (char32_t)c;
            }
            else if ((c & 0b11100000) == 0b11000000)
            {
                if (i + 1 < utf8.size())
                {

                    uint8_t c2 = (uint8_t)utf8[++i];
                    cres |= c2 & 0b00111111;
                    cres |= (c & 0b00011111) << 6;

                }
                else {
                    i++;
                    continue;
                };
            }
            else if ((c & 0b11110000) == 0b11100000)
            {
                if (i + 2 < utf8.size())
                {
                    uint8_t c2 = (uint8_t)utf8[++i];
                    uint8_t c3 = (uint8_t)utf8[++i];
                    cres |= c3 & 0b00111111;
                    cres |= (c2 & 0b00111111) << 6;
                    cres |= (c & 0b00001111) << 12;

                }
                else { i += 2; continue; }
            }
            else if ((c & 0b11111000) == 0b11110000)
            {
                if (i + 3 < utf8.size())
                {
                    uint8_t c2 = (uint8_t)utf8[++i];
                    uint8_t c3 = (uint8_t)utf8[++i];
                    uint8_t c4 = (uint8_t)utf8[++i];

                    cres |= c4 & 0b00111111;
                    cres |= (c3 & 0b00111111) << 6;
                    cres |= (c2 & 0b00111111) << 12;
                    cres |= (c & 0b00000111) << 18;

                }
                else { i += 3; continue; }
            }
            if (cres >= 0x10000 && cres <= 0x10FFFF)
            {
                auto subtracted = cres - 0x10000;

                auto high = (0x3FF & (subtracted >> 10)) | 0xD800;
                auto low = (0x3FF & subtracted) | 0xDC00;

                utf16.push_back(high);
                utf16.push_back(low);
            }
            else {
                utf16.push_back((char16_t)cres);
            }

        }
    }
    void UTF16::FromUTF32(std::basic_string<char16_t>& utf16, const std::basic_string<char32_t>& utf32)
    {
        for (auto cres : utf32)
        {
            if (cres >= 0x10000 && cres <= 0x10FFFF)
            {
                auto subtracted = cres - 0x10000;

                auto high = (0x3FF & (subtracted >> 10)) | 0xD800;
                auto low = (0x3FF & subtracted) | 0xDC00;

                utf16.push_back(high);
                utf16.push_back(low);
            }
            else {
                utf16.push_back((char16_t)cres);
            }
        }
    }

    void UTF32::FromUTF8(std::basic_string<char32_t>& utf32, const std::basic_string<char>& utf8)
    {
        for (size_t i = 0; i < utf8.size();i++)
        {
            uint8_t c = (uint8_t)utf8[i];
            char32_t cres = 0;
            if (c <= 127)
            {
                cres=(char32_t)c;
            }
            else if ((c & 0b11100000) == 0b11000000)
            {
                if (i + 1 < utf8.size())
                {
                    
                    uint8_t c2 = (uint8_t)utf8[++i];
                    cres |= c2 & 0b00111111;
                    cres |= (c & 0b00011111) << 6;
                    
                }
                else {
                    i++;
                    continue;
                };
            }
            else if ((c & 0b11110000) == 0b11100000)
            {
                if (i + 2 < utf8.size())
                {
                    uint8_t c2 = (uint8_t)utf8[++i];
                    uint8_t c3 = (uint8_t)utf8[++i];
                    cres |= c3 & 0b00111111;
                    cres |= (c2 & 0b00111111) << 6;
                    cres |= (c & 0b00001111) << 12;
                    
                }
                else { i += 2; continue; }
            }
            else if ((c & 0b11111000) == 0b11110000)
            {
                if (i + 3 < utf8.size())
                {
                    uint8_t c2 = (uint8_t)utf8[++i];
                    uint8_t c3 = (uint8_t)utf8[++i];
                    uint8_t c4 = (uint8_t)utf8[++i];
                    
                    cres |= c4 & 0b00111111;
                    cres |= (c3 & 0b00111111) << 6;
                    cres |= (c2 & 0b00111111) << 12;
                    cres |= (c & 0b00000111) << 18;
                    
                }
                else { i += 3; continue; }
            }
            utf32.push_back(cres);
        }

    }

    void UTF32::FromUTF16(std::basic_string<char32_t>& utf32, const std::basic_string<char16_t>& utf16)
    {
        for (size_t i = 0; i < utf16.size();i++)
        {
            char32_t c = utf16[i];
            if ((c & 0xFC00) == 0xD800)
            {
                c = (c & 0x03FF) << 10;
                i++;
                if (i >= utf16.size()) return;


                char32_t c2 = utf16[i];
                if ((c2 & 0xFC00) != 0xDC00) continue;


                c |= (c2 & 0x03FF);

                c += 0x10000;
            }
            utf32.push_back(c);
        }
    }
}