#include "TessesFramework/Text/HeaderGenerator.hpp"
namespace Tesses::Framework::Text {
    
    void GenerateCHeaderFile(std::shared_ptr<Streams::Stream> strm,std::string name, std::shared_ptr<TextStreams::TextWriter> writer)
    {
        const size_t BLK_SZ=1024;
        writer->WriteLine("#pragma once");
        writer->WriteLine("#if defined(__cplusplus)");
        writer->WriteLine("extern \"C\" {");
        writer->WriteLine("#endif");
        writer->WriteLine("#include <stdint.h>");
        writer->WriteLine("#include <stddef.h>");
        writer->Write("const uint8_t ");
        writer->Write(name);
        writer->WriteLine("_data[] = {");
        uint64_t total = 0;
        size_t read;
        
        uint8_t* data = new uint8_t[BLK_SZ];
        bool first=true;


        do {
            read = strm->ReadBlock(data, BLK_SZ);

            for(size_t i = 0; i < read; i++)
            {
                if(!first) writer->Write(", ");
                writer->Write((uint64_t)data[i]);
                first=false;
            }
            total += read;
        } while(read != 0);
        
        
        delete data;

        writer->WriteLine("};");
        writer->Write("const size_t ");
        writer->Write(name);
        writer->Write("_length = ");
        writer->Write(total);
        writer->WriteLine(";");

        writer->WriteLine("#if defined(__cplusplus)");
        writer->WriteLine("}");
        writer->WriteLine("#endif");

    }
    
    std::string GenerateCHeaderFile(std::shared_ptr<Streams::Stream> strm,std::string name)
    {
        auto writer=std::make_shared<TextStreams::StringWriter>();
        GenerateCHeaderFile(strm,name,writer);
        return writer->GetString();
    }
    void GenerateCHeaderFile(const std::vector<uint8_t>& data,std::string name, std::shared_ptr<TextStreams::TextWriter> writer)
    {
        auto ms = std::make_shared<Tesses::Framework::Streams::MemoryStream>(false);
        ms->GetBuffer() = data;
        GenerateCHeaderFile(ms,name,writer);
    }
    std::string GenerateCHeaderFile(const std::vector<uint8_t>& data,std::string name)
    {
        auto writer = std::make_shared<TextStreams::StringWriter>();
        GenerateCHeaderFile(data,name,writer);
        return writer->GetString();
    }
};