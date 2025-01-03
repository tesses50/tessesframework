
#include "TessesFramework/TessesFramework.hpp"

#include "TessesFramework/TessesFramework.h"
#include <iostream>

using namespace Tesses::Framework;
using namespace Tesses::Framework::Streams;
using namespace Tesses::Framework::Filesystem;
using namespace Tesses::Framework::Threading;


#if defined(__cplusplus)
extern "C" {
#endif



string_t* string_create()
{
    return static_cast<string_t*>(new std::string());
}
string_t* string_create_from_buff(const void* text, size_t len)
{
    return static_cast<string_t*>(new std::string((const char*)text,len));
}
string_t* string_create_from_charpointer(const char* text)
{
    return static_cast<string_t*>(new std::string(text));
}
string_t* string_resize(string_t* str, size_t len)
{
    static_cast<std::string*>(str)->resize(len);
    return str;
}
string_t* string_set_char(string_t* str, size_t index, char c)
{
    static_cast<std::string*>(str)->at(index) = c;
    return str;
}
char string_get_char(string_t* str,size_t index)
{
    return static_cast<std::string*>(str)->at(index);
}
string_t* string_append_char(string_t* str, char c)
{
    static_cast<std::string*>(str)->push_back(c);
    return str;
}
string_t* string_append_from_buff(string_t* str,const void* text, size_t len)
{
    static_cast<std::string*>(str)->append((const char*)text,len);
    return str;
}
string_t* string_append_from_charpointer(string_t* str,const char* text)
{
    static_cast<std::string*>(str)->append(text);
    return str;
}
string_t* string_append(string_t* str, string_t* toAppend)
{
    if(str == NULL) return NULL;
    static_cast<std::string*>(str)->append(*static_cast<std::string*>(str));
    return str;
}
string_t* string_append_signed(string_t* str, int64_t num)
{
    static_cast<std::string*>(str)->append(std::to_string(num));
    return str;
}
string_t* string_append_unsigned(string_t* str, uint64_t num)
{
    static_cast<std::string*>(str)->append(std::to_string(num));
    return str;
}
string_t* string_append_double(string_t* str, double num)
{
    static_cast<std::string*>(str)->append(std::to_string(num));
    return str;
}
void string_print(string_t* str)
{
    std::cout << *static_cast<std::string*>(str);
}
void string_println(string_t* str)
{
    std::cout << *static_cast<std::string*>(str) << std::endl;
}
size_t string_size(string_t* str)
{
    return static_cast<std::string*>(str)->size();
}
const char* string_c_str(string_t* str)
{
    return static_cast<std::string*>(str)->c_str();
}
void string_free(string_t* str)
{
    delete static_cast<std::string*>(str);
}

void tf_init()
{
    TF_Init();
}
tf_thread_t* tf_create_thread(void* userData, tf_action_user_data_t cb)
{
    Thread* thrd = new Thread([userData,cb]()->void{
        cb(userData);
    });
    return static_cast<tf_thread_t*>(thrd);
}
void tf_join_thread(tf_thread_t* thrd)
{
    auto thrd2 = static_cast<Thread*>(thrd);
    if(thrd2 != nullptr)
    {
        thrd2->Join();
        delete thrd2;
    }
}
void tf_detach_thread(tf_thread_t* thrd)
{
    auto thrd2 = static_cast<Thread*>(thrd);
    if(thrd2 != nullptr)
    {
        thrd2->Detach();
        delete thrd2;
    }
}

tf_mutex_t* tf_mutex_create()
{
    return static_cast<tf_mutex_t*>(new Mutex());
}
void tf_mutex_lock(tf_mutex_t* mtx)
{
    static_cast<Mutex*>(mtx)->Lock();
    
}
bool tf_mutex_trylock(tf_mutex_t* mtx)
{
   return static_cast<Mutex*>(mtx)->TryLock();
    
}
void tf_mutex_unlock(tf_mutex_t* mtx)
{
    static_cast<Mutex*>(mtx)->Unlock();
    
}
void tf_mutex_free(tf_mutex_t* mtx)
{
    delete static_cast<Mutex*>(mtx);
    
}

bool tf_stream_canread(tf_stream_t* strm)
{
    return static_cast<Stream*>(strm)->CanRead();
}
bool tf_stream_canwrite(tf_stream_t* strm)
{
    return static_cast<Stream*>(strm)->CanWrite();
}
bool tf_stream_canseek(tf_stream_t* strm)
{
    return static_cast<Stream*>(strm)->CanSeek();
}
bool tf_stream_eof(tf_stream_t* strm)
{
    return static_cast<Stream*>(strm)->EndOfStream();
}
int64_t tf_stream_getlen(tf_stream_t* strm)
{
    return static_cast<Stream*>(strm)->GetLength();
}
int64_t tf_stream_getpos(tf_stream_t* strm)
{
    return static_cast<Stream*>(strm)->GetPosition();
}
void tf_stream_seek(tf_stream_t* strm, int64_t pos, TF_WHENCE whence)
{
    static_cast<Stream*>(strm)->Seek(pos,whence == TF_SEEK_BEGIN ? SeekOrigin::Begin : whence == TF_SEEK_CURRENT ? SeekOrigin::Current : SeekOrigin::End);
}
size_t tf_stream_read(tf_stream_t* strm, uint8_t* buffer, size_t length)
{
    return static_cast<Stream*>(strm)->Read(buffer,length);
}
size_t tf_stream_write(tf_stream_t* strm, const uint8_t* buffer, size_t length)
{
    return static_cast<Stream*>(strm)->Write(buffer,length);
}
size_t tf_stream_readblock(tf_stream_t* strm, uint8_t* buffer, size_t length)
{
    return static_cast<Stream*>(strm)->ReadBlock(buffer,length);
}
void tf_stream_writeblock(tf_stream_t* strm, const uint8_t* buffer, size_t length)
{
    static_cast<Stream*>(strm)->WriteBlock(buffer,length);
}
void tf_stream_copyto(tf_stream_t* src, tf_stream_t* dest, size_t blockSize)
{
    static_cast<Stream*>(src)->CopyTo(static_cast<Stream*>(dest),blockSize);
}
void tf_stream_close(tf_stream_t* strm)
{
    delete static_cast<Stream*>(strm);
}
void tf_stream_flush(tf_stream_t* strm)
{
    static_cast<Stream*>(strm)->Flush();
}
int32_t tf_stream_readbyte(tf_stream_t* strm)
{
    return static_cast<Stream*>(strm)->ReadByte();
}
void tf_stream_writebyte(tf_stream_t* strm, uint8_t val)
{
    static_cast<Stream*>(strm)->WriteByte(val);
}

tf_stream_t* tf_stream_fopen(const char* file, const char* mode)
{
    FILE* f = fopen(file,mode);
    if(f == NULL) return NULL;
    return static_cast<tf_stream_t*>(new FileStream(f,true,std::string(mode)));
}

#if defined(__cplusplus)
}
#endif
