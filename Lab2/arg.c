//#include "../include/arg.h"
#include "download.h"
#include "macros.h"

void parse_input(char * arg, requestedData * data){
    char * token;

    if(!arg)
        return;
        

    if(strncmp("ftp://", arg, 6) != 0){
        printf("Argument must start with 'ftp://' \n");
        return;
    }

    arg += 6;

    //printf("Url without 'ftp': %s\n", arg);

    token = strtok(arg, ":");
    strcpy(data->user, token);
    token = strtok(NULL, "@");
    strcpy(data->password, token);
    token = strtok(NULL, "/");
    strcpy(data->host, token);
    token = strtok(NULL, "\0");
    strcpy(data->url_path, token);
}

void print_data_struct(requestedData * data){
    printf("User: %s\n", data->user);
    printf("Password: %s\n", data->password);
    printf("Host: %s\n", data->host);
    printf("Url: %s\n", data->url_path);
}