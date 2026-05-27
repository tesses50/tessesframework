#include "TessesFramework/TessesFramework.hpp"
#include <TessesFramework/Platform/Environment.hpp>
#include <exception>
#include <iostream>
#include <stdexcept>
using namespace Tesses::Framework;
using namespace Tesses::Framework::Http;
using namespace Tesses::Framework::Streams;
using namespace Tesses::Framework::TextStreams;
using namespace Tesses::Framework::Threading;

std::shared_ptr<ServerSentEvents> sse = std::make_shared<ServerSentEvents>();

class Johnny : public ServerContextData {
  public:
    Johnny() { text = "Steve Ballmer"; }
    std::string text;
    ~Johnny() { std::cout << "Destroying" << std::endl; }
};
class MyWebServer : public IHttpServer {
  public:
    bool Handle(ServerContext &ctx) {
        std::cout << ctx.path << std::endl;
        if (ctx.path == "/") {
            std::shared_ptr<FileStream> fs =
                std::make_shared<FileStream>("index.html", "rb");

            ctx.WithMimeType("text/html").SendStream(fs);
            return true;
        } else if (ctx.path == "/mypath.html") {
            std::string txt =
                "<h1>Root: " + HttpUtils::HtmlEncode(ctx.GetServerRoot()) +
                "</h1>";
            ctx.WithMimeType("text/html").SendText(txt);
            return true;
        } else if (ctx.path == "/getabsolute.html") {
            std::string path;
            if (ctx.queryParams.TryGetFirst("path", path)) {
                std::string txt =
                    "<h1>Path: " +
                    HttpUtils::HtmlEncode(ctx.MakeAbsolute(path)) + "</h1>";
                ctx.WithMimeType("text/html").SendText(txt);
                return true;
            }
        } else if (ctx.path == "/streaming.html") {
            StreamWriter writer(ctx.OpenResponseStream());
            writer.WriteLine("<html>");
            writer.WriteLine("<head><title>Streaming</title></head>");
            writer.WriteLine("<body>");

            writer.WriteLine("<h1>Streaming</h1>");

            writer.WriteLine("<ul>");

            for (size_t i = 0; i < 10000; i++) {
                writer.WriteLine("<li>" + std::to_string(i) + "</li>");
                TF_Sleep(10);
            }

            writer.WriteLine("</ul>");

            writer.WriteLine("</body>");
            writer.WriteLine("</html>");
            return true;
        } else if (ctx.path == "/ssetest.html") {

        } else if (ctx.path == "/sse") {
            ctx.SendServerSentEvents(sse);
            return true;
        } else if (ctx.path == "/main.js") {

            std::shared_ptr<FileStream> fs =
                std::make_shared<FileStream>("main.js", "rb");

            ctx.WithMimeType("text/js").SendStream(fs);
            return true;
        } else if (ctx.path == "/upload") {
            ctx.ParseFormData(
                [](std::string mime, std::string filename, std::string name)
                    -> std::shared_ptr<Tesses::Framework::Streams::Stream> {
                    return std::make_shared<FileStream>(filename, "wb");
                });
        } else if (ctx.path == "/steve") {
            Johnny *data = ctx.GetServerContentData<Johnny>("mytag");
            data->text = "Demi Lovato";
        }

        return false;
    }
};
class MyOtherWebServer : public IHttpServer {
  public:
    bool Handle(ServerContext &ctx) {
        TF_LOG("IN HANDLE");
        if (ctx.path == "/") {
            std::string name;
            if (ctx.queryParams.TryGetFirst("name", name)) {
                std::cout << name << std::endl;
                TF_LOG(name);
                ctx.WithMimeType("text/plain")
                    .WithContentDisposition(HttpUtils::Sanitise(name) + ".txt",
                                            false)
                    .SendText(name + " is cool.");
                // do something with q

                return true;
            }
        } else if (ctx.path == "/status") {
            int64_t num;
            if (ctx.queryParams.TryGetFirstInt("code", num)) {
                ctx.statusCode = (StatusCode)num;
            }

            ctx.SendErrorPage(true);
            return true;
        } else if (ctx.path == "/error") {
            throw std::runtime_error("This is a error");
        } else if (ctx.path == "/error-debug") {
            ctx.WithDebug(true);
            throw std::runtime_error(
                "Platform is " +
                Tesses::Framework::Platform::Environment::GetPlatform());
        } else if (ctx.path == "/mymount/steve") {
            Johnny *data = ctx.GetServerContentData<Johnny>("mytag");
            ctx.SendText(data->text);
            return true;
        } else if (ctx.path == "/mypath.html") {
            std::string txt =
                "<h1>Root: " + HttpUtils::HtmlEncode(ctx.GetServerRoot()) +
                "</h1>";
            ctx.WithMimeType("text/html").SendText(txt);
            return true;
        }

        return false;
    }
};

int main(int argc, char **argv) {
    TF_InitWithConsole();
    int64_t timer = 0;
    auto timerHDL = TF_Timer([&timer]() -> void {
        timer++;

        sse->SendData("Timer has ticked " + std::to_string(timer) +
                      " times now");
    });

    std::shared_ptr<RouteServer> routeSvr = std::make_shared<RouteServer>();
    routeSvr->Get("/name/{name}/greeting", [](ServerContext &ctx) -> bool {
        std::string name;
        if (ctx.pathArguments.TryGetFirst("name", name)) {
            ctx.WithMimeType("text/plain").SendText("Hello " + name);
        } else {
            ctx.WithMimeType("text/plain").SendText("Please provide a name");
        }
        return true;
    });
    routeSvr->Get("/name/{name}/length", [](ServerContext &ctx) -> bool {
        std::string name;
        if (ctx.pathArguments.TryGetFirst("name", name)) {
            ctx.WithMimeType("text/plain")
                .SendText("The length of the name is " +
                          std::to_string(name.size()));
        } else {
            ctx.WithMimeType("text/plain").SendText("Please provide a name");
        }
        return true;
    });
    std::shared_ptr<MyOtherWebServer> myo =
        std::make_shared<MyOtherWebServer>();
    std::shared_ptr<MyWebServer> mws = std::make_shared<MyWebServer>();

    std::shared_ptr<MountableServer> mountable =
        std::make_shared<MountableServer>(myo);
    mountable->Mount("/mymount/", mws);
    mountable->Mount("/routeSvr/", routeSvr);
    HttpServer server(10001, mountable);
    server.StartAccepting();
    TF_RunEventLoop();
    std::cout << "Closing server" << std::endl;
}
