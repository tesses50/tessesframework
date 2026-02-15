#pragma once
#include "Common.hpp"

namespace Tesses::Framework {
    enum class UuidStringifyConfig {
        IsUppercase=0b001,
        HasCurly=0b010,
        HasDashes=0b100,
        UppercaseCompact = IsUppercase,
        LowercaseCompact = 0,
        UppercaseNoCurly = IsUppercase | HasDashes,
        LowercaseNoCurly = HasDashes,
        UppercaseCurly = IsUppercase | HasDashes | HasCurly,
        LowercaseCurly = HasDashes | HasCurly
    };
    struct Uuid {
        Uuid() = default;
        uint32_t time_low = 0;
        uint16_t time_mid = 0;
        uint16_t time_hi_and_version = 0;
        uint8_t  clock_seq_hi_and_reserved = 0;
        uint8_t  clock_seq_low = 0;
        uint8_t  node[6] = {0,0,0,0,0,0};
        
        static Uuid Generate();
        static void Generate(Uuid& uuid);

        static bool TryParse(std::string text, Uuid& uuid);

        std::string ToString(UuidStringifyConfig cfg = UuidStringifyConfig::UppercaseCurly) const;

        bool IsEmpty() const;
    };

    bool operator==(const Uuid& left, const Uuid& right);
    bool operator!=(const Uuid& left, const Uuid& right);
}