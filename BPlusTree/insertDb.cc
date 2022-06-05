#include<string>
#include<stdio.h>
#include<stdlib.h>
#include "bpt.h"

using namespace bpt;

int main(void){

    bplus_tree database("test.db");
    std::string tempKey = "";
    char str[10];
    for(int i=0; i<10; ++i){
        str[0] = i+'0';
        bpt::key_t key = str;
        database.insert(key, rand()/1000);
    }   


    return 0;
}