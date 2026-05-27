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

#pragma once
#include "../Streams/Stream.hpp"
namespace Tesses::Framework::Mail {
class SMTPBody {
  public:
    std::string mimeType;
    virtual void
    Write(std::shared_ptr<Tesses::Framework::Streams::Stream> strm) = 0;
    virtual ~SMTPBody();
};

class SMTPStringBody : public SMTPBody {
  public:
    SMTPStringBody();
    SMTPStringBody(std::string text, std::string mimeType);
    std::string text;
    void Write(std::shared_ptr<Tesses::Framework::Streams::Stream> strm);
};

class SMTPStreamBody : public SMTPBody {
    std::shared_ptr<Tesses::Framework::Streams::Stream> stream;

  public:
    SMTPStreamBody(std::string mimeType,
                   std::shared_ptr<Tesses::Framework::Streams::Stream> stream);
    void Write(std::shared_ptr<Tesses::Framework::Streams::Stream> strm);
    ~SMTPStreamBody();
};

class SMTPClient {
    std::shared_ptr<Tesses::Framework::Streams::Stream> strm;

  public:
    SMTPClient(std::shared_ptr<Tesses::Framework::Streams::Stream> strm);
    std::string domain;
    std::string username;
    std::string password;
    std::string from;
    std::string from_name;
    std::string to;
    std::string subject;
    std::shared_ptr<SMTPBody> body;
    std::vector<std::pair<std::string, std::shared_ptr<SMTPBody>>> attachments;
    void Send();
    ~SMTPClient();
};

} // namespace Tesses::Framework::Mail