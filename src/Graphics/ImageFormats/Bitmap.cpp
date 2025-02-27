#include "TessesFramework/Graphics/ImageFormats/Bitmap.hpp"
#include "TessesFramework/Streams/ByteWriter.hpp"
#include "TessesFramework/Streams/MemoryStream.hpp"
#include <iostream>
namespace Tesses::Framework::Graphics::ImageFormats {
    Bitmap::Bitmap()
    {

    }
    void Bitmap::Load(Tesses::Framework::Streams::Stream* strm, Image* image)
    {
        uint8_t header[14];
        if(strm->ReadBlock(header,14) < 14) throw TextException("Invalid bmp file, header to small.");
        if(header[0] != 0x42 || header[1] != 0x4D) throw TextException("Invalid bmp file, does not start with BM.");
        uint8_t info_header[40];
        if(strm->ReadBlock(info_header,40) < 40) throw TextException("Invalid bmp file, info header is too small.");
        if(info_header[0] != 40 || info_header[1] != 0 || info_header[2] != 0 || info_header[3] != 0) throw TextException("Invald bmp file, info header size is corrupt.");
        uint32_t width = 0;
        width |= (uint32_t)info_header[4];
        width |= (uint32_t)info_header[5] << 8;
        width |= (uint32_t)info_header[6] << 16;
        width |= (uint32_t)info_header[7] << 24;
        uint32_t height = 0;
        height |= (uint32_t)info_header[8];
        height |= (uint32_t)info_header[9] << 8;
        height |= (uint32_t)info_header[10] << 16;
        height |= (uint32_t)info_header[11] << 24;

        int32_t h0= *(int32_t*)&height;
        bool isInverted=h0 < 0;
        if(isInverted) height = (uint32_t)(-h0);

        image->SetSize(width,height);
        uint16_t planes = 0;
        planes |= (uint16_t)info_header[12];
        planes |= (uint16_t)info_header[13] << 8;
        if(planes != 1) throw TextException("Planes is not 1");
        uint16_t bpp = 0;
        bpp |= (uint16_t)info_header[14];
        bpp |= (uint16_t)info_header[15] << 8;
        uint32_t compression = 0;
        compression |= (uint32_t)info_header[16];
        compression |= (uint32_t)info_header[17] << 8;
        compression |= (uint32_t)info_header[18] << 16;
        compression |= (uint32_t)info_header[19] << 24;

        if(compression != 0) throw TextException("This bmp decoder does not support compression");
        uint32_t imageSize = 0;
        imageSize |= (uint32_t)info_header[20];
        imageSize |= (uint32_t)info_header[21] << 8;
        imageSize |= (uint32_t)info_header[22] << 16;
        imageSize |= (uint32_t)info_header[23] << 24;
       
        uint32_t colorsUsed = 0;
        colorsUsed |= (uint32_t)info_header[32];
        colorsUsed |= (uint32_t)info_header[33] << 8;
        colorsUsed |= (uint32_t)info_header[34] << 16;
        colorsUsed |= (uint32_t)info_header[35] << 24;

        std::vector<Color> col={};

        if(colorsUsed > 0)
        {
            col.resize(colorsUsed);
            uint8_t* buff = new uint8_t[colorsUsed*4];
            if(strm->ReadBlock(buff,colorsUsed*4) != colorsUsed*4) { delete buff; throw TextException("Could not read colors");}
            for(size_t i = 0; i < colorsUsed; i++)
            {
                col[i].Red = buff[i * 4 + 2];
                col[i].Green = buff[i * 4 + 1];
                col[i].Blue = buff[i * 4];
                col[i].Alpha = 255;
            }
            delete buff;
        }
        std::vector<Color>& pixel_out = image->Data();
        if(bpp == 24)
        {
            uint32_t stride = 3 * width;
            uint32_t m = stride % 4;
            if(m != 0) stride += 4 - (m % 4);

            uint8_t* buffer = new uint8_t[stride];
            for(uint32_t y = 0; y < height; y++)
            {

                uint32_t yOut = isInverted ? y : ((height - y)-1);
                if(strm->ReadBlock(buffer,stride) != stride)
                {
                    delete buffer;
                    throw TextException("Bmp image data is truncated");
                }
                for(uint32_t x = 0; x < width; x++)
                {
                    pixel_out[yOut * width + x].Red = buffer[x*3+2];
                    pixel_out[yOut * width + x].Green = buffer[x*3+1];
                    pixel_out[yOut * width + x].Blue = buffer[x*3];
                    pixel_out[yOut * width + x].Alpha=255;
                }
            }
            delete buffer;
        }
        else if(bpp == 16)
        {
            uint32_t stride = 2 * width;
            uint32_t m = stride % 4;
            if(m != 0) stride += 4 - (m % 4);

            uint8_t* buffer = new uint8_t[stride];
            for(uint32_t y = 0; y < height; y++)
            {
                uint32_t yOut =isInverted ? y : ((height - y)-1);
                if(strm->ReadBlock(buffer,stride) != stride)
                {
                    delete buffer;
                    throw TextException("Bmp image data is truncated");
                }
                for(uint32_t x = 0; x < width; x++)
                {
                    uint16_t px = (uint16_t)(buffer[x*2+1] << 8) | (uint16_t)buffer[x*2];
                    uint16_t red = (px & 0xF800) >> 11;
                    uint16_t green = (px & 0x07E0) >> 5;
                    uint16_t blue =  (px & 0x001F);
                    red = (red * 255 + 15) / 31;
                    green = (green * 255 + 31) / 63;
                    blue = (blue * 255 + 15) / 31;
                    pixel_out[yOut * width + x].Red = red;
                    pixel_out[yOut * width + x].Green = green;
                    pixel_out[yOut * width + x].Blue = blue;
                    pixel_out[yOut * width + x].Alpha=255;
                }
            }
            delete buffer;
        }
        else if(bpp == 8)
        {
            uint32_t stride = width;

            uint8_t* buffer = new uint8_t[stride];
            for(uint32_t y = 0; y < height; y++)
            {
                uint32_t yOut =isInverted ? y : ((height - y)-1);
                if(strm->ReadBlock(buffer,stride) != stride)
                {
                    delete buffer;
                    throw TextException("Bmp image data is truncated");
                }
                for(uint32_t x = 0; x < width; x++)
                {
                    pixel_out[yOut * width + x] = col.at (buffer[x]);
                }
            }
            delete buffer;
        }
        else throw TextException("Bitdepth " + std::to_string(bpp) + "bpp is not supported yet.");
    }
    void Bitmap::Save(Tesses::Framework::Streams::Stream* strm, Image* image,std::string flags)
    {
        Tesses::Framework::Streams::MemoryStream memStrm(true);
        Tesses::Framework::Streams::ByteWriter writer(memStrm);
        uint32_t stride = image->Width() * 3;
        uint32_t m = stride % 4;
        if(m != 0) stride += 4 - (m % 4);
        uint32_t sz = (stride * image->Height())+14+40;
        memStrm.WriteBlock((const uint8_t*)"BM",2);

        writer.WriteU32LE(sz);
        writer.WriteU32LE(0);
        writer.WriteU32LE(14+40);
        writer.WriteU32LE(40);
        writer.WriteU32LE(image->Width());
        writer.WriteU32LE(image->Height());
        writer.WriteU16LE(1);
        writer.WriteU16LE(24);
        writer.WriteU32LE(0);
        writer.WriteU32LE(0);
        writer.WriteU32LE(0);
        writer.WriteU32LE(0);
        writer.WriteU32LE(0);
        writer.WriteU32LE(0);
        memStrm.Seek(0,Tesses::Framework::Streams::SeekOrigin::Begin);
        memStrm.CopyTo(strm);
        uint8_t* buffer = new uint8_t[stride];
        memset(buffer,0,stride);
        auto width = image->Width();
        auto height = image->Height();
        for(uint32_t y = 0; y < height; y++)
        {
            
            uint32_t yOut = (height - y)-1;
            for(uint32_t x = 0; x < width; x++)
            {
                auto c = image->GetPixel(x,yOut);
                buffer[x*3+2] = c.Red;
                buffer[x*3+1] = c.Green;
                buffer[x*3] = c.Blue;
            }
            strm->WriteBlock(buffer,stride);
        }
    }
}