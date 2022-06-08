//#include "../include/download.h"
#include "download.h"
#include "macros.h"


int main(int argc, char *argv[])
{	

    requestedData data;

    if(argc != 2){
        printf("Invalid arguments. Example:: ftp://[<user>:<password>@]<host>/<url-path>\n");
        return -1;
    }

    parse_input(argv[1], &data);
	print_data_struct(&data);

    struct hostent *h = getIP(data); 
	char *ip = inet_ntoa(*((struct in_addr *)h->h_addr));

	printf("\nRequesting socket to port 21\n");
	int socket_request = socket_config(ip, SERVER_PORT);
	printf("Connected\n");

	FILE * socket_data = fdopen(socket_request, "r");

	if (read_reply(socket_data, NULL) != 0)
	{
		printf("ERROR IN COMMANDS\n");
		close(socket_request);
		fclose(socket_data);
		return -1;
	}

  	// login
	char command[MAX_STRING_SIZE*2];
	sprintf(command, "user %s\n", data.user);
	printf ("COMMAND: %s\n", command);
	send_command(socket_request, command);
	if (read_reply(socket_data, NULL) != 0)
	{
		printf("ERROR IN COMMANDS\n");
		close(socket_request);
		fclose(socket_data);
		return -1;
	}
	
	sprintf(command, "pass %s\n", data.password);
	printf ("COMMAND: %s\n", command);
	send_command(socket_request, command);
	if (read_reply(socket_data, NULL) != 0)
	{
		printf("ERROR IN COMMANDS\n");
		close(socket_request);
		fclose(socket_data);
		return -1;
	}
	
	int porta;
	if((porta = pasv_mode(socket_request)) < 0)
	{
		printf("ERROR IN COMMANDS\n");
		close(socket_request);
		return -1;
	}

	printf("\nRequesting socket to port %d\n", porta);
	int download_fd = socket_config(ip, porta);
	printf("Connected\n");

	sprintf(command, "retr %s\n", data.url_path);
	printf ("COMMAND: %s\n", command);
	send_command(socket_request, command);
	if (read_reply(socket_data, NULL) != 0)
	{
		printf("ERROR IN COMMANDS\n");
		close(socket_request);
		fclose(socket_data);
		close(download_fd);
		return -1;
	}

	if(download(download_fd, data.file_name)!=0)
	{
		printf("ERROR DOWNLOADING FILE\n");
		close(socket_request);
		fclose(socket_data);
		close(download_fd);
		return -1;
	}

	if (read_reply(socket_data, NULL) != 0)
	{
		printf("ERROR IN COMMANDS\n");
		close(socket_request);
		fclose(socket_data);
		close(download_fd);
		return -1;
	}

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

int send_command(int socketfd, char *command)
{
		
	int bytes_sent = send(socketfd, command, strlen(command), 0);
	
	if (bytes_sent <= 0)
	{
		printf("Error Sending Command\n");
		return -1;
	}
	
	//printf("Command sent: %s with %d bytes\n", command, bytes_sent);
	return 0;
}

int read_reply(FILE * socketf, char copy[])
{
	char buf [2048];

	while(fgets(buf, 2048, socketf))
	{
		printf("< %s", buf);
		if (copy != NULL)
			strcpy(copy, buf);

		if (buf[0] == '4' || buf[0] == '5')
		{
			printf("Connection error\n");
			return -1;
		}

		if(buf[3] == ' ')
			break;

	}
	return 0;
}

int pasv_mode(int socketfd)
{
	char command[MAX_STRING_SIZE*2];
	char buf[MAX_STRING_SIZE*4];
	char *token;
	int real_port;
	
	int port[2];

	sprintf(command, "pasv\n");
	printf ("COMMAND: %s\n", command);
	send_command(socketfd, command);

	FILE *socketf = fdopen(socketfd, "r");
	if(read_reply(socketf, buf) < 0){
		return -1;
	}

	strtok(buf, "(");
	for (int i=0; i<4; i++)
		token = strtok(NULL, ",");
		
	token = strtok(NULL, ",");
	port[0] = (int)strtol(token, NULL, 10);
	token = strtok(NULL, ")");
	port[1] = (int)strtol(token, NULL, 10);

	real_port = port[0]*256 + port[1];
	//printf("PORTA %d\n", real_port);

	return real_port;
}

int download(int socketfd, char *filename)
{
	FILE *file = fopen(filename, "w+");
	char buf[MAX_STRING_SIZE*2];
	int bytes_read;

	if(!file)
	{
		printf("ERROR CREATING FILE FOR DOWNLOAD\n");
		return -1;
	}

	while((bytes_read = recv(socketfd, buf, MAX_STRING_SIZE*2, 0)) > 0)
		fwrite(buf, bytes_read, 1, file);

	fclose(file);

	return 0;
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
    printf("IP Address : %s\n",inet_ntoa(*((struct in_addr *)h->h_addr)));

    return h;
}
