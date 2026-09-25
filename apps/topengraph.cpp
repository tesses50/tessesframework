#include "TessesFramework/TessesFramework.hpp"
using namespace Tesses::Framework::Streams;
using namespace Tesses::Framework::Serialization;
using namespace Tesses::Framework::TextStreams;

std::shared_ptr<Stream> OpenRead(std::string src) {

    if (src == "-") {
        return std::make_shared<FileStream>(stdin, false, "r");
    } else if (src.find("http://") == 0 || src.find("https://") == 0) {
        Tesses::Framework::Http::HttpRequest req;
        req.url = src;
        req.method = "GET";
        Tesses::Framework::Http::HttpResponse resp(req);

        if ((int)resp.statusCode < 200 || (int)resp.statusCode > 299) {
            throw std::runtime_error(
                "Response does not indicate success: " +
                std::to_string((int)resp.statusCode) + " " +
                Tesses::Framework::Http::HttpUtils::StatusCodeString(
                    resp.statusCode) +
                ".");
        }
        return resp.ReadAsStream();
    } else {
        auto strm = std::make_shared<FileStream>(src, "r");
        if (!strm->CanRead()) {
            return nullptr;
        }
        return strm;
    }
}
int main(int argc, char **argv) {
    if (argc < 2) {
        std::cout << "USAGE: " << argv[0] << " SRC" << std::endl;
        std::cout << "SRC: html file or url or - for stdin" << std::endl;
        return 0;
    }
    auto src = OpenRead(argv[1]);

    if (src == nullptr) {

        std::cerr << "ERROR: Input could not be read" << std::endl;
        return 1;
    }

    auto html = Html::Parse(std::make_shared<StreamReader>(src));
    std::vector<std::shared_ptr<const Html::HtmlNode>> titleTags;
    html->FindNodes("title", titleTags);

    for (auto &title : titleTags) {
        Tesses::Framework::Console::WriteView("Title: ");
        Tesses::Framework::Console::WriteLine(title->InnerText());
        break;
    }

    std::vector<std::shared_ptr<const Html::HtmlNode>> metaTags;
    html->FindNodes("meta", metaTags);

    for (auto &meta : metaTags) {
        auto prop_off = meta->attributes.find("property");
        auto content_off = meta->attributes.find("content");
        if (!(prop_off != meta->attributes.end() &&
              content_off != meta->attributes.end()))
            continue;
        auto &prop = prop_off->second;
        auto &content = content_off->second;

        if (prop && content && prop->find("og:") == 0) {
            Tesses::Framework::Console::WriteView(prop.value());
            Tesses::Framework::Console::WriteView(": ");
            Tesses::Framework::Console::WriteLineView(content.value());
        }
    }

    return 0;
}