#pragma once
#include "ImageFormat.hpp"
namespace Tesses::Framework::Graphics::ImageFormats {
    class Bitmap : public ImageFormat {
        public:
            Bitmap();
            void Load(Tesses::Framework::Streams::Stream* strm, Image* image);
            void Save(Tesses::Framework::Streams::Stream* strm,  Image* image,std::string flags="");
    };
};