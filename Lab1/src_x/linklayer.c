#include "linklayer.h"
#include "aux.h"

// global var
int flag=1, conta=1, state;

void alarme(int timeout, int tries);
void atende(); //aux_alarme

// Opens a conection using the "port" parameters defined in struct linkLayer, returns "-1" on error and "1" on sucess
int llopen(linkLayer connectionParameters);
// Sends data in tx with size txSize
int llwrite(char* tx, int txSize);
// Receive data in packet
int llread(char* packet);
// Closes previously opened connection; if showStatistics==TRUE, link layer should print statistics in the console on close
int llclose(int showStatistics);

//aux.h
int StateMachine(unsigned char tx, int state);

//funtions
/****************ALARME**************************/
void atende()                   // atende alarme
{
 	printf("alarme # %d\n", conta);
	flag=1;
	conta++;
}

void alarme(int timeout, int tries)
{
    (void) signal(SIGALRM, atende);             // instala  rotina que atende interrupcao

    while(conta <= tries)
    {
        if(flag)        {alarm(timeout); flag=0;}     // activa alarme de 3s 
    }

    printf("Vou terminar.\n");
}
/************************************************/
//por a alarm(0) para cancelar

int llopen(linkLayer connectionParameters)
{
    int fd, flag_ret=0;
    struct termios oldtio,newtio;
    unsigned char buf[255];
    unsigned char UA[5];
    unsigned char SET[5];
    //(void) signal(SIGALRM, atende);
    int STOP;
    ssize_t res;

    int i=0, j=0, sum = 0, speed = 0, state=0;
    
    fd = open(connectionParameters.serialPort, O_RDWR | O_NOCTTY );
    if (fd <0) 
    {
        exit(-1);
    }

    if ((connectionParameters.role != TRANSMITTER) && (connectionParameters.role != RECEIVER))
    {
        printf("ERROR, MUST BE 0 OR 1\n");
        exit(-1);
    }

    if (connectionParameters.role == TRANSMITTER)
    {
        printf ("Transmitting Mode\n");
        SET[0] = FLAG;
        SET[1] = A;
        SET[2] = C;
        SET[3] = BCC;
        SET[4] = FLAG;
        write(fd, SET, 5);
        alarme(connectionParameters.timeOut, connectionParameters.numTries);

        while (state != STOP_STATE)
        {
            res = read(fd, &UA[i], 1);
            if(res != 0 || flag == 0)        //pkg recieved
                alarm(0);                       

            if(res == 0     || flag == 1)
                write(fd, SET, 5);              //falta encerrar o programa ao fim de tres tentativas

            printf("[%c]st: %d\n",UA[i], state);
            StateMachine(UA[i], state);
            i++;
        }
       
    }
    else if (connectionParameters.role == RECEIVER)
    {
        printf("Receiving Mode\n");
        while(state != STOP_STATE)
        {
        
            read(fd, &SET[i], 1);
            printf("[%c]st: %d\n",SET[i], state);
            StateMachine(SET[i], state);
            i++;
        }
        write(fd, UA, 5);
    }
    else
    {
        printf("ERROR CONNECTING\n");
        exit(-1);
    }

    bzero(&newtio, sizeof(newtio));
    newtio.c_cflag = connectionParameters.baudRate | CS8 | CLOCAL | CREAD;
    newtio.c_iflag = IGNPAR;
    newtio.c_oflag = 0;

    /* set input mode (non-canonical, no echo,...) */
    newtio.c_lflag = 0;

    newtio.c_cc[VTIME]    = 1;   /* inter-character timer unused */
    newtio.c_cc[VMIN]     = 0;   /* blocking read until 1 chars received */

    if ( tcgetattr(fd,&oldtio) == -1) { /* save current port settings */
      perror("tcgetattr");
      exit(-1);
    }

    return 1;
}

int StateMachine(unsigned char tx, int state)
{    
    switch(state)
    {
        case START_STATE: 
            if(tx == FLAG)          state = 1;
            break;
        case FLAG_STATE:      
            if(tx == A)             state = 2;
            else if(tx == FLAG)     state = 1;
            else state = 0;
            break;
        case A_STATE:
            if(tx == C)             state = 3;
            else if(tx == FLAG)     state = 1;
            else state = 0;       
            break;
        case C_STATE:
            if(tx == BCC)           state = 4;
            else if(tx == FLAG)     state = 1;
            else state = 0;
            break;
        case BCC_STATE:
            if(tx == FLAG)          state = 5;
            else state = 0;
            break;
        case STOP_STATE:
            break; 
    }
    return state;
}

int llwrite(char* tx, int txSize)
{
    return 0;
}

int llread(char* packet)
{
    return 0;
}

int llclose(int showStatistics)
{
    return 0;
}