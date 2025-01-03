#include "TessesFramework.h"

int main(int argc, char** argv)
{
    tf_vfs_t* local =  tf_vfs_create_local();

    tf_vfs_dir_t* dir = tf_vfs_opendir(local, "/home/mike");

    string_t* str;
    while(str = tf_vfs_readdir(dir))
    {
        string_println(str);
    }
    tf_vfs_closedir(dir);

    tf_stream_t* strm = tf_vfs_open(local,"/home/mike/myPhoto.png", "wb");



    tf_vfs_close(local);
}