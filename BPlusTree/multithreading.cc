#include<stdio.h>
#include<stdlib.h>
#include<string>
#include<thread>
#include<vector>
#include<algorithm>
#include "bpt.h"
#include "variables.h"


std::pair<int, int> threadResults[THREAD_NUM];

void multithread(int left, int right, const int threadNumber);

using namespace bpt;

int main(void){

    bplus_tree database(DB_NAME);
    std::thread threads[THREAD_NUM];
    std::string str;

    for(int i=0; i<THREAD_NUM; ++i){
       
        
        std::thread (multithread, RANGE*i, RANGE*i+RANGE-1, i);
    }

    for(int i=0; i<THREAD_NUM; ++i){
        threads[i].join();
    }

    for(int i=0; i<THREAD_NUM; ++i){
        printf("%d %d", threadResults[i].first, threadResults[i].second);
    }
    
    return 0;
}

void multithread(int left, int right, const int threadNumber){
    
    
    std::string str = std::to_string(left);
    char leftStr[str.length()];
    strcpy(leftStr, str.c_str());
    str = std::to_string(right);
    char rightStr[str.size()];
    strcpy(rightStr,str.c_str());
    
    
    bplus_tree database(DB_NAME);

    bpt::key_t start(leftStr);
    value_t values[512];
    bool next = true;
    int sum = 0;
    int count=0;
    while (next) {
        int ret = database.search_range(&start,rightStr, values, 512, &next);
        if (ret < 0)
                break;

        for (int i = 0; i < ret; i++)
            count++;
            sum++;
        }

    threadResults[threadNumber] = {sum, count};
}