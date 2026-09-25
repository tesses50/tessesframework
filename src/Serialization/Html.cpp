#include "TessesFramework/Serialization/Html.hpp"
#include "TessesFramework/Console.hpp"
#include "TessesFramework/Http/HttpUtils.hpp"
#include <unordered_set>
namespace Tesses::Framework::Serialization::Html {

static std::unordered_set<std::string> html_nodes_with_no_body = {
    "area",  "base", "br",   "col",    "embed", "hr",  "img",
    "input", "link", "meta", "source", "track", "wbr",
};
std::string HtmlNode::ToString() const {
    std::string text;
    HtmlNode::ToString(text);
    return text;
}

void HtmlNode::InnerText(std::string &str) const {
    if (this->isText) {
        str += this->text_or_tag;
    } else {
        for (const auto &item : this->children)
            item->InnerText(str);
    }
}

std::string HtmlNode::InnerText() const {
    std::string innerText;
    InnerText(innerText);
    return innerText;
}

void HtmlNode::ToString(std::string &str) const {
    if (this->isText) {
        str += Http::HttpUtils::HtmlEncode(this->text_or_tag);
    } else {
        str += "<";
        str += this->text_or_tag;
        for (auto &attr : this->attributes) {
            str += ' ';
            str += attr.first;
            if (attr.second) {
                str += "=\"";
                str += Http::HttpUtils::HtmlEncode(attr.second.value());
                str += "\"";
            }
        }
        str += ">";

        if (html_nodes_with_no_body.count(this->text_or_tag) == 0) {
            for (auto &child : this->children)
                child->ToString(str);

            str += "</";
            str += this->text_or_tag;
            str += ">";
        }
    }
}
std::shared_ptr<HtmlNode> HtmlNode::FromText(std::string_view text) {
    auto node = std::make_shared<HtmlNode>();
    node->isText = true;
    node->text_or_tag = std::string{text};
    return node;
}
std::shared_ptr<HtmlNode> HtmlNode::FromNode(
    std::string_view name,
    std::initializer_list<std::pair<std::string, std::optional<std::string>>>
        attrs,
    std::initializer_list<std::shared_ptr<HtmlNode>> nodes) {
    return HtmlNode::FromNode(name, attrs.begin(), attrs.end(), nodes.begin(),
                              nodes.end());
}

std::shared_ptr<HtmlNode> HtmlNode::FromNode(
    std::string_view name,
    std::vector<std::pair<std::string, std::optional<std::string>>> attrs,
    std::vector<std::shared_ptr<HtmlNode>> nodes) {
    return HtmlNode::FromNode(name, attrs.begin(), attrs.end(), nodes.begin(),
                              nodes.end());
}

void HtmlNode::FindNodes(
    std::string_view tag,
    std::vector<std::shared_ptr<const HtmlNode>> &nodes) const {
    if (!this->isText) {
        if (this->text_or_tag == tag)
            nodes.push_back(this->shared_from_this());
        for (const auto &node : this->children)
            node->FindNodes(tag, nodes);
    }
}

std::vector<std::shared_ptr<const HtmlNode>>
HtmlNode::FindNodes(std::string_view tag) const {
    std::vector<std::shared_ptr<const HtmlNode>> nodes;
    FindNodes(tag, nodes);
    return nodes;
}

enum class HtmlLexTokenType { SYMBOL, TEXT, STRING };
struct HtmlLexToken {
    std::string text;
    HtmlLexTokenType type;
    HtmlLexToken(HtmlLexTokenType type, std::string_view text)
        : text(text), type(type) {}
};

struct HtmlLexer {
    std::shared_ptr<TextStreams::TextReader> reader;
    std::vector<HtmlLexToken> &tokens;
    int32_t _peeked = -1;
    std::string builder;
    bool ignoreSpaceMode = false;
    std::string nodeName = {};

    HtmlLexer(std::shared_ptr<TextStreams::TextReader> reader,
              std::vector<HtmlLexToken> &tokens)
        : reader(std::move(reader)), tokens(tokens) {}

    int32_t Peek() {
        if (_peeked != -1)
            return _peeked;
        _peeked = Read();
        return _peeked;
    }

    int64_t Read() {
        if (_peeked != -1) {
            auto tmp = _peeked;
            _peeked = -1;
            return tmp;
        }

        return reader->ReadChar();
    }

    void Flush() {
        if (!builder.empty()) {
            this->tokens.emplace_back(HtmlLexTokenType::TEXT, builder);

            if (ignoreSpaceMode && nodeName.empty())
                nodeName = builder;

            builder = "";
        }
    }

    void SkipTo(std::string_view fmt) {
        if (fmt.empty())
            return;
        std::string window;
        window.resize(fmt.size());

        while (fmt != window) {
            int32_t read = Read();
            if (read == -1)
                break;
            for (size_t i = 0; i < window.size() - 1; i++) {
                window[i] = window[i + 1];
            }

            window.back() = static_cast<char>(read);
        }
    }

    void Lex() {
        int read;
        while ((read = Read()) != -1) {
            int peek = Peek();
            switch (read) {
            case '"':
            case '\'': {
                if (ignoreSpaceMode) {
                    Flush();
                    char quote = static_cast<char>(read);
                    std::string val;
                    int c;
                    while ((c = Read()) != -1 && c != quote)
                        val += static_cast<char>(c);
                    tokens.emplace_back(HtmlLexTokenType::STRING, val);
                } else {
                    builder += static_cast<char>(read);
                }
                break;
            }
            case '/': {
                if (ignoreSpaceMode && peek == '>') {
                    Flush();
                    Read();
                    ignoreSpaceMode = false;
                    tokens.emplace_back(HtmlLexTokenType::SYMBOL, "/>");
                } else if (ignoreSpaceMode) {
                    builder += '/';
                } else {
                    builder += '/';
                }
                break;
            }
            case '>': {
                if (ignoreSpaceMode) {
                    Flush();
                    ignoreSpaceMode = false;
                    tokens.emplace_back(HtmlLexTokenType::SYMBOL, ">");

                    if (nodeName == "script" || nodeName == "style") {
                        nodeName = "_";
                        std::string builder2 = "";
                        bool eof = false;
                        while ((read = Read()) != -1) {
                            if (read == '<') {
                                peek = Peek();
                                if (peek == '/') {
                                    Read();
                                    break;
                                } else if (peek == -1) {
                                    eof = true;
                                    break;
                                } else {
                                    builder2 += '<';
                                }
                            } else {
                                builder2 += static_cast<char>(read);
                            }
                        }

                        if (read != -1 && !eof) {
                            tokens.emplace_back(HtmlLexTokenType::TEXT,
                                                builder2);

                            ignoreSpaceMode = true;
                            tokens.emplace_back(HtmlLexTokenType::SYMBOL, "</");

                        } else {
                            return;
                        }
                    }

                } else {
                    builder += '>';
                }
            } break;
            case '<': {
                Flush();
                if (peek == '!') {
                    //<!-- -->
                    //<!DOCTYPE html>
                    Read();
                    peek = Peek();
                    if (peek == '-') {
                        Read();
                        peek = Peek();
                        if (peek == '-') {
                            Read();
                            SkipTo("-->");

                        } else {
                            SkipTo(">");
                        }
                    } else {
                        SkipTo(">");
                    }

                } else if (peek == '/') {
                    Read();

                    ignoreSpaceMode = true;
                    tokens.emplace_back(HtmlLexTokenType::SYMBOL, "</");
                } else {

                    ignoreSpaceMode = true;
                    tokens.emplace_back(HtmlLexTokenType::SYMBOL, "<");
                    nodeName = {};
                }
            } break;
            case '=': {
                if (!ignoreSpaceMode) {
                    builder += "=";
                } else {
                    Flush();
                    tokens.emplace_back(HtmlLexTokenType::SYMBOL, "=");
                }
            } break;
            case ' ':
            case '\n':
            case '\t':
            case '\r':
                if (!ignoreSpaceMode) {
                    builder += static_cast<char>(read);
                } else {
                    Flush();
                }
                break;
            default:
                builder += static_cast<char>(read);
                break;
            }
        }
        Flush();
    }
};

struct HtmlParser {

    std::vector<HtmlLexToken> &tokens;
    size_t index = 0;
    HtmlParser(std::vector<HtmlLexToken> &tokens) : tokens(tokens) {}

    bool IsSymbol(std::string_view type) {
        if (Done())
            return false;
        auto &obj = Peak();
        if (obj.type != HtmlLexTokenType::SYMBOL)
            return false;

        if (obj.text != type)
            return false;
        index++;
        return true;
    }

    bool Done() { return index >= tokens.size(); }

    HtmlLexToken &Read() { return tokens[index++]; }
    HtmlLexToken &Peak() { return tokens[index]; }

    void Parse(std::string_view name, std::shared_ptr<HtmlNode> &node,
               bool root = false) {
        while (!Done()) {
            auto &curToken = Peak();
            switch (curToken.type) {
            case HtmlLexTokenType::TEXT: {
                Read();
                node->children.push_back(HtmlNode::FromText(
                    Http::HttpUtils::HtmlDecode(curToken.text)));
            } break;
            case HtmlLexTokenType::SYMBOL: {
                if (curToken.text == "<") {
                    Read();
                    if (Done()) {
                        return;
                    }
                    auto &val = Peak();
                    if (val.type == HtmlLexTokenType::TEXT) {
                        Read();

                        std::shared_ptr<HtmlNode> newNode =
                            std::make_shared<HtmlNode>();
                        newNode->isText = false;
                        newNode->text_or_tag = val.text;

                        while (!Done()) {
                            auto &k = Peak();

                            if (k.type == HtmlLexTokenType::TEXT) {
                                Read();
                                if (IsSymbol("=")) {
                                    if (!Done()) {
                                        auto &v = Peak();

                                        if (v.type ==
                                            HtmlLexTokenType::STRING) {
                                            Read();
                                            newNode->attributes[k.text] =
                                                Http::HttpUtils::HtmlDecode(
                                                    v.text);

                                            continue;
                                        }
                                    }
                                }
                                newNode->attributes[k.text] = std::nullopt;

                                continue;
                            }

                            if (k.type == HtmlLexTokenType::SYMBOL &&
                                (k.text == ">" || k.text == "/>"))
                                break;
                        }
                        if (!IsSymbol("/>")) {
                            IsSymbol(">");
                            if (html_nodes_with_no_body.count(val.text) == 0)
                                Parse(val.text, newNode);
                        }

                        node->children.push_back(newNode);
                    }
                } else if (curToken.text == "</") {
                    Read();
                    if (Done()) {
                        node->children.push_back(HtmlNode::FromText("</"));
                        return;
                    }
                    auto &val = Peak();
                    Read();
                    IsSymbol(">");

                    if (val.text == name && !root)
                        return;
                }
            } break;
            }
        }
    }

    std::shared_ptr<HtmlNode> Parse() {
        std::shared_ptr<HtmlNode> node = std::make_shared<HtmlNode>();
        node->isText = false;
        node->text_or_tag = ":root";
        Parse(":root", node, true);
        return node;
    }
};

std::shared_ptr<HtmlNode>
Parse(std::shared_ptr<TextStreams::TextReader> reader) {
    std::vector<HtmlLexToken> tokens;

    HtmlLexer lexer(reader, tokens);

    lexer.Lex();

    HtmlParser parser(tokens);

    return parser.Parse();
}

} // namespace Tesses::Framework::Serialization::Html