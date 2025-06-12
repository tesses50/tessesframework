#pragma once
#include "../Common.hpp"

namespace Tesses::Framework::TextStreams
{
    class NewLine {}; //dummy class
    class TextWriter {
        public:
            TextWriter();
            std::string newline;
            virtual void WriteData(const char* text, size_t len)=0;
            void Write(int64_t n);
            void Write(uint64_t n);
            void Write(const void* ptr);
            void Write(const char* ptr);
            void Write(char c);
            void Write(double d);
            void Write(std::string text);
            inline TextWriter& operator<<(int64_t n)
            {
                Write(n);
                return *this;
            }
            inline TextWriter& operator<<(uint64_t n)
            {
                Write(n);
                return *this;
            }
            inline TextWriter& operator<<(const void* n)
            {
                Write(n);
                return *this;
            }
            inline TextWriter& operator<<(const char* n)
            {
                Write(n);
                return *this;
            }
            inline TextWriter& operator<<(char n)
            {
                Write(n);
                return *this;
            }
            inline TextWriter& operator<<(double n)
            {
                Write(n);
                return *this;
            }
            inline TextWriter& operator<<(std::string n)
            {
                Write(n);
                return *this;
            }
            
            inline TextWriter& operator<<(NewLine nl)
            {
                WriteLine();
                return *this;
            }
        
            void WriteLine(std::string txt);
            void WriteLine(int64_t n);
            void WriteLine(uint64_t n);
            void WriteLine(const void* ptr);
            void WriteLine(const char* ptr);
            void WriteLine(char c);
            void WriteLine(double d);
            
            void WriteLine();
        
            virtual ~TextWriter();
    };

    
}