#include "crow_all.h"
#include <iostream>
#include <mutex>
#include <unordered_set>
#include <queue>
#include "multithreading.h"
#include <sys/ipc.h>
#include <sys/msg.h>
#include "bpt.h"

//#include "variables.h"
int main(){

    pid_t pid = fork();
    if(pid==0){
        MultiThreadingBPT mtbpt = MultiThreadingBPT();
        exit(0);
    }
    
        key_t key;
        int msgid;
        key = ftok("random", 65);
        msgid = msgget(key, 0666 | IPC_CREAT);
        struct mesg_buffer message;
        int totalCount=0;
        while(totalCount<10*5000){
            msgrcv(msgid, &message, sizeof(message), 0, 0);
            printf("got sum: %lld, got count %lld\n", message.sum, message.count);
            totalCount+=message.count;
        }
        
        msgctl(msgid, IPC_RMID, NULL);

    // crow::SimpleApp app; //define your crow application

    // //define your endpoint at the root directory
    // CROW_ROUTE(app, "/")([](){
    //     return "Hello world";
    // });

    // std::mutex mtx;
    // std::unordered_set<crow::websocket::connection*> users;
    // // Websoket server
    // CROW_WEBSOCKET_ROUTE(app, "/ws")
    //   .onopen([&](crow::websocket::connection& conn) {
    //       CROW_LOG_INFO << "new websocket connection from " << conn.get_remote_ip();
    //       std::lock_guard<std::mutex> _(mtx);
    //       users.insert(&conn);
    //   })
    //   .onclose([&](crow::websocket::connection& conn, const std::string& reason) {
    //       CROW_LOG_INFO << "websocket connection closed: " << reason;
    //       std::lock_guard<std::mutex> _(mtx);
    //       users.erase(&conn);
    //   })
    //   .onmessage([&](crow::websocket::connection& /*conn*/, const std::string& data, bool is_binary) {
    //       std::lock_guard<std::mutex> _(mtx);
    //       std::cout<<data<<std::endl;
    //       for (auto u : users)
    //           if (is_binary)
    //               u->send_binary(data);
    //           else
    //               u->send_text(data);
    //   });
    // //set the port, set the app to run on multiple threads, and run the app
    // app.port(18080).multithreaded().run();
}