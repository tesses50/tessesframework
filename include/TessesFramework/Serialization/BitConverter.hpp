#pragma once
#include "../Common.hpp"
#include "../Uuid.hpp"
namespace Tesses::Framework::Serialization
{
    
/**
 * @brief A bit converter
 * 
 */
class BitConverter {
    public:
 
        static double ToDoubleBits(uint64_t v);
  
        static uint64_t ToUintBits(double v);
        static float ToFloatBits(uint32_t v);
  
        static uint32_t ToUint32Bits(float v);
   
        static double ToDoubleBE(uint8_t& b);
        static float ToFloatBE(uint8_t& b);
       
        static uint64_t ToUint64BE(uint8_t& b);
        static uint32_t ToUint32BE(uint8_t& b);
        static uint16_t ToUint16BE(uint8_t& b);

        static double ToDoubleLE(uint8_t& b);
        static float ToFloatLE(uint8_t& b);
        static uint64_t ToUint64LE(uint8_t& b);
        static uint32_t ToUint32LE(uint8_t& b);
        static uint16_t ToUint16LE(uint8_t& b);

        static int64_t ToSint64BE(uint8_t& b);
        static int32_t ToSint32BE(uint8_t& b);
        static int16_t ToSint16BE(uint8_t& b);
        static int64_t ToSint64LE(uint8_t& b);
        static int32_t ToSint32LE(uint8_t& b);
        static int16_t ToSint16LE(uint8_t& b);

        static Uuid ToUuid(uint8_t& b);


        static void ToUuid(uint8_t& b, Uuid& uuid);


        static void FromDoubleBE(uint8_t& b, double v);
        static void FromFloatBE(uint8_t& b, float v);
        static void FromUint64BE(uint8_t& b, uint64_t v);
        static void FromUint32BE(uint8_t& b, uint32_t v);
        static void FromUint16BE(uint8_t& b, uint16_t v);

        static void FromDoubleLE(uint8_t& b, double v);
        static void FromFloatLE(uint8_t& b, float v);
        static void FromUint64LE(uint8_t& b, uint64_t v);
        static void FromUint32LE(uint8_t& b, uint32_t v);
        static void FromUint16LE(uint8_t& b, uint16_t v);
        
        static void FromSint64BE(uint8_t& b, int64_t v);
        static void FromSint32BE(uint8_t& b, int32_t v);
        static void FromSint16BE(uint8_t& b, int16_t v);
        
        static void FromSint64LE(uint8_t& b, int64_t v);
        static void FromSint32LE(uint8_t& b, int32_t v);
        static void FromSint16LE(uint8_t& b, int16_t v);
        
        static void FromUuid(uint8_t& b, const Uuid& uuid);



        static inline bool IsLittleEndian()
        {
            uint8_t a[2];
            a[0] = 0x01;
            a[1] = 0xA4;
            uint16_t num=0;
            memcpy(&num,&a, 2);
            return num != 420;
        }
};

}