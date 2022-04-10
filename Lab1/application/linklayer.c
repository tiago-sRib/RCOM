#include "linklayer.h"
#include "aux.h"

// global var
int attempts=1, timeOutFLAG=1; // numTries, timeOut; ?? 
int fd;
struct termios oldtio,newtio;
linkLayer cP;

//funtions
void timeOut()
{
    printf("alarme # %d\n", attempts);
    timeOutFLAG=1;
    attempts++;
}

int llopen(linkLayer connectionParameters)
{
    unsigned char b;
    unsigned char buf[5];
    int state = START_STATE;

    ssize_t res;
    llcopy(connectionParameters);
    
    fd = connectionConfig(connectionParameters);

    if(fd == -1){
        puts("ERROR CONNECTING fd");
        return -1;
    }

    switch (cP.role)
    {
    case TRANSMITTER:
        createPkg(SET_pkg, buf);
    
        (void) signal(SIGALRM, timeOut);

        write(fd, buf, 5);
        while(attempts <= cP.numTries && state != STOP_STATE){
            
            res = read(fd, &b, 1);
            
            if(res == 0 && timeOutFLAG){
                alarm(cP.timeOut);
                timeOutFLAG = 0;
                write(fd, buf, 5);
            }

            if(res)
            {
                alarm(0);
                printf("[%d]st: ", state);
                printFlags(b);
                printf("-> ");
                state = StateMachineUA(b, state); 
            }   
        }

        if(attempts > cP.numTries){
            puts("Number of tries exceded");
            exit(-1);
        }
        break;
    
    case RECEIVER:

        while(state != STOP_STATE)
        {
            read(fd, &b, 1);
            printf("[%d]st: ", state);
            printFlags(b);
            printf("-> ");
            state = StateMachineSET(b, state);
        }

        createPkg(UA_pkg, buf);
        write(fd, buf, 5);
        break;

    default:
        printf("ERROR CONNECTING, MUST BE 0 OR 1\n");
        exit(-1);
        break;
    }

    sleep(1);
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
    unsigned char b, buf[30];
    int state = START_STATE;
    ssize_t res;

    switch (cP.role)
    {
    case TRANSMITTER:
        createPkg(DISC_pkg, buf);

        (void) signal(SIGALRM, timeOut);

        write(fd, buf, 5);
        while(attempts <= cP.numTries && state != STOP_STATE){
            
            res = read(fd, &b, 1);
            
            if(res == 0 && timeOutFLAG){
                alarm(cP.timeOut);
                timeOutFLAG = 0;
                write(fd, buf, 5);
            }

            if(res)
            {
                alarm(0);
                printf("[%d]st: ", state);
                printFlags(b);
                printf("-> ");
                state = StateMachineDISC(b, state); 
            }   
        }

        if(attempts > cP.numTries){
            puts("Number of tries exceded");
            exit(-1);
        }

        createPkg(UA2_pkg, buf);
        write(fd, buf, 5);

    break;

    case RECEIVER:
        while(state != STOP_STATE)
        {
            read(fd, &b, 1);
            printf("[%d]st: ", state);
            printFlags(b);
            printf("-> ");
            state = StateMachineDISC(b, state);
        }

        createPkg(DISC_pkg, buf);
        write(fd, buf, 5);

        state = START_STATE;
        puts("DISC pkgs exchenged");
        while(state != STOP_STATE)
        {
            read(fd, &b, 1);
            printf("[%d]st: ", state);
            printFlags(b);
            printf("-> ");
            state = StateMachineUA2(b, state);
        }
        puts("Everithing went smooth");
    break;

    
    default:
        exit(-1);
        break;
    }

    close(fd);
    sleep(1);
    return 0;
}

void llcopy(linkLayer connectionParameters){
    cP.role = connectionParameters.role;
    cP.timeOut = connectionParameters.timeOut;
    cP.numTries = connectionParameters.numTries;
}

int connectionConfig(linkLayer connectionParameters){
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
    newtio.c_cflag = getBaud(connectionParameters.baudRate) | CS8 | CLOCAL | CREAD;
    newtio.c_iflag = IGNPAR;
    newtio.c_oflag = 0;

    /* set input mode (non-canonical, no echo,...) */
    newtio.c_lflag = 0;

    newtio.c_cc[VTIME]    = 0;   /* inter-character timer unused */
    newtio.c_cc[VMIN]     = 1;   /* blocking read until 1 chars received */

    tcflush(fd, TCIOFLUSH);

    if ( tcsetattr(fd,TCSANOW,&newtio) == -1) {
      perror("tcsetattr");
      exit(-1);
    }

    return fd;
}
