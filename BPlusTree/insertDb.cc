#include<string>
#include<stdio.h>
#include<stdlib.h>
#include "bpt.h"
#include "variables.h"

using namespace bpt;

int main(void){

    bplus_tree database(DB_NAME);
    long long len = RANGE*THREAD_NUM;
    for(long long i=0; i<len; ++i){
        std::string tempKey = std::to_string(i);
        char str[tempKey.length()];
        strcpy(str, tempKey.c_str());
        bpt::key_t key(str);
        if(i%10000==0){
            printf("%lld records inserted\n", i);
        }
        if(i<len-10){
            database.insert(key, rand(),false);

        }
        else{
            database.insert(key, rand(),true);
        }
    }   


    return 0;
}