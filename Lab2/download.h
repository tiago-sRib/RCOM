#ifndef DOWNLOAD_H_
#define DOWNLOAD_H_

#include <string.h>   
#include <stdlib.h> 
#include <stdio.h> 
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <signal.h>
#include <netdb.h>
#include <strings.h>

#include "macros.h"

typedef struct requested_data{
    char user[MAX_STRING_SIZE]; 
    char password[MAX_STRING_SIZE];  
    char host[MAX_STRING_SIZE]; 
    char url_path[MAX_STRING_SIZE]; 
    char file_name[MAX_STRING_SIZE];
} requestedData;

void parse_input(char *arg, requestedData * data);
void print_data_struct(requestedData * data);

int socket_config (char *ip, int port);

struct hostent *getIP(requestedData data);

#endif