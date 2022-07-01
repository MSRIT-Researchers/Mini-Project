#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <vector>
#include <iostream>
#include <algorithm>
#include <ctime>
#include "bpt.h"
#include <chrono>
#include <queue>
#include <sys/wait.h>
#include <unistd.h>
#include "multithreading.h"
#include <sys/ipc.h>
#include <sys/msg.h>
#include "variables.h"

#define UNDERLINE "\033[4m"
#define CLOSEUNDERLINE "\033[0m"

// using namespace bpt;

    MultiThreadingBPT::MultiThreadingBPT() {
        bpt::bplus_tree database(DB_NAME); 
        bpt::meta_t meta = database.get_meta();

        int number_of_threads = meta.number_of_threads;
        // set the last offset as zero
        meta.thread_offsets[number_of_threads] = 0;

        // double singleProcessTime = computeUsingSingleProcess();
        double multiProcessesTime = computeUsingMultipleProcesses();

        // double percentage = (singleProcessTime/multiProcessesTime) ;
        // printf("Multiprocessing is %fx faster than a single process\n",percentage);
        // this->serverQ.push(101);

    }

    void MultiThreadingBPT::sendDataToMessageQ(int sum, int count){
        mesg_buffer message;
        message.count = count;
        message.sum = sum;

        key_t key;
        int msgid;
        key = ftok("random", 65);
        msgid = msgget(key, 0666 | IPC_CREAT);
        // printf("sending data here\n");
        int returnVal = msgsnd(msgid, &message, sizeof(message), 0);     
        // printf("sent\n return value: %d", returnVal);   
    }

    void MultiThreadingBPT::listenToMessageQ(){
        key_t key;
        int msgid;
        key = ftok("random", 65);
        msgid = msgget(key, 0666 | IPC_CREAT);
        struct mesg_buffer message;
        int totalCount=0;
        while(totalCount<THREAD_NUM*RANGE){
            msgrcv(msgid, &message, sizeof(message), 0, 0);
            printf("got sum: %lld, got count %lld\n", message.sum, message.count);
            totalCount+=message.count;
        }
        
        msgctl(msgid, IPC_RMID, NULL);
    }

    void MultiThreadingBPT::spawnChild(int i , int startOffset , int endOffset){
        pid_t pid = fork();
        if(pid == 0){
            // clock_t start = clock();
            multithread_aggregate(i,startOffset,endOffset);
            // clock_t end = clock();
            // printf("time taken by process %d is %f s\n", i, (end - start) / (double)(CLOCKS_PER_SEC));
            exit(0);
        }
    }

    // Get the time since epoch in milliseconds
    uint64_t MultiThreadingBPT::timeSinceEpochMillisec() {
        using namespace std::chrono;
        return duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
    }

    double MultiThreadingBPT::computeUsingMultipleProcesses(){

        bpt::bplus_tree database(DB_NAME);
        bpt::meta_t meta = database.get_meta();
        bpt::leaf_node_t leaf;

        printf(UNDERLINE "Multiple processes - Number of offsets: %ld\n\n" CLOSEUNDERLINE,meta.number_of_threads );
        for(size_t i=0; i<meta.number_of_threads; ++i){
            database.run_map(&leaf, meta.thread_offsets[i]);
            printf("Process %lu runs from offset with value: %d\n",i, leaf.children[0].value);
        }
        printf("\n");

        uint64_t startTime =timeSinceEpochMillisec();
        meta.thread_offsets[meta.number_of_threads] = 0;
        for (size_t i = 0; i < meta.number_of_threads ; ++i){
            spawnChild(i, meta.thread_offsets[i], meta.thread_offsets[i+1]);
        }

        

        // Wait for all processes to complete
        for(size_t i=0; i<meta.number_of_threads; i++){
            wait(NULL);
        }
        /// aggregate the results
        long long int fsum =0, fcount = 0;
        for(size_t i  =  0;i<meta.number_of_threads;i++){
            fsum += threadResults[i].first;
            fcount += threadResults[i].second;
        }

        // printf("Sum: %lld, count: %lld\n", fsum, fcount);   
        
        uint64_t endTime = timeSinceEpochMillisec();

        double multiProcessesTime = (endTime - startTime)/1000.0;
        printf("time taken by Multiprocessing: %f s\n\n", multiProcessesTime);

        return multiProcessesTime;
    }
    double MultiThreadingBPT::computeUsingSingleProcess(){

        clock_t start, end;
        bpt::bplus_tree database(DB_NAME);
        bpt::meta_t meta = database.get_meta();

        start = MultiThreadingBPT::timeSinceEpochMillisec();
        
        printf(UNDERLINE "\nSingle Process\n\n" CLOSEUNDERLINE);
        multithread_aggregate(0, meta.thread_offsets[0], 0);
        // printf("Sum : %lld ,Count : %lld \n", threadResults[0].first,threadResults[0].second);
        
        end = timeSinceEpochMillisec();

        double singleProcessTime = (end - start)/1000.0;
        printf("time taken by Single Process: %f s\n\n\n\n", singleProcessTime);
        return singleProcessTime;

    }
    void MultiThreadingBPT::multithread_aggregate(const int thread_number, off_t start_leaf_offset, off_t end_leaf_offset){
        long long sum = 0;
        long long c = 0;

       bpt::bplus_tree database(DB_NAME);
        bpt::leaf_node_t temp;
        database.run_map(&temp, start_leaf_offset);
        while (temp.next != end_leaf_offset){
            for (size_t i = 0; i < temp.n; ++i){
                sum += temp.children[i].value;
                c++;
            }
            if(c>=1000){
                sleep(1);
                sendDataToMessageQ(sum, c);
                c=0;
                sum=0;
            }
            database.run_map(&temp, temp.next);
        }
        // if(end_leaf_offset==0){
        for (size_t i = 0; i < temp.n; ++i){
                sum += temp.children[i].value;
                c++;
        }
        // std::cout<<"Sum: "<<sum<<" Count: "<<c<<std::endl;
        // this->serverQ.push(sum);

        printf("Done Processing Thread:%d\n", thread_number);
        sendDataToMessageQ(sum, c);
    }

    // void multithread_aggregate_last(const int thread_number, off_t start_leaf_offset)
    // {
    //     printf("multithreaded here\n");
    //     long long sum = 0;
    //     long long c = 0;

    //     bplus_tree database(DB_NAME);
    //     leaf_node_t temp;
    //     database.run_map(&temp, start_leaf_offset);

    //     off_t end_leaf_offset = 0;

    //     while (temp.next != end_leaf_offset)
    //     {
    //         for (int i = 0; i < temp.n; ++i)
    //         {
    //             sum += temp.children[i].value;
    //             c++;
    //         }
    //         database.run_map(&temp, temp.next);
    //     }

    //     for (int i = 0; i < temp.n; ++i)
    //     {
    //         sum += temp.children[i].value;
    //         c++;
    //     }
    //     database.run_map(&temp, temp.next);
    //     // printf("sum : %lld\n", sum);

    //     threadResults[thread_number] = {sum, c};
    // }
