#include "TessesFramework/TessesFramework.hpp"
#include <iostream>
using namespace Tesses::Framework;
using namespace Tesses::Framework::Streams;
using namespace Tesses::Framework::Http;
using namespace Tesses::Framework::Filesystem;

std::shared_ptr<VFS> vfs;

int main(int argc, char **argv) {

    TF_InitWithConsole();
    vfs = std::make_shared<SubdirFilesystem>(
        LocalFS,
        Tesses::Framework::Filesystem::VFSPath::GetAbsoluteCurrentDirectory());

    std::shared_ptr<CallbackServer> cb = std::make_shared<
        CallbackServer>([](ServerContext &ctx) -> bool {
        if (ctx.path == "/") {
            ctx.WithMimeType("text/html")
                .SendText(
                    "<!DOCTYPE html>"
                    "<html>"
                    "<head><meta "
                    "charset=\"UTF-8\"><title>AnonyDrop</title><meta "
                    "name=\"color-scheme\" content=\"dark light\"></head>"
                    "<body>"
                    "<h1>AnonyDrop</h1>"
                    "<a href=\"./files/\">Files</a>"
                    "<form action=\"./upload\" method=\"post\" "
                    "enctype=\"multipart/form-data\" accept-charset=\"UTF-8\">"
                    "<input type=\"file\" name=\"file\" multiple>"
                    "<input type=\"submit\" value=\"Upload\">"
                    "</form>"
                    "</body>"
                    "</html>");

            return true;
        } else if (ctx.path == "/upload") {
            if (ctx.NeedToParseFormData()) {
                ctx.ParseFormData(
                    [](std::string mime, std::string filename,
                       std::string name) -> std::shared_ptr<Stream> {
                        if (name != "file")
                            return nullptr;
                        VFSPath path("/" + filename);
                        auto strm = vfs->OpenFile(path, "wb");

                        return strm;
                    });
                ctx.WithMimeType("text/html")
                    .SendText("<!DOCTYPE html>"
                              "<html>"
                              "<head><meta name=\"color-scheme\" "
                              "content=\"dark light\"><title>AnonyDrop - "
                              "Uploaded successfully</title></head>"
                              "<body>"
                              "<h1>Uploaded successfully</h1>"
                              "<a href=\"./\">Back</a>"

                              "</body>"
                              "</html>");
                return true;
            } else {
                ctx.statusCode = Tesses::Framework::Http::BadRequest;
                ctx.WithMimeType("text/html")
                    .SendText("<!DOCTYPE html>"
                              "<html>"
                              "<head><title>AnonyDump - Error: Must contain "
                              "multipart and POST</title>"
                              "<body>"
                              "<h1>Error: Must contain multipart and POST</h1>"
                              "<a href=\"./\">Back</a>"

                              "</form>"
                              "</body>"
                              "</html>");
            }
        }
        return false;
    });

    auto mountable =
        std::make_shared<Tesses::Framework::Http::MountableServer>(cb);
    mountable->Mount("/files/", std::make_shared<FileServer>(vfs, true, false));

    HttpServer srv(4985, mountable);
    srv.StartAccepting();

    TF_RunEventLoop();

    TF_Quit();
    return 0;
}
