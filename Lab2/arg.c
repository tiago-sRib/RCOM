//#include "../include/arg.h"
#include "download.h"
#include "macros.h"

void parse_input(char * arg, requestedData * data)
{
    char * token;

    if(!arg)
        return;
        
    if(strncmp("ftp://", arg, 6) != 0)
    {
        printf("Argument must start with 'ftp://' \n");
        return;
    }

    arg += 6 * sizeof(char);

    if ((strstr(arg, ":")==NULL) || (strstr(arg, "@") == NULL)) 
    {
        sprintf(data->user,"anonymous");
        sprintf(data->password,"pass");

        token = strtok(arg, "/");
        strncpy(data->host, token, MAX_STRING_SIZE);
        
    }
    else
    {
        token = strtok(arg, ":");
        strncpy(data->user, token, MAX_STRING_SIZE);
        token = strtok(NULL, "@");
        strncpy(data->password, token, MAX_STRING_SIZE);
        token = strtok(NULL, "/");
        strncpy(data->host, token, MAX_STRING_SIZE);
    }
    token = strtok(NULL, "\0");
    strncpy(data->url_path, token, MAX_STRING_SIZE);

    char * aux_token = token;
    int counter = 0;
    while ((aux_token = strstr(aux_token, "/") ) != NULL)
    {
        counter++;
        aux_token += sizeof(char);
    }

    if(counter > 0){
        while ((token = strstr(token, "/") ) != NULL)
        {
            token += sizeof(char);
            aux_token = token;    
        }
        strncpy(data->file_name, aux_token, MAX_STRING_SIZE);
    }
    else strncpy(data->file_name, data->url_path, MAX_STRING_SIZE);
}


void print_data_struct(requestedData * data)
{
    printf("Data inserted:\n");
    printf("\tUser: %s\n"         , data->user);
    printf("\tPassword: %s\n"     , data->password);
    printf("\tHost: %s\n"         , data->host);
    printf("\tUrl: %s\n"          , data->url_path);
    printf("\tFile Name: %s\n\n"    , data->file_name);
}