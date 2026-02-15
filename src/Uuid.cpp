#include "TessesFramework/Uuid.hpp"
#include "TessesFramework/Http/HttpUtils.hpp"
#include "TessesFramework/Crypto/Crypto.hpp"

namespace Tesses::Framework {
    Uuid Uuid::Generate()
    {
        //xxxxxxxx-xxxx-4xxx-yxxx-xxxxxxxxxxxx
        Uuid uuid;
        Uuid::Generate(uuid);
        return uuid;
    }

    void Uuid::Generate(Uuid& uuid)
    {
        std::vector<uint8_t> bytes(16);
        Crypto::RandomBytes(bytes, "TF_UUID");

        uuid.time_low = (uint32_t)bytes[0];
        uuid.time_low |= (uint32_t)bytes[1] << 8;
        uuid.time_low |= (uint32_t)bytes[2] << 16;
        uuid.time_low |= (uint32_t)bytes[3] << 24;
        uuid.time_mid = (uint16_t)bytes[4];
        uuid.time_mid |= (uint16_t)bytes[5] << 8;
        uuid.time_hi_and_version = (uint16_t)bytes[6];
        uuid.time_hi_and_version |= (uint16_t)bytes[7] << 8;
        uuid.clock_seq_hi_and_reserved = bytes[8];
        uuid.clock_seq_low = bytes[9];
        for(size_t i = 0; i < 6; i++)
        {
            uuid.node[i] = bytes[i+10];
        }
        uuid.time_hi_and_version &= ~0x00F0;
        uuid.time_hi_and_version |= 0x0040;
        uuid.clock_seq_hi_and_reserved &= ~0b11000000;
        uuid.clock_seq_hi_and_reserved |=  0b10000000;


    }

    bool Uuid::TryParse(std::string text, Uuid& uuid)
    {
        std::array<uint8_t,32> hex_digits;
        size_t hex_offset = 0;
        size_t text_offset = 0;
        for(; text_offset < text.size(); text_offset++)
        {
            if(text[text_offset] == '{' && (text_offset != 0 || hex_offset != 0))
                return false;
            if(text[text_offset] == '}' && hex_offset < 32)
                return false;
            if(text[text_offset] == '-' && hex_offset != 8 && hex_offset != 12 && hex_offset != 16 && hex_offset != 20)
                return false;
            if((text[text_offset] >= 'A' && text[text_offset] <= 'F') || (text[text_offset] >= 'a' && text[text_offset] <= 'f') || text[text_offset] >= '0' && text[text_offset] <= '9')
            {
                if(hex_offset >= 32) return false;
                hex_digits[hex_offset] = Http::HttpUtils::HexToNibble(text[text_offset]);
                hex_offset++;
            }
            else return false;

        }
        
        uint8_t b = hex_digits[0] << 4 | hex_digits[1];
        uuid.time_low = (uint32_t)b;
        b = hex_digits[2] << 4 | hex_digits[3];
        uuid.time_low |= (uint32_t)b << 8;
        b = hex_digits[4] << 4 | hex_digits[5];
        uuid.time_low |= (uint32_t)b << 16;
        b = hex_digits[6] << 4 | hex_digits[7];
        uuid.time_low |= (uint32_t)b << 24;

        b = hex_digits[8] << 4 | hex_digits[9];
        uuid.time_mid = (uint16_t)b;
        b = hex_digits[10] << 4 | hex_digits[11];
        uuid.time_mid |= (uint16_t)b << 8;

        b = hex_digits[12] << 4 | hex_digits[13];
        uuid.time_hi_and_version = (uint16_t)b;
        b = hex_digits[14] << 4 | hex_digits[15];
        uuid.time_hi_and_version |= (uint16_t)b << 8;

        uuid.clock_seq_hi_and_reserved = hex_digits[16] << 4 | hex_digits[17];

        uuid.clock_seq_low = hex_digits[18] << 4 | hex_digits[19];

        for(size_t i = 0; i < 6; i++)
        {
            uuid.node[i] = hex_digits[20+(i*2)] << 4 | hex_digits[21+(i*2)];
        }

        return true;
    }
    //9c4994e7-3c82-4c30-a459-8fdcd960b4ac

    std::string Uuid::ToString(UuidStringifyConfig cfg) const
    {
        bool hasCurly = ((int)cfg & (int)UuidStringifyConfig::HasCurly) != 0;
        bool isUppercase = ((int)cfg & (int)UuidStringifyConfig::IsUppercase) != 0;
        bool hasDash = ((int)cfg & (int)UuidStringifyConfig::HasDashes) != 0;
        
        std::string uuid_str = ""; 
        if(hasCurly)
            uuid_str += "{";

        uint8_t byte = (uint8_t)(this->time_low & 0xFF);
                
        uuid_str += Http::HttpUtils::NibbleToHex(byte>>4,isUppercase);
        uuid_str += Http::HttpUtils::NibbleToHex(byte,isUppercase);
        byte = (uint8_t)((this->time_low >> 8) & 0xFF);
        uuid_str += Http::HttpUtils::NibbleToHex(byte>>4,isUppercase);
        uuid_str += Http::HttpUtils::NibbleToHex(byte,isUppercase);
        byte = (uint8_t)((this->time_low >> 16) & 0xFF);
        uuid_str += Http::HttpUtils::NibbleToHex(byte>>4,isUppercase);
        uuid_str += Http::HttpUtils::NibbleToHex(byte,isUppercase);
        byte = (uint8_t)((this->time_low >> 24) & 0xFF);
        uuid_str += Http::HttpUtils::NibbleToHex(byte>>4,isUppercase);
        uuid_str += Http::HttpUtils::NibbleToHex(byte,isUppercase);

        if(hasDash)
            uuid_str += "-";

        byte = (uint8_t)(this->time_mid & 0xFF);
                
        uuid_str += Http::HttpUtils::NibbleToHex(byte>>4,isUppercase);
        uuid_str += Http::HttpUtils::NibbleToHex(byte,isUppercase);

        byte = (uint8_t)((this->time_mid >> 8) & 0xFF);
                
        uuid_str += Http::HttpUtils::NibbleToHex(byte>>4,isUppercase);
        uuid_str += Http::HttpUtils::NibbleToHex(byte,isUppercase);

        if(hasDash)
            uuid_str += "-";

        byte = (uint8_t)(this->time_hi_and_version & 0xFF);
                
        uuid_str += Http::HttpUtils::NibbleToHex(byte>>4,isUppercase);
        uuid_str += Http::HttpUtils::NibbleToHex(byte,isUppercase);

        byte = (uint8_t)((this->time_hi_and_version >> 8) & 0xFF);
                
        uuid_str += Http::HttpUtils::NibbleToHex(byte>>4,isUppercase);
        uuid_str += Http::HttpUtils::NibbleToHex(byte,isUppercase);
        if(hasDash)
            uuid_str += "-";
        
        uuid_str += Http::HttpUtils::NibbleToHex(this->clock_seq_hi_and_reserved>>4,isUppercase);
        uuid_str += Http::HttpUtils::NibbleToHex(this->clock_seq_hi_and_reserved,isUppercase);
        uuid_str += Http::HttpUtils::NibbleToHex(this->clock_seq_low>>4,isUppercase);
        uuid_str += Http::HttpUtils::NibbleToHex(this->clock_seq_low,isUppercase);
        if(hasDash)
            uuid_str += "-";

        for(size_t i = 0; i < 6; i++)
        {
            byte = this->node[i];
            uuid_str += Http::HttpUtils::NibbleToHex(byte>>4,isUppercase);
            uuid_str += Http::HttpUtils::NibbleToHex(byte,isUppercase);
        }

        if(hasCurly)
            uuid_str += "}";
        return uuid_str;
            
            
        
    }

    bool Uuid::IsEmpty() const
    {
        return this->time_low == 0 && 
            this->time_mid == 0 &&
            this->time_hi_and_version == 0 &&
            this->clock_seq_hi_and_reserved == 0 &&
            this->clock_seq_low == 0 &&
            this->node[0] == 0 &&
            this->node[1] == 0 &&
            this->node[2] == 0 &&
            this->node[3] == 0 &&
            this->node[4] == 0 &&
            this->node[5] == 0;
    }

    bool operator==(const Uuid& left, const Uuid& right)
    {
        return left.time_low == right.time_low &&
            left.time_mid == right.time_mid &&
            left.time_hi_and_version == right.time_hi_and_version &&
            left.clock_seq_hi_and_reserved == right.clock_seq_hi_and_reserved &&
            left.clock_seq_low == right.clock_seq_low &&
            left.node[0] == right.node[0] &&
            left.node[1] == right.node[1] &&
            left.node[2] == right.node[2] &&
            left.node[3] == right.node[3] &&
            left.node[4] == right.node[4] &&
            left.node[5] == right.node[5];
            
    }
    bool operator!=(const Uuid& left, const Uuid& right)
    {
        return left.time_low != right.time_low &&
            left.time_mid != right.time_mid &&
            left.time_hi_and_version != right.time_hi_and_version &&
            left.clock_seq_hi_and_reserved != right.clock_seq_hi_and_reserved &&
            left.clock_seq_low != right.clock_seq_low &&
            left.node[0] != right.node[0] &&
            left.node[1] != right.node[1] &&
            left.node[2] != right.node[2] &&
            left.node[3] != right.node[3] &&
            left.node[4] != right.node[4] &&
            left.node[5] != right.node[5];
    }
}