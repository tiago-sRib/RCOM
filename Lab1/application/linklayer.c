#include "linklayer.h"
#include "aux.h"

// global var
int attempts=1, timeOutFLAG=1; // numTries, timeOut; ?? 
static int fd;
struct termios oldtio,newtio;
linkLayer cP;

int C_I = 0b00, C_RR = 0b01;

//funtions
void timeOut()
{
    printf("Alarm #%d\n", attempts);
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
    connectionConfig(connectionParameters);

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
        exit(-1);
        break;
    }

    sleep(1);
    return 1;
}

int llwrite(char* buf, int bufSize)
{   
    if(buf == NULL)  exit(-1);
    printf("\nllwrite(): \n");
    
    unsigned char * pkg = NULL;
    unsigned char b;
    int size, state = START_STATE;
    ssize_t res;

    size = createInfoPkg((unsigned char *)buf, bufSize, C_I, pkg);
    write(fd, pkg, size);

    (void) signal(SIGALRM, timeOut);

    while(attempts <= cP.numTries && state != STOP_STATE){
        
        res = read(fd, &b, 1);
        
        if(res == 0 && timeOutFLAG){
            alarm(cP.timeOut);
            timeOutFLAG = 0;
            write(fd, pkg, 5);
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

    free(pkg);
    return 0;
}

int llread(char* packet)
{  
    unsigned char buf[5], b;
    int state = START_STATE;
    unsigned char Nr;
    
    
    while(state != STOP_STATE)
        {
            read(fd, &b, 1);
            printf("[%d]st: ", state);
            printFlags(b);
            printf("-> ");
            
            state = StateMachineRR(b, state, &Nr);
        }

        buf[0] = FLAG;
        buf[1] = A;
        write(fd, buf, 5);
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

void connectionConfig(linkLayer connectionParameters){
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
}

int createInfoPkg(unsigned char * data, int sizeData, unsigned char Ns, unsigned char * pkg)
{
    int extraSize = 0, HEADER = 4, endPkg = 2;
    int finalSize;
    
    for(int i = 0; i < sizeData; i++)
    {
        if(data[i] == FLAG || data[i] == ESC)
            extraSize++;
    }

    finalSize = HEADER + endPkg + sizeData + extraSize;
    pkg = calloc(sizeof(unsigned char), finalSize);
    
    Ns = 0b10;
    pkg[0] = FLAG;
    pkg[1] = A;
    pkg[2] = C_I;
    pkg[3] = A ^ pkg[2];
    pkg[finalSize - 2] = createBCC2(data, sizeData);
    pkg[finalSize - 1] = FLAG; 

    byte_stuffing(data, sizeData, (pkg + 4));
    
    return finalSize;
}

int byte_stuffing(unsigned char *buf, int bufSize, unsigned char *newbuf)
{
    int counter = 0;
    if ((!buf) || bufSize < 0 || !newbuf)
    {
        puts("ERROR IN BYTE STUFFING");
        exit(-1);
    }

    for (int i=0; i<bufSize; i++)
    {
        if ((buf[i] == FLAG))
        {
            newbuf[i+counter] = ESC;
            newbuf[i+counter+1] = (FLAG^STUFF);
            counter++;
        }
        else if (buf[i] == ESC)
        {
            newbuf[i+counter] = ESC;
            newbuf[i+counter+1] = (ESC^STUFF);
            counter++;
        }
        else newbuf[i+counter] = buf[i];
    }
    
    return 0;
}

int byte_destuffing(unsigned char *newBuf, int bufSize, unsigned char *buf)
{
    for (int i=0, newpos = 0; i<bufSize; i++, newpos++)
    {
        if (newBuf[i] == ESC)
        {
            if (newBuf[i+1] == (FLAG^STUFF)) 
                buf[newpos] = FLAG;
            else if (newBuf[i+1] == (ESC^STUFF))
                buf[newpos] = ESC;
        }
        else buf[newpos] = newBuf[i];
    }
    return 0;
}

unsigned char createBCC2(unsigned char *data, int lenght)
{
    unsigned char buf = 0x00;
    for (int i=0; i<lenght; i++)
        buf ^= data[i];

    return buf;
}