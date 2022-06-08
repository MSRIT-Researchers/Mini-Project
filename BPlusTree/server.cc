#include "crow_all.h"
#include <iostream>

int main()
{
    crow::SimpleApp app; //define your crow application

    //define your endpoint at the root directory
    CROW_ROUTE(app, "/")([](){
        return "Hello world";
    });

    // Websoket server
    CROW_WEBSOCKET_ROUTE(app, "/ws")
        .onopen([&](crow::websocket::connection& conn){
                // Called when websocket server is launched.
                std::cout<<"Websocket server running"<<std::endl;
                })
        .onclose([&](crow::websocket::connection& conn, const std::string& reason){
                // Called when websocket server is closed.
                std::cout<<"Wwbsocket server terminated"<<std::endl;
                })
        .onmessage([&](crow::websocket::connection& /*conn*/, const std::string& data, bool is_binary){
                // Called when websocket server receives a message.
                    std::cout<<data<<std::endl;
                });

    //set the port, set the app to run on multiple threads, and run the app
    app.port(18080).multithreaded().run();
}