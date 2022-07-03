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

int limit = 10*6000;

void listenToStream(){
        key_t key = ftok("random", 65);
        int msgid = msgget(key, 0666 | IPC_CREAT);
        key_t key2 = ftok("server.cc", 64);
        int msgid2 = msgget(key2, 0666 | IPC_CREAT);
        struct mesg_buffer message, message2;
        int totalCount=0;
        while(totalCount<limit){
            msgrcv(msgid, &message, sizeof(message), 0, 0);
            // printf("Processed %d records\n", totalCount);
            totalCount+=message.count;
            message2.count = totalCount;
            message2.sum = 1;
            // printf("sending message2.count %lld\n", message2.count);
            msgsnd(msgid2, &message2, sizeof(message2), 0);     
        }

        printf("totalCount %d\n", totalCount);
        msgctl(msgid, IPC_RMID, NULL);
}

void init(){
    pid_t pid = fork();
    if(pid==0){
        MultiThreadingBPT mtbpt = MultiThreadingBPT();
        exit(0);
    }
    // listenToStream();
    pid = fork();
    if(pid==0){
        listenToStream();
        exit(0);
    }

    return;
}

// int main(){

        // MultiThreadingBPT mtbpt = MultiThreadingBPT();
        // init();
// }
int main(){
    
    key_t key2 = ftok("server.cc", 64);
    int msgid2 = msgget(key2, 0666 | IPC_CREAT);
    msgctl(msgid2, IPC_RMID, NULL);
    msgid2 = msgget(key2, 0666 | IPC_CREAT);

    struct mesg_buffer message2;
    message2.count = 0;
    crow::SimpleApp app; //define your crow application

    //define your endpoint at the root directory
    CROW_ROUTE(app, "/")([](){
        return "Hello world";
    });

    std::mutex mtx;
    std::unordered_set<crow::websocket::connection*> users;
    crow::websocket::connection* current;
    long long i = 1;

    // Websoket server
    CROW_WEBSOCKET_ROUTE(app, "/ws")
      .onopen([&](crow::websocket::connection& conn) {
          CROW_LOG_INFO << "new websocket connection from " << conn.get_remote_ip();
          std::lock_guard<std::mutex> _(mtx);
          users.insert(&conn);
          current = &conn;
          i=0;

      })
      .onclose([&](crow::websocket::connection& conn, const std::string& reason) {
          CROW_LOG_INFO << "websocket connection closed: " << reason;
          std::lock_guard<std::mutex> _(mtx);
          users.erase(&conn);
      })
      .onmessage([&](crow::websocket::connection& /*conn*/, const std::string& data, bool is_binary) {
          std::lock_guard<std::mutex> _(mtx);
          if(data=="start"){
                std::cout<<data<<std::endl;
                message2.count = 0;
                init();
          }
          else if(data=="ping"){
                if(message2.count<limit){
                    msgrcv(msgid2, &message2, sizeof(message2), 0, 0);
                    // printf("Sending %d\n", message2.count);
                    current->send_text(std::to_string(message2.count));                
                }
                else{
                    current->send_text("end");                
    
                }
          }
          else if(data=="kill"){
              app.stop();
              while(wait(NULL)>0);
              msgctl(msgid2, IPC_RMID, NULL);

          }
      });
    //set the port, set the app to run on multiple threads, and run the app

    std::ifstream MyFile("../website/src/serverport");
    std::string port;
    getline(MyFile,port);
    int port_num = stoi(port);
    printf("Running on port %d\n", port_num);
    app.port(port_num).multithreaded().run();
}