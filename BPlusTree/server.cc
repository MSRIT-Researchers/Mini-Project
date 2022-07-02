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

//#include "variables.h"
void init(){
    pid_t pid = fork();
    if(pid==0){
        MultiThreadingBPT mtbpt = MultiThreadingBPT();
        exit(0);
    }
    
      
}

void listenToStream(crow::websocket::connection* user){

      key_t key;
        int msgid;
        key = ftok("random", 65);
        msgid = msgget(key, 0666 | IPC_CREAT);
        struct mesg_buffer message;
        int totalCount=0;
        int c= 10;
        while(totalCount<10*50000 && c--){

            // usleep(200*1000);
            msgrcv(msgid, &message, sizeof(message), 0, 0);
            // printf("got sum: %lld, got count %lld\n", message.sum, message.count);
            printf("Processed %d records\n", totalCount);
            // std::string str = "Processed"+std::to_string(totalCount)+" records\n";
            user->send_text(std::to_string(totalCount));

            totalCount+=message.count;
        }
        msgctl(msgid, IPC_RMID, NULL);
}
int main(){
    
    
    crow::SimpleApp app; //define your crow application

    //define your endpoint at the root directory
    CROW_ROUTE(app, "/")([](){
        return "Hello world";
    });

    std::mutex mtx;
    std::unordered_set<crow::websocket::connection*> users;
    // Websoket server
    CROW_WEBSOCKET_ROUTE(app, "/ws")
      .onopen([&](crow::websocket::connection& conn) {
          CROW_LOG_INFO << "new websocket connection from " << conn.get_remote_ip();
          std::lock_guard<std::mutex> _(mtx);
          users.insert(&conn);

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
            for(long long i = 1;i<=1e11;i++){
                for (auto u : users){
                    printf("Sending %lld\n", i);
                    u->send_text(std::to_string(i));
                }
            }
            // init();

            // for (auto u : users){
            //     listenToStream(u);
            // }
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