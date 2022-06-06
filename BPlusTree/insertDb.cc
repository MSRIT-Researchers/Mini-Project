#include<string>
#include<stdio.h>
#include<stdlib.h>
#include<algorithm>
#include<vector>
#include "bpt.h"
#include "variables.h"

using namespace bpt;

void getStringFromInt(int i, char* dest);

int main(void){

    bplus_tree database(DB_NAME);
    
    for(int i=0; i<RANGE*THREAD_NUM; ++i){
        std::string tempKey = std::to_string(i);
        char str[tempKey.length()];
        strcpy(str, tempKey.c_str());
        bpt::key_t key(str);
        
        database.insert(key, i);
    }   


    return 0;
}