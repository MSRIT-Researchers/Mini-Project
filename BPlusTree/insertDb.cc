#include<string>
#include<stdio.h>
#include<stdlib.h>
#include "bpt.h"
#include "variables.h"

using namespace bpt;

bplus_tree database(DB_NAME);

void insert(int i, bool offsets){
        std::string tempKey = std::to_string(i);
        char str[tempKey.length()];
        strcpy(str, tempKey.c_str());
        bpt::key_t key(str);
        
        database.insert(key, rand()%1000, offsets);
}

int main(void){

    
    for(long long i=0; i<RANGE*THREAD_NUM-10000; ++i){
       insert(i, false);
    }   


    for(long long i=RANGE*THREAD_NUM-10000; i<RANGE*THREAD_NUM; ++i){
        insert(i, true);
    }

    return 0;
}