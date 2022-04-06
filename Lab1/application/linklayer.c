#include "linklayer.h"
#include "aux.h"

// global var
int fd, attempts=1, timeoutFLAG=1;

void atende();
int get_baud(int baud);
void printFLAGS(unsigned char x);
void createPkg(char *mode, unsigned char * pkg);
int StateMachine(unsigned char tx, int state);

// Opens a conection using the "port" parameters defined in struct linkLayer, returns "-1" on error and "1" on sucess
int llopen(linkLayer connectionParameters);
// Sends data in tx with size txSize
int llwrite(char* tx, int txSize);
// Receive data in packet
int llread(char* packet);
// Closes previously opened connection; if showStatistics==TRUE, link layer should print statistics in the console on close
int llclose(int showStatistics);


//funtions
void atende()                 // atende alarme
{
 	printf("alarme # %d\n", attempts);
    timeoutFLAG=1;
    attempts++;
}

int llopen(linkLayer connectionParameters)
{
    struct termios oldtio,newtio;
    unsigned char UA[5];
    unsigned char SET[5];
    int state = START_STATE;
    ssize_t res;

    int i, j;
    
    fd = open(connectionParameters.serialPort, O_RDWR | O_NOCTTY );
    if (fd <0) 
    {
        perror(connectionParameters.serialPort);
        exit(-1);
    }

    if ( tcgetattr(fd,&oldtio) == -1) 
    {                               /* save current port settings */
      perror("tcgetattr");
      exit(-1);
    }

    bzero(&newtio, sizeof(newtio));
    newtio.c_cflag = get_baud(connectionParameters.baudRate) | CS8 | CLOCAL | CREAD;
    newtio.c_iflag = IGNPAR;
    newtio.c_oflag = 0;

    /* set input mode (non-canonical, no echo,...) */
    newtio.c_lflag = 0;

    newtio.c_cc[VTIME]    = 1;   /* inter-character timer unused */
    newtio.c_cc[VMIN]     = 0;   /* blocking read until 1 chars received */

    tcflush(fd, TCIOFLUSH);

    if ( tcsetattr(fd,TCSANOW,&newtio) == -1) {
      perror("tcsetattr");
      exit(-1);
    }

    if ((connectionParameters.role != TRANSMITTER) && (connectionParameters.role != RECEIVER))
    {
        printf("ERROR CONNECTING, MUST BE 0 OR 1\n");
        exit(-1);
    }

    if (connectionParameters.role == TRANSMITTER)
    {
        printf ("Transmitting Mode\n");
        createPkg("SET", SET);
        (void) signal(SIGALRM, atende);

        while (attempts <= connectionParameters.numTries)
        {
            write(fd, SET, 5);

            i=0;
            while (state != STOP_STATE)
            {    
                res = read(fd, &UA[i], 1);

                if(res == 0 && timeoutFLAG)
                {
                    printf("trigger alarm\n");
                    alarm(connectionParameters.timeOut);
                    timeoutFLAG = 0;
                }

                if(res != 0)
                {
                    alarm(0);                    //pkg recieved
                
                    printf("[%d]st: ", state);
                    printFLAGS(UA[i]);
                    printf("-> ");
                    state = StateMachine(UA[i], state); 

                    i++;
                }            
            }
        }

        if(0){
                puts("numTries exceded");
                exit(-1);
        }
    }

    if (connectionParameters.role == RECEIVER)
    {
        printf("Receiving Mode\n");
        i = 0;
        while(state != STOP_STATE)
        {
            if(i==0) sleep(connectionParameters.timeOut*2);

            read(fd, &SET[i], 1);
            printf("[%d]st: ", state);
            printFLAGS(SET[i]);
            printf("-> ");
            state = StateMachine(SET[i], state);
            i++;
        }

        createPkg("UA", UA);
        write(fd, UA, 5);
    }

    return 1;
}

int StateMachine(unsigned char tx, int state)
{   
    switch(state)
    {
        case START_STATE: 
            if(tx == FLAG){
                state = FLAG_STATE;
            }
            break;
        case FLAG_STATE:      
            if(tx == A){
                state = A_STATE;
            }
            else if(tx == FLAG){    
                state = FLAG_STATE;
            }
            else state = START_STATE;
            break;
        case A_STATE:
            if(tx == C){
                state = C_STATE;
            }
            else if(tx == FLAG){
                state = FLAG_STATE;
            }
            else{
                state = START_STATE;
            }       
            break;
        case C_STATE:
            if(tx == BCC){
                state = BCC_STATE;
            }
            else if(tx == FLAG){
                state = FLAG_STATE;
            }
            else state = START_STATE;
            break;
        case BCC_STATE:
            if(tx == FLAG){
                state = STOP_STATE;
            }
            else{
                state = FLAG_STATE;
            }
            break;
        case STOP_STATE:
            break; 
    }

    printf("Going to [%d]st\n", state);
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

void printFLAGS(unsigned char x){
    switch (x)
    {
    case FLAG:
        printf("FLAG");
        break;
    case BCC:
        printf("BCC");
        break;
    case A:
        printf("A/C");
        break;
    default:
        printf("%u", x);
        break;
    }
}

int get_baud(int baud)
{
    switch (baud) {
    case 9600:
        return B9600;
    case 19200:
        return B19200;
    case 38400:
        return B38400;
    case 57600:
        return B57600;
    case 115200:
        return B115200;
    case 230400:
        return B230400;
    case 460800:
        return B460800;
    case 500000:
        return B500000;
    case 576000:
        return B576000;
    case 921600:
        return B921600;
    case 1000000:
        return B1000000;
    case 1152000:
        return B1152000;
    case 1500000:
        return B1500000;
    case 2000000:
        return B2000000;
    case 2500000:
        return B2500000;
    case 3000000:
        return B3000000;
    case 3500000:
        return B3500000;
    case 4000000:
        return B4000000;
    default: 
        return -1;
    }
}


void createPkg(char *mode, unsigned char * pkg)
{
    if(strcmp("SET", mode) == 0 || strcmp("UA", mode) == 0){
        pkg[0] = FLAG;  
        pkg[1] = A;
        pkg[2] = C;
        pkg[3] = BCC;
        pkg[4] = FLAG; 
    }
}