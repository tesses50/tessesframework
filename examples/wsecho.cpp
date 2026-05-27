#include "TessesFramework/Http/HttpClient.hpp"
#include "TessesFramework/Threading/Thread.hpp"
#include <iostream>
using namespace Tesses::Framework::Http;

class WebSocketConn : public WebSocketConnection {

  public:
    void OnOpen(std::function<void(WebSocketMessage &)> sendMessage,
                std::function<void()> ping, std::function<void()> close) {
        while (true) {
            std::cout << "> ";
            std::string req;
            std::getline(std::cin, req);
            if (req == "exit")
                break;
            else
                SendWebSocketMessage(sendMessage, req);
        }
        close();
    }
    void OnReceive(WebSocketMessage &message) {
        std::cout << "Message: " << message.ToString() << std::endl;
    }
    void OnClose(bool clean) {
        std::cout << (clean ? "Closed cleanly" : "Closed unclean") << std::endl;
    }
};

int main(int argc, char **argv) {
    Tesses::Framework::TF_Init();
    HttpDictionary reqHeaders;
    std::shared_ptr<WebSocketConn> conn = std::make_shared<WebSocketConn>();
    WebSocketClient("ws://echo.websocket.org/", reqHeaders, conn);

    return 0;
}
