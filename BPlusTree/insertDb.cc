#include<string>
#include<stdio.h>
#include<stdlib.h>
#include "bpt.h"
#include "variables.h"

using namespace bpt;

int main(void){

    bplus_tree database(DB_NAME);
    long long len = 6362620;
    long long i=0;
    char buff[200];
    FILE* f = fopen("dataset.csv","r");
    fgets(buff,sizeof(buff),f);

    while(i<len && fgets(buff,sizeof(buff),f))
    {
        int k = 0;
        char* token = strtok(buff,",");
        while(k<2 && token!=NULL)
        {
            k++;
            token = strtok(NULL,",");
        }
        if(token=="1.0E7") token = "10000000.00";
        float t = std::stof(token);
        std::string tempKey = std::to_string(i);
        char str[tempKey.length()];
        strcpy(str, tempKey.c_str());
        bpt::key_t key(str);
        if(i%10000==0){
            printf("%lld records inserted\n", i);
        }
        if(i<len-10){
            database.insert(key, t,false);

        }
        else{
            database.insert(key, t,true);
        }
        i++;
    }
    // string line;
    // bplus_tree database(DB_NAME);
    // long long len = RANGE*THREAD_NUM;
    // fstream file (dataset.csv,ios::in);
    // for(long long i=0; i<len; ++i){
    //     std::string tempKey = std::to_string(i);
    //     char str[tempKey.length()];
    //     strcpy(str, tempKey.c_str());
    //     bpt::key_t key(str);
    //     if(i%10000==0){
    //         printf("%lld records inserted\n", i);
    //     }
    //     if(i<len-10){
    //         database.insert(key, rand(),false);

    //     }
    //     else{
    //         database.insert(key, rand(),true);
    //     }
    // }   


    return 0;
}