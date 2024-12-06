#include "TessesFramework/TessesFramework.hpp"
#include <iostream>
using namespace Tesses::Framework;
using namespace Tesses::Framework::Http;
using namespace Tesses::Framework::Streams;
using namespace Tesses::Framework::TextStreams;
using namespace Tesses::Framework::Threading;

class MyWebServer : public IHttpServer {
    public:
        bool Handle(ServerContext& ctx)
        {
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
            return false;
        }
        bool Handle1(ServerContext& ctx)
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
            return false;
        }
};

int main(int argc, char** argv)
{
    TF_Init();
    TcpServer server(9985U,10);
    MyWebServer svr;
    while(true)
    {
        std::string ip;
        uint16_t port;
        auto res = server.GetStream(ip, port);
        if(res == nullptr) return 0;
        std::cout << "Got from " << ip << ":" << port << std::endl;
        Thread thrd([ip,port,res,&svr]()->void {
            HttpServer::Process(*res, svr, ip,port, false);
            delete res;
        });
        thrd.Detach();
    }
    
}