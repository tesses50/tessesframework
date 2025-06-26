#pragma once
#include <string>
#include <cstdint>
namespace Tesses::Framework::Text::StringConverter {
	class UTF8 {
	public:
		static void FromUTF16(std::basic_string<char>& utf8, const std::basic_string<char16_t>& utf16);
		static void FromUTF32(std::basic_string<char>& utf8, const std::basic_string<char32_t>& utf32);
	};
	class UTF16 {
	public:
		static void FromUTF8(std::basic_string<char16_t>& utf16, const std::basic_string<char>& utf8);
		static void FromUTF32(std::basic_string<char16_t>& utf16, const std::basic_string<char32_t>& utf32);
	};
	class UTF32 {
	public:
		static void FromUTF8(std::basic_string<char32_t>& utf32, const std::basic_string<char>& utf8);
		static void FromUTF16(std::basic_string<char32_t>& utf32, const std::basic_string<char16_t>& utf16);
	};
}