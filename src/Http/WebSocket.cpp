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

#include "TessesFramework/Http/WebSocket.hpp"
namespace Tesses::Framework::Http {

CallbackWebSocketConnection::CallbackWebSocketConnection() {}
CallbackWebSocketConnection::CallbackWebSocketConnection(
    std::function<void(std::function<void(WebSocketMessage &)>,
                       std::function<void()>, std::function<void()>)>
        onOpen,
    std::function<void(WebSocketMessage &)> onReceive,
    std::function<void(bool)> onClose) {
    this->onOpen = onOpen;
    this->onReceive = onReceive;
    this->onClose = onClose;
}

void CallbackWebSocketConnection::OnOpen(
    std::function<void(WebSocketMessage &)> sendMessage,
    std::function<void()> ping, std::function<void()> closeConnection) {
    if (this->onOpen)
        this->onOpen(sendMessage, ping, closeConnection);
}
void CallbackWebSocketConnection::OnReceive(WebSocketMessage &message) {
    if (this->onReceive)
        this->onReceive(message);
}
void CallbackWebSocketConnection::OnClose(bool clean) {
    if (this->onClose)
        this->onClose(clean);
}

WebSocketMessage::WebSocketMessage() {
    this->isBinary = false;
    this->data = {};
}
WebSocketMessage::WebSocketMessage(std::vector<uint8_t> data) {
    this->isBinary = true;
    this->data = data;
}
WebSocketMessage::WebSocketMessage(const void *data, size_t len) {
    this->isBinary = true;
    this->data = {};
    this->data.insert(this->data.end(), (uint8_t *)data,
                      ((uint8_t *)data) + len);
}
WebSocketMessage::WebSocketMessage(std::string message) {
    this->isBinary = false;
    this->data = {};
    this->data.insert(this->data.end(), message.begin(), message.end());
}
std::string WebSocketMessage::ToString() {
    std::string str = {};
    str.insert(str.end(), this->data.begin(), this->data.end());
    return str;
}

void SendWebSocketMessage(std::function<void(WebSocketMessage &)> cb,
                          std::string text) {
    WebSocketMessage message(text);
    cb(message);
}
} // namespace Tesses::Framework::Http