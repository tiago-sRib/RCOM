#include "linklayer.h"
#include "aux.h"

#define FLAG 0x7E
#define A 0x03
#define C 0x03
#define BCC A^C

// global var
int flag=1, conta=1;

void alarme();
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
int StateMachine(unsigned char tx, unsigned char rx, int state);

//funtions
/****************ALARME**************************/
void atende()                   // atende alarme
{
 	printf("alarme # %d\n", conta);
	flag=1;
	conta++;
}

void alarme()
{
    (void) signal(SIGALRM, atende);             // instala  rotina que atende interrupcao

    while(conta < 4){
        if(flag)        {alarm(3); flag=0;}     // activa alarme de 3s 
    }

    printf("Vou terminar.\n");
}
/************************************************/
//por a alarm(0) para cancelar

int llopen(linkLayer connectionParameters)
{
    int fd,c, res;
    struct termios oldtio,newtio;
    unsigned char buf[255];
    unsigned char UA[255];
    unsigned char SET[255];

    int i, sum = 0, speed = 0;
    
    fd = open(connectionParameters.serialPort, O_RDWR | O_NOCTTY );
    if (fd <0) {perror(argv[1]); exit(-1); }

    if ( tcgetattr(fd,&oldtio) == -1) { /* save current port settings */
      perror("tcgetattr");
      exit(-1);
    }

}

int StateMachine(unsigned char tx, unsigned char rx, int state)
{    
    switch(state)
    {
        case START_STATE: 
            if(tx == FLAG)          {rx=tx;   state = 1;}
            break;
        case FLAG_STATE:      
            if(tx == A)             {rx=tx;   state = 2;}
            else if(tx == FLAG)     {rx=tx;       state = 1;}
            else state = 0;
            break;
        case A_STATE:
            if(tx == C)             {rx=tx;   state = 3;}
            else if(tx == FLAG)     {rx=tx;       state = 1;}
            else state = 0;       
            break;
        case C_STATE:
            if(tx == BCC)           {rx=tx;   state = 4;}
            else if(tx == FLAG)     {rx=tx;       state = 1;}
            else state = 0;
            break;
        case BCC_STATE:
            if(tx == FLAG)          {rx=tx;   state = 5;}
            else state = 0;
            break;
        case STOP_STATE:
            break; 
    }

    return state;
}

