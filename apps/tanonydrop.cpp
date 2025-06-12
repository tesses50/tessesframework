#include "TessesFramework/TessesFramework.hpp"
#include <iostream>
using namespace Tesses::Framework;
using namespace Tesses::Framework::Streams;
using namespace Tesses::Framework::Http;
using namespace Tesses::Framework::Filesystem;

VFS* vfs;

int main(int argc, char** argv)
{


    TF_InitWithConsole();
    vfs = new SubdirFilesystem(&LocalFS,Tesses::Framework::Filesystem::VFSPath::GetAbsoluteCurrentDirectory(),false);

    CallbackServer cb([](ServerContext& ctx)->bool{
        if(ctx.path == "/")
        {
            ctx.WithMimeType("text/html")
            .SendText(
                "<!DOCTYPE html>"
                "<html>"
                "<head><meta charset=\"UTF-8\"><title>AnonyDump</title></head>"
                "<body>"
                "<h1>AnonyDump</h1>"
                "<a href=\"./files/\">Files</a>"
                "<form action=\"./upload\" method=\"post\" enctype=\"multipart/form-data\" accept-charset=\"UTF-8\">"
                "<input type=\"file\" name=\"file\" multiple>"
                "<input type=\"submit\" value=\"Upload\">"
                "</form>"
                "</body>"
                "</html>"
            );

            return true;
        }
        else if(ctx.path == "/upload")
        {
            if(ctx.NeedToParseFormData())
            {
                ctx.ParseFormData([](std::string mime, std::string filename,std::string name)->Stream* {
                    if(name != "file") return nullptr;
                    VFSPath path("/"+filename);
                    auto strm = vfs->OpenFile(path,"wb");
                    
                    return strm;
                });
                ctx.WithMimeType("text/html")
                .SendText(
                    "<!DOCTYPE html>"
                    "<html>"
                    "<head><title>AnonyDump - Uploaded successfully</title>"
                    "<body>"
                    "<h1>Uploaded successfully</h1>"
                    "<a href=\"./\">Back</a>"
                    
                    "</form>"
                    "</body>"
                    "</html>"
                );
                return true;
            }
            else {
                ctx.statusCode= Tesses::Framework::Http::BadRequest;
                ctx.WithMimeType("text/html")
                .SendText(
                    "<!DOCTYPE html>"
                    "<html>"
                    "<head><title>AnonyDump - Error: Must contain multipart and POST</title>"
                    "<body>"
                    "<h1>Error: Must contain multipart and POST</h1>"
                    "<a href=\"./\">Back</a>"
                    
                    "</form>"
                    "</body>"
                    "</html>"
                );
            }
        }
        return false;
    });

    Tesses::Framework::Http::MountableServer mountable(cb);
    mountable.Mount("/files/",new FileServer(vfs,true,true,false),true);

    HttpServer srv(4985,mountable);
    srv.StartAccepting();

    TF_RunEventLoop();

    TF_Quit();
    return 0;
}