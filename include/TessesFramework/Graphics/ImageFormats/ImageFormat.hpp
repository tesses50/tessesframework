#pragma once
#include "../../Streams/Stream.hpp"
#include "../Image.hpp"

namespace Tesses::Framework::Graphics::ImageFormats {
    class ImageFormat {
        public:
            virtual void Load(Tesses::Framework::Streams::Stream* strm, Image* image)=0;
            virtual void Save(Tesses::Framework::Streams::Stream* strm, Image* image,std::string flags="")=0;

            virtual ~ImageFormat();
    };
}