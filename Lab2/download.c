//#include "../include/download.h"
#include "download.h"
#include "macros.h"

int main(int argc, char *argv[]){

    requestedData data;
    if(argc != 2){
        printf("Invalid arguments. Exemple:: ftp://[<user>:<password>@]<host>/<url-path>\n");
        return -1;
    }

    parse_input(argv[1], &data);
    struct hostent *ent = getIP(data); 

    print_data_struct(&data);

    int	sockfd;
	struct sockaddr_in server_addr;
	char buf[] = "Mensagem de teste na travessia da pilha TCP/IP\n";  
	int	bytes;
	
	/*server address handling*/
	bzero((char*)&server_addr,sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = inet_addr(SERVER_ADDR);	/*32 bit Internet address network byte ordered*/
	server_addr.sin_port = htons(SERVER_PORT);		/*server TCP port must be network byte ordered */
    
	/*open an TCP socket*/
	if ((sockfd = socket(AF_INET,SOCK_STREAM,0)) < 0) {
    		perror("socket()");
        	exit(0);
    	}
	/*connect to the server*/
    	if(connect(sockfd, 
	           (struct sockaddr *)&server_addr, 
		   sizeof(server_addr)) < 0){
        	perror("connect()");
		exit(0);
	}
    	/*send a string to the server*/
	bytes = write(sockfd, buf, strlen(buf));
	printf("Bytes escritos %d\n", bytes);

	close(sockfd);
	exit(0);

    return 0;
}

struct hostent *getIP(requestedData data){
    struct hostent *h;
    
    if ((h=gethostbyname(data.host)) == NULL) {  
        herror("gethostbyname");
        return NULL;
    }

    printf("Host name  : %s\n", h->h_name);
    printf("IP Address : %s\n",inet_ntoa(*((struct in_addr *)h->h_addr_list)));

    return h;
}
