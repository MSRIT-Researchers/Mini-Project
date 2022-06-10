#include<string>
#include<stdio.h>
#include<stdlib.h>
#include<assert.h>
#include "bpt.h"
#include "variables.h"

using namespace bpt;

int main(void){

    bplus_tree database(DB_NAME);
    
    printf("*********Testing all insertions*******\n");
    for(int i=0; i<RANGE*THREAD_NUM; ++i){
        std::string tempKey = std::to_string(i);
        char str[tempKey.length()];
        strcpy(str, tempKey.c_str());
        bpt::key_t key(str);

        int result;
        database.insert(key, i);
        database.search(key, &result);
        assert(i==result);
    }   

    printf("*******All Test case passed!!!!!******\n");


    return 0;
}