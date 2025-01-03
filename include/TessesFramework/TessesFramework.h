#pragma once

#if defined(__cplusplus)
extern "C" {
#endif
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

typedef void string_t;
typedef void tf_vfs_t;
typedef void tf_stream_t;
typedef void tf_thread_t;
typedef void tf_mutex_t;
typedef void tf_vfs_dir_t;

typedef void (*tf_action_user_data_t)(void* ptr);


typedef enum {
    TF_SEEK_BEGIN,
    TF_SEEK_CURRENT,
    TF_SEEK_END
} TF_WHENCE;

string_t* string_create();
string_t* string_create_from_buff(const void* text, size_t len);
string_t* string_create_from_charpointer(const char* text);
string_t* string_resize(string_t* str, size_t len);
string_t* string_set_char(string_t* str, size_t index, char c);
char string_get_char(string_t* str,size_t index);
string_t* string_append_char(string_t* str, char c);
string_t* string_append_from_buff(string_t* str,const void* text, size_t len);
string_t* string_append_from_charpointer(string_t* str,const char* text);
string_t* string_append(string_t* str, string_t* toAppend);
string_t* string_append_signed(string_t* str, int64_t num);
string_t* string_append_unsigned(string_t* str, uint64_t num);
string_t* string_append_double(string_t* str, double num);
void string_print(string_t* str);
void string_println(string_t* str);
size_t string_size(string_t* str);
const char* string_c_str(string_t* str);
void string_free(string_t* str);

void tf_init();
tf_thread_t* tf_create_thread(void* userData, tf_action_user_data_t cb);
void tf_join_thread(tf_thread_t* thrd);
void tf_detach_thread(tf_thread_t* thrd);

tf_mutex_t* tf_mutex_create();
void tf_mutex_lock(tf_mutex_t* mtx);
bool tf_mutex_trylock(tf_mutex_t* mtx);
void tf_mutex_unlock(tf_mutex_t* mtx);
void tf_mutex_free(tf_mutex_t* mtx);

bool tf_stream_canread(tf_stream_t* strm);
bool tf_stream_canwrite(tf_stream_t* strm);
bool tf_stream_canseek(tf_stream_t* strm);
bool tf_stream_eof(tf_stream_t* strm);
int64_t tf_stream_getlen(tf_stream_t* strm);
int64_t tf_stream_getpos(tf_stream_t* strm);
void tf_stream_seek(tf_stream_t* strm, int64_t pos, TF_WHENCE whence);
size_t tf_stream_read(tf_stream_t* strm, uint8_t* buffer, size_t length);
size_t tf_stream_write(tf_stream_t* strm, const uint8_t* buffer, size_t length);
size_t tf_stream_readblock(tf_stream_t* strm, uint8_t* buffer, size_t length);
void tf_stream_writeblock(tf_stream_t* strm, const uint8_t* buffer, size_t length);
void tf_stream_copyto(tf_stream_t* src, tf_stream_t* dest, size_t blockSize);
void tf_stream_close(tf_stream_t* strm);
void tf_stream_flush(tf_stream_t* strm);
int32_t tf_stream_readbyte(tf_stream_t* strm);
void tf_stream_writebyte(tf_stream_t* strm, uint8_t val);

tf_stream_t* tf_stream_fopen(const char* file, const char* mode);

#if defined(__cplusplus)
}
#endif
