#include "TessesFramework/TessesFramework.hpp"
#include <iostream>
using namespace Tesses::Framework;
using namespace Tesses::Framework::Http;
using namespace Tesses::Framework::Streams;
using namespace Tesses::Framework::TextStreams;
using namespace Tesses::Framework::Threading;
class Johnny : public ServerContextData
{
    public:
        Johnny()
        {
            text = "Steve Ballmer";
        }
        std::string text;
        ~Johnny()
        {
            std::cout << "Destroying" << std::endl;
        }
};
class MyWebServer : public IHttpServer {
    public:
        bool Handle(ServerContext& ctx)
        {
            std::cout << ctx.path << std::endl;
            if(ctx.path == "/")
            {
                FileStream fs("index.html","rb");
                
                ctx
                .WithMimeType("text/html")
                .SendStream(fs);
                return true;
            }
            else if(ctx.path == "/streaming.html")
            {
                StreamWriter writer(ctx.OpenResponseStream(),true);
                writer.WriteLine("<html>");
                writer.WriteLine("<head><title>Streaming</title></head>");
                writer.WriteLine("<body>");

                writer.WriteLine("<h1>Streaming</h1>");

                writer.WriteLine("<ul>");

                for(size_t i=0;i<10000; i++)
                {
                    writer.WriteLine("<li>" + std::to_string(i) + "</li>");
                    
                }

                writer.WriteLine("</ul>");

                writer.WriteLine("</body>");
                writer.WriteLine("</html>");
                return true;
            }
            else if(ctx.path == "/main.js")
            {

                FileStream fs("main.js","rb");
                
                ctx
                .WithMimeType("text/js")
                .SendStream(fs);
                return true;
            }
            else if(ctx.path == "/upload")
            {
                ctx.ParseFormData([](std::string mime, std::string filename, std::string name)->Tesses::Framework::Streams::Stream*{
                    return new FileStream(filename,"wb");
                });
            }
            else if(ctx.path == "/steve")
            {
                Johnny* data = ctx.GetServerContentData<Johnny>("mytag");
                data->text = "Demi Lovato";
            }
            
            return false;
        }
        
};
class MyOtherWebServer : public IHttpServer
{
    public:
        bool Handle(ServerContext& ctx)
        {
            if(ctx.path == "/")
            {
            std::string name;
            if(ctx.queryParams.TryGetFirst("name",name))
            {
                std::cout << name << std::endl;
                ctx
                .WithMimeType("text/plain")
                .WithContentDisposition(HttpUtils::Sanitise(name) + ".txt",false)
                .SendText(name + " is cool.");
                //do something with q
                
                return true;
            }
            }
            else if(ctx.path == "/mymount/steve")
            {
                Johnny* data = ctx.GetServerContentData<Johnny>("mytag");
                ctx.SendText(data->text);
                return true;
            }
            
            return false;
        }
};

int main(int argc, char** argv)
{
    TF_Init();
    MyOtherWebServer myo;
    MyWebServer mws;

    MountableServer mountable(myo);
    mountable.Mount("/mymount/",mws);
    HttpServer server(10001,mountable);
    server.StartAccepting();
    TF_RunEventLoop();
    std::cout << "Closing server" << std::endl;
}