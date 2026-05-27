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
#include <cstddef>
#include <cstdint>
#include <functional>
#include <string>
#include <vector>
namespace Tesses::Framework::Http {
class WebSocketMessage {
  public:
    std::vector<uint8_t> data;
    bool isBinary;
    WebSocketMessage();
    WebSocketMessage(std::vector<uint8_t> data);
    WebSocketMessage(const void *data, size_t len);
    WebSocketMessage(std::string message);
    std::string ToString();
};
void SendWebSocketMessage(std::function<void(WebSocketMessage &)> cb,
                          std::string text);
class WebSocketConnection {

  public:
    virtual void OnOpen(std::function<void(WebSocketMessage &)> sendMessage,
                        std::function<void()> ping,
                        std::function<void()> close) = 0;
    virtual void OnReceive(WebSocketMessage &message) = 0;
    virtual void OnClose(bool clean) = 0;
    virtual ~WebSocketConnection();
};
class CallbackWebSocketConnection : public WebSocketConnection {
  public:
    std::function<void(std::function<void(WebSocketMessage &)>,
                       std::function<void()>, std::function<void()>)>
        onOpen;
    std::function<void(WebSocketMessage &)> onReceive;
    std::function<void(bool)> onClose;
    CallbackWebSocketConnection();
    CallbackWebSocketConnection(
        std::function<void(std::function<void(WebSocketMessage &)>,
                           std::function<void()>, std::function<void()>)>
            onOpen,
        std::function<void(WebSocketMessage &)> onReceive,
        std::function<void(bool)> onClose);

    void OnOpen(std::function<void(WebSocketMessage &)> sendMessage,
                std::function<void()> ping, std::function<void()> closeFn);
    void OnReceive(WebSocketMessage &message);
    void OnClose(bool clean);
};
} // namespace Tesses::Framework::Http