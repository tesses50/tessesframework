/*
    TessesFramework a library to make C++ easier for me, used in CrossLang:
   https://git.tesses.org/tesses50/crosslang Copyright (C) 2026 Mike Nolan

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

#include "TessesFramework/Http/ContentDisposition.hpp"
#include "TessesFramework/Http/HttpUtils.hpp"
#include <iostream>
namespace Tesses::Framework::Http {
bool ContentDisposition::TryParse(std::string str, ContentDisposition &cd) {
    auto res = HttpUtils::SplitString(str, "; ");

    if (res.empty())
        return false;
    cd.type = res[0];
    bool hasFileNameStar = false;
    for (size_t i = 1; i < res.size(); i++) {
        auto res2 = HttpUtils::SplitString(res[i], "=", 2);
        if (res2.size() == 2) {
            if (res2[0] == "filename*") {
                // cd.filename = res2[1];
                // UTF-8''
                std::string p = res2[1];
                if (p.find("UTF-8''") == 0) {
                    hasFileNameStar = true;
                    p = HttpUtils::UrlPathDecode(p.substr(7));
                    cd.filename = p;
                }
            } else if (res2[0] == "filename" && !hasFileNameStar) {
                std::string p = res2[1];
                if (!p.empty() && p[0] == '\"') {
                    p = p.substr(1, p.size() - 2);
                }

                p = HttpUtils::UrlPathDecode(p);

                cd.filename = p;
            } else if (res2[0] == "name") {
                std::string p = res2[1];
                if (!p.empty() && p[0] == '\"') {
                    p = p.substr(1, p.size() - 2);
                }

                cd.fieldName = HttpUtils::UrlPathDecode(p);
            }
        }
    }
    return true;
}

std::string ContentDisposition::ToString() {
    std::vector<std::string> parts;

    parts.push_back(this->type);

    if (!this->fieldName.empty()) {
        parts.push_back("name=\"" +
                        HttpUtils::UrlPathEncode(this->fieldName, true) + "\"");
    }

    if (!this->filename.empty()) {
        parts.push_back("filename=\"" +
                        HttpUtils::UrlPathEncode(this->filename, true) + "\"");
    }

    return HttpUtils::Join("; ", parts);
}
} // namespace Tesses::Framework::Http