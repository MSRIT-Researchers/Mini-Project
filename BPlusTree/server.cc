#include "crow_all.h"
#include <iostream>
#include <mutex>
#include <unordered_set>
#include <queue>
#include "multithreading.h"
#include <sys/ipc.h>
#include <sys/msg.h>
#include "bpt.h"
#include <sys/wait.h>
#include <fstream>

void listenToStream(long long* count){
      key_t key;
        int msgid;
        key = ftok("random", 65);
        msgid = msgget(key, 0666 | IPC_CREAT);
        struct mesg_buffer message;
        int totalCount=0;
        while(totalCount<10*50000){
            msgrcv(msgid, &message, sizeof(message), 0, 0);
            printf("Processed %d records\n", totalCount);
            totalCount+=message.count;
            *count = totalCount;
        }
        msgctl(msgid, IPC_RMID, NULL);
}

void init(long long *count){
    pid_t pid = fork();
    if(pid==0){
        MultiThreadingBPT mtbpt = MultiThreadingBPT();
        exit(0);
    }
    pid =  fork();
    if(pid==0){
        listenToStream(count);
        exit(0);
    }
    return;
}

int main(){
    
    
    crow::SimpleApp app; //define your crow application

    //define your endpoint at the root directory
    CROW_ROUTE(app, "/")([](){
        return "Hello world";
    });

    std::mutex mtx;
    std::unordered_set<crow::websocket::connection*> users;
    crow::websocket::connection* current;
    long long i = 1;
    long long count =0;
    // Websoket server
    CROW_WEBSOCKET_ROUTE(app, "/ws")
      .onopen([&](crow::websocket::connection& conn) {
          CROW_LOG_INFO << "new websocket connection from " << conn.get_remote_ip();
          std::lock_guard<std::mutex> _(mtx);
          users.insert(&conn);
          current = &conn;
          count=0;
          i=0;

      })
      .onclose([&](crow::websocket::connection& conn, const std::string& reason) {
          CROW_LOG_INFO << "websocket connection closed: " << reason;
          std::lock_guard<std::mutex> _(mtx);
          users.erase(&conn);
      })
      .onmessage([&](crow::websocket::connection& /*conn*/, const std::string& data, bool is_binary) {
          std::lock_guard<std::mutex> _(mtx);
          std::cout<<data<<std::endl;
          if(data=="Start"){
                if(count==0){
                    init(&count);
                }
                if(count<=10*50000){
                    printf("Sending %lld\n", count);
                    current->send_text(std::to_string(count));                
                }
          }
          else if(data=="kill"){
              app.stop();
              wait(NULL);
          }
      });
    //set the port, set the app to run on multiple threads, and run the app

    std::ifstream MyFile("../website/src/serverport");
    std::string port;
    getline(MyFile,port);
    int port_num = stoi(port);
    app.port(port_num).multithreaded().run();
}