#include "TessesFramework/TessesFramework.hpp"
using namespace Tesses::Framework::Streams;
using namespace Tesses::Framework::Serialization;
using namespace Tesses::Framework::TextStreams;

Json::JToken Html2JToken(std::shared_ptr<Html::HtmlNode> node) {
    if (node->isText) {
        return node->text_or_tag;
    } else {
        Json::JObject obj;
        obj.SetValue("Tag", node->text_or_tag);
        Json::JObject attrs;
        for (auto &item : node->attributes) {
            if (item.second) {
                attrs.SetValue(item.first, *item.second);
            } else {
                attrs.SetValue(item.first, nullptr);
            }
        }
        obj.SetValue("Attributes", attrs);

        Json::JArray childern;

        for (auto &item : node->children)
            childern.Add(Html2JToken(item));

        obj.SetValue("Children", childern);
        return obj;
    }
}

std::shared_ptr<FileStream> OpenWrite(std::string dest) {
    if (dest == "-") {
        return std::make_shared<FileStream>(stdout, false, "w");
    } else {
        auto strm = std::make_shared<FileStream>(dest, "w");
        if (!strm->CanWrite()) {
            return nullptr;
        }
        return strm;
    }
}
std::shared_ptr<FileStream> OpenRead(std::string src) {
    if (src == "-") {
        return std::make_shared<FileStream>(stdin, false, "r");
    } else {
        auto strm = std::make_shared<FileStream>(src, "r");
        if (!strm->CanRead()) {
            return nullptr;
        }
        return strm;
    }
}
int main(int argc, char **argv) {
    if (argc < 3) {
        std::cout << "USAGE: " << argv[0] << " SRC DEST" << std::endl;
        std::cout << "SRC: html file or - for stdin to convert to json doc"
                  << std::endl;
        std::cout << "DEST: json file or - for stdout" << std::endl;
        return 0;
    }
    auto src = OpenRead(argv[1]);

    auto dest = OpenWrite(argv[2]);

    if (src == nullptr) {

        std::cerr << "ERROR: Input could not be read" << std::endl;
        return 1;
    }
    if (dest == nullptr) {
        std::cerr << "ERROR: Output could not be read" << std::endl;
        return 1;
    }

    StreamWriter writer(dest);

    auto html = Html::Parse(std::make_shared<StreamReader>(src));

    writer.WriteLine(Json::Json::Encode(Html2JToken(html)));

    return 0;
}