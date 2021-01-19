//
// Created by Aidan on 6/20/2018.
//

#pragma once

#include <discordpp/botStruct.hh>
#include <discordpp/log.hh>

#include "lib/simple-websocket-server/client_wss.hpp"
#include <nlohmann/json.hpp>

using json = nlohmann::json;
using WsClient = SimpleWeb::SocketClient<SimpleWeb::WSS>;

namespace discordpp {
template <class BASE>
class WebsocketSimpleWeb : public BASE, virtual BotStruct {
  public:
    virtual void
    initBot(unsigned int apiVersionIn, const std::string &tokenIn,
            std::shared_ptr<boost::asio::io_context> aiocIn) override {
        BASE::initBot(apiVersionIn, tokenIn, aiocIn);
    }

    virtual void send(const int opcode, sptr<const json> payload,
                      sptr<const handleSent> callback) override {
        json out{{"op", opcode},
                 {"d", ((payload == nullptr) ? json() : *payload)}};

        log::log(log::debug, [out](std::ostream *log) {
            *log << "Sending: " << out.dump(4) << '\n';
        });

        connection_->send(out.dump());
        if (callback != nullptr) {
            (*callback)();
        }
    };

  protected:
    void runctd() override {
        connect();

        BASE::runctd();
    }

    virtual void connect() override {
        connecting_ = true;
        log::log(log::info, [](std::ostream *log) {
            *log << "Fetching gateway..." << std::endl;
        });
        call(std::make_shared<Call>(Call{
            std::make_shared<std::string>("GET"),
            std::make_shared<std::string>("/gateway/bot"), nullptr, nullptr,
            std::make_shared<const handleRead>([this](const bool error,
                                                      const json &gateway) {
                if (error) {
                    log::log(log::info, [](std::ostream *log) {
                        *log << " Failed." << std::endl;
                    });
                    return;
                }

                log::log(log::info, [](std::ostream *log) {
                    *log << " Done." << std::endl;
                });

                connecting_ = false;
                log::log(log::trace, [gateway](std::ostream *log) {
                    *log << "Gateway: " << gateway.dump(2) << std::endl;
                });
                log::log(log::info, [this, gateway](std::ostream *log) {
                    *log << "WebSocket Address: "
                         << gateway["body"]["url"].get<std::string>().substr(6)
                         << ":443/?v=" << std::to_string(apiVersion)
                         << "&encoding=json" << std::endl;
                });

                ws_ = std::make_unique<WsClient>(
                    gateway["body"]["url"].get<std::string>().substr(6) +
                        ":443/"
                        "?v=" +
                        std::to_string(apiVersion) + "&encoding=json",
                    false);

                ws_->io_service = aioc;

                ws_->on_message =
                    [this](std::shared_ptr<WsClient::Connection> connection,
                           std::shared_ptr<WsClient::InMessage> in_message) {
                        json payload = json::parse(in_message->string());

                        log::log(log::trace, [payload](std::ostream *log) {
                            *log << "Message received: \"" << payload.dump(4)
                                 << "\"" << std::endl;
                        });

                        receivePayload(payload);
                    };

                ws_->on_open =
                    [this](std::shared_ptr<WsClient::Connection> connection) {
                        connected_ = true;
                        log::log(log::info, [](std::ostream *log) {
                            *log << " Done." << std::endl;
                        });
                        connection_ = connection;
                        log::log(log::info, [connection](std::ostream *log) {
                            *log << "WebSocket IP: "
                                 << connection->remote_endpoint().address()
                                 << std::endl;
                        });
                    };

                ws_->on_close =
                    [this](const std::shared_ptr<WsClient::Connection>
                               & /*connection*/,
                           int status, const std::string & /*reason*/) {
                        log::log(log::error, [status](std::ostream *log) {
                            *log
                                << "Sending: "
                                << "Client: Closed connection with status code "
                                << status << std::endl;
                        });
                        reconnect("The stream closed");
                    };

                // See
                // http://www.boost.org/doc/libs/1_55_0/doc/html/boost_asio/reference.html,
                // Error Codes for error code meanings
                ws_->on_error =
                    [this](const std::shared_ptr<WsClient::Connection>
                               & /*connection*/,
                           const SimpleWeb::error_code &ec) {
                        if (connected_) {
                            std::cout << "WebSocket Error: " << ec
                                      << ", error message: " << ec.message()
                                      << std::endl;
                            connected_ = false;
                            connect();
                        }
                    };

                ws_->config.timeout_request = 10;

                log::log(log::info,
                         [](std::ostream *log) { *log << "Connecting..."; });
                ws_->start();
            })}));
    }

    virtual void disconnect() override {
        connected_ = false;
        ws_->stop();
    }

  private:
    std::unique_ptr<WsClient> ws_;
    std::shared_ptr<WsClient::Connection> connection_;
};
} // namespace discordpp
