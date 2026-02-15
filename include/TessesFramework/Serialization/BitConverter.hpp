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
        /**
         * @brief Get the bits of a double from a int64_t
         * 
         * @param v a int64_t with double's bits
         * @return double the double
         */
        static double ToDoubleBits(uint64_t v);
        /**
         * @brief Get the bits of a int64_t from a double
         * 
         * @param v a double with int64_t's bits
         * @return uint64_t the int64_t
         */
        static uint64_t ToUintBits(double v);
        /**
         * @brief Get big endian double from uint8_t reference of first element of 8 byte array
         * 
         * @param b a reference to the first byte of an array
         * @return double the double
         */
        static double ToDoubleBE(uint8_t& b);
        /**
         * @brief Get big endian uint64_t from uint8_t reference of first element of 8 byte array
         * 
         * @param b a reference to the first byte of an array
         * @return uint64_t the uint64_t
         */
        static uint64_t ToUint64BE(uint8_t& b);
        static uint32_t ToUint32BE(uint8_t& b);
        static uint16_t ToUint16BE(uint8_t& b);

        static double ToDoubleLE(uint8_t& b);
        static uint64_t ToUint64LE(uint8_t& b);
        static uint32_t ToUint32LE(uint8_t& b);
        static uint16_t ToUint16LE(uint8_t& b);

        static Uuid ToUuidBE(uint8_t& b);

        static Uuid ToUuidMS(uint8_t& b);

        static void ToUuidBE(uint8_t& b, Uuid& uuid);

        static void ToUuidMS(uint8_t& b, Uuid& uuid);

        static void FromDoubleBE(uint8_t& b, double v);
        static void FromUint64BE(uint8_t& b, uint64_t v);
        static void FromUint32BE(uint8_t& b, uint32_t v);
        static void FromUint16BE(uint8_t& b, uint16_t v);
        static void FromDoubleLE(uint8_t& b, double v);
        static void FromUint64LE(uint8_t& b, uint64_t v);
        static void FromUint32LE(uint8_t& b, uint32_t v);
        static void FromUint16LE(uint8_t& b, uint16_t v);
        
        static void FromUuidBE(uint8_t& b, const Uuid& uuid);

        static void FromUuidMS(uint8_t& b, const Uuid& uuid);


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