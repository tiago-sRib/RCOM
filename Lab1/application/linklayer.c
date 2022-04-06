#include "linklayer.h"
#include "aux.h"

// global var
int fd, attempts=1, timeoutFLAG=1;
struct termios oldtio,newtio;

void atende();

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
    unsigned char x;
    unsigned char buf[5];
    int state = START_STATE;
    ssize_t res;

    //int i;
    
    fd = llopenfd(connectionParameters);
    if(fd == -1){
        puts("ERROR CONNECTING fd");
        return -1;
    }

    if ((connectionParameters.role != TRANSMITTER) && (connectionParameters.role != RECEIVER))
    {
        printf("ERROR CONNECTING, MUST BE 0 OR 1\n");
        exit(-1);
    }

    if (connectionParameters.role == TRANSMITTER)
    {
        printf ("Transmitting Mode\n");
        createPkg("SET", buf);
        (void) signal(SIGALRM, atende);

        write(fd, buf, 5);
        while (attempts <= connectionParameters.numTries && state != STOP_STATE){
            

            res = read(fd, &x, 1);
            
            if(res == 0 && timeoutFLAG){
                alarm(connectionParameters.timeOut);
                timeoutFLAG = 0;
                write(fd, buf, 5);
            }

            if(res)
            {
                alarm(0);             //pkg recieved

                //printf("[%d]st: ", state);
                //printFLAGS(x);
                //printf("-> ");
                state = StateMachine(x, state); 
            }   
        }
        if(attempts > connectionParameters.numTries){
                puts("Number of tries exceded");
                exit(-1);
        }
    }

    if (connectionParameters.role == RECEIVER)
    {
        printf("Receiving Mode\n");
        
        while(state != STOP_STATE)
        {
            read(fd, &x, 1);
            //printf("[%d]st: ", state);
            //printFLAGS(x);
            //printf("-> ");
            state = StateMachine(x, state);
        }

        createPkg("UA", buf);
        write(fd, buf, 5);
    }

    return 1;
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
    if ( tcsetattr(fd,TCSANOW,&oldtio) == -1) {
      perror("tcsetattr");
      exit(-1);
    }

    close(fd);
    return 0;
}

int llopenfd (linkLayer connectionParameters){
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

    return fd;
}