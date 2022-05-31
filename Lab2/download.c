//#include "../include/download.h"
#include "download.h"
#include "macros.h"

int main(int argc, char *argv[])
{

    requestedData data;
    if(argc != 2){
        printf("Invalid arguments. Exemple:: ftp://[<user>:<password>@]<host>/<url-path>\n");
        return -1;
    }

    parse_input(argv[1], &data);
	print_data_struct(&data);

    struct hostent *h = getIP(data); 
	char *ip = inet_ntoa(*((struct in_addr *)h->h_addr));

	printf("Requesting socket to port 21\n");
	int socket_request = socket_config(ip, SERVER_PORT);
	
	char buf[2048] = "12345";
	int buf_size;
	//buf_size = read_socket_reply(socket_request, buf);
	buf_size = send(socket_request, buf, strlen(buf), 0);
	printf("Bytes escritos %d\n", buf_size);
    

	return 0;
}

int socket_config (char *ip, int port)
{
	int	sockfd;
	struct sockaddr_in server_addr;
	/*char buf[] = "Mensagem de teste na travessia da pilha TCP/IP\n";  
	int	bytes;*/
	
	/*server address handling*/
	bzero((char*)&server_addr,sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = inet_addr(ip);	/*32 bit Internet address network byte ordered*/
	server_addr.sin_port = htons(port);		/*server TCP port must be network byte ordered */
    
	/*open an TCP socket*/
	if ((sockfd = socket(AF_INET,SOCK_STREAM,0)) < 0) 
	{
    	perror("socket()");
		exit(0);
    }
	/*connect to the server*/
    	if(connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
		{
    	perror("connect()");
		exit(0);
		}
    	/*send a string to the server*/
	

	/*close(sockfd);
	exit(0);*/


	return sockfd;
}

int read_socket_reply(int socketfd, unsigned char * buf)
{
	int res;
	
	res = recv(socketfd, buf, 2048, 0);
	
	if(res == -1)
	{
		printf("Error getting response\n");
		return NULL;
	}
	else if(res == 0)
	{
		printf("Host closed connection\n");
		return NULL;
	}

	buf[res-1] = NULL;

	return res;
}


struct hostent *getIP(requestedData data)
{
    struct hostent *h;
    
    if ((h=gethostbyname(data.host)) == NULL) 
	{  
        herror("gethostbyname");
        return NULL;
    }

    printf("Host name  : %s\n", h->h_name);
    printf("IP Address : %s\n",inet_ntoa(*((struct in_addr *)h->h_addr_list)));

    return h;
}
