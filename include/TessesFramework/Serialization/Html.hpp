/*
    TessesFramework a library to make C++ easier for me, used in CrossLang:
    https://git.tesses.org/tesses50/crosslang

    Copyright (C) 2026 Mike Nolan
    SPDX-License-Identifier: GPL-3.0-or-later WITH TessesFramework-Exception-1.0

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#pragma once
#include "../TextStreams/TextReader.hpp"
namespace Tesses::Framework::Serialization::Html {

struct HtmlNode;
struct HtmlNode : public std::enable_shared_from_this<HtmlNode> {

    std::string text_or_tag;
    std::vector<std::pair<std::string, std::optional<std::string>>> attributes;
    std::vector<std::shared_ptr<HtmlNode>> children;
    bool isText = false;

    void FindNodes(std::string_view tag,
                   std::vector<std::shared_ptr<const HtmlNode>> &nodes) const;
    std::vector<std::shared_ptr<const HtmlNode>>
    FindNodes(std::string_view tag) const;

    void ToString(std::string &str) const;
    std::string ToString() const;

    void InnerText(std::string &str) const;
    std::string InnerText() const;
    static std::shared_ptr<HtmlNode> FromText(std::string_view text);
    static std::shared_ptr<HtmlNode>
    FromNode(std::string_view name,
             std::initializer_list<
                 std::pair<std::string, std::optional<std::string>>>
                 attrs,
             std::initializer_list<std::shared_ptr<HtmlNode>> nodes);
    static std::shared_ptr<HtmlNode> FromNode(
        std::string_view name,
        std::vector<std::pair<std::string, std::optional<std::string>>> attrs,
        std::vector<std::shared_ptr<HtmlNode>> nodes);

    template <typename IttrAttrs, typename IttrNodes>
    static std::shared_ptr<HtmlNode>
    FromNode(std::string_view name, IttrAttrs attrBegin, IttrAttrs attrEnd,
             IttrNodes nodesBegin, IttrNodes nodesEnd) {
        auto node = std::make_shared<HtmlNode>();
        node->isText = false;
        node->text_or_tag = std::string{name};
        node->attributes.insert(node->attributes.end(), attrBegin, attrEnd);
        node->children.insert(node->children.end(), nodesBegin, nodesEnd);
        return node;
    }
};

std::shared_ptr<HtmlNode>
Parse(std::shared_ptr<TextStreams::TextReader> reader);
} // namespace Tesses::Framework::Serialization::Html
