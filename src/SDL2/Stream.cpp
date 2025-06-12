
#if defined(TESSESFRAMEWORK_ENABLE_SDL2)

#include "TessesFramework/SDL2/Stream.hpp"
namespace Tesses::Framework::SDL2 {

static SDLCALL size_t rwops_stream_read(struct SDL_RWops * context, void *ptr,
                             size_t size, size_t maxnum)
{
    if(context->hidden.unknown.data1 == nullptr) return 0;
    auto ctx = static_cast<Tesses::Framework::Streams::Stream*>(context->hidden.unknown.data1);
    if(ctx->CanRead())
        return ctx->Read((uint8_t*)ptr,size*maxnum) / size;
    return 0;
}
static SDLCALL size_t rwops_stream_write(struct SDL_RWops * context, const void *ptr,
                             size_t size, size_t maxnum)
{
    if(context->hidden.unknown.data1 == nullptr) return 0;
    auto ctx = static_cast<Tesses::Framework::Streams::Stream*>(context->hidden.unknown.data1);
    if(ctx->CanWrite())
    return ctx->Write((const uint8_t*)ptr,size*maxnum) / size;
    return 0;
}
static SDLCALL Sint64 rwops_stream_seek(struct SDL_RWops * context, Sint64 offset,
                             int whence)
{

    if(context->hidden.unknown.data1 == nullptr) return -1;
    auto ctx = static_cast<Tesses::Framework::Streams::Stream*>(context->hidden.unknown.data1);
    if(!ctx->CanSeek()) return -1;

    switch(whence)
    {
        case RW_SEEK_SET:
            ctx->Seek(offset,Tesses::Framework::Streams::SeekOrigin::Begin);
            break;
        case RW_SEEK_CUR:
            ctx->Seek(offset,Tesses::Framework::Streams::SeekOrigin::Current);
            break;
        case RW_SEEK_END:
            ctx->Seek(offset,Tesses::Framework::Streams::SeekOrigin::End);
            break;
    }


    return ctx->GetPosition();
}
static SDLCALL Sint64 rwops_stream_size(struct SDL_RWops * context)
{
    if(context->hidden.unknown.data1 == nullptr) return -1;
    auto ctx = static_cast<Tesses::Framework::Streams::Stream*>(context->hidden.unknown.data1);
    return ctx->GetLength();
}
static SDLCALL int rwops_stream_close(struct SDL_RWops* context)
{
    if(context->hidden.unknown.data2 == nullptr) return 0;
    delete static_cast<Tesses::Framework::Streams::Stream*>(context->hidden.unknown.data1);
    return 0;
}
SDL_RWops* RwopsFromStream(Tesses::Framework::Streams::Stream* strm, bool owns)
{
    auto rw=SDL_AllocRW();
    rw->type = SDL_RWOPS_UNKNOWN;
    rw->hidden.unknown.data1 = static_cast<void*>(strm);
    rw->hidden.unknown.data2 = (void*)(uintptr_t)(owns?1:0);
    rw->read = rwops_stream_read;
    rw->write = rwops_stream_write;
    rw->seek = rwops_stream_seek;
    rw->size = rwops_stream_size;
    rw->close = rwops_stream_close;

    
    return rw;
}
}
#endif