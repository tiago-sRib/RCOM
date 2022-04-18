#include "linklayer.h"
#include "aux.h"

int attempts=1, timeOutFLAG=1;
static int fd;
struct termios oldtio,newtio;
linkLayer cP;

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
                printf("UA [%d]st: ", state);
                printFlags(b);
                printf("-> ");
                state = StateMachineUA(b, state); 
                printf("[%d]st\n", state);

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
            printf("SET [%d]st: ", state);
            printFlags(b);
            printf("-> ");
            state = StateMachineSET(b, state);
            printf("[%d]st\n", state);
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

    if(buf == NULL || bufSize > MAX_PAYLOAD_SIZE) 
        exit(-1);

    unsigned char * pkg = NULL;
    unsigned char b;
    int size, state = START_STATE;
    ssize_t res;
    //unsigned char Stuff[MAX_PAYLOAD_SIZE*2], deStuff[MAX_PAYLOAD_SIZE];
    //int numErros = 0, StuffSize = 0, deStuffSize = 0;
    s = r;
    
    pkg = createInfoPkg((unsigned char *)buf, bufSize, &size);
    write(fd, pkg, size);
    

    while(state != STOP_STATE){
        
        res = read(fd, &b, 1);
        
        //printf("RR [%d]st: ", state);
        //printFlags(b);
        //printf("-> ");
        state = StateMachineRR_REJ(b, state);
        //printf("[%d]st\n", state);

    }   
    
    free(pkg); 
    return 0;
}

int llread(char* packet)
{  
    unsigned char buf[5], b;
    int state = START_STATE;
    int i = 0, pkgSize, deStuffSize, BCC2, BCC2_reconstruido;
    unsigned char pkgRecieved[MAX_PAYLOAD_SIZE * 2];
    int res = 0, res_old = 0;
    
    while(state != STOP_STATE && i < MAX_PAYLOAD_SIZE * 2)
    {
        res += read(fd, &b, 1);
        //printf("I [%d]st: ", state);
        //printFlags(b);
        //printf("-> ");
        
        if(res > res_old)
        {
            state = StateMachineI(b, state);

            if(state == DATA_STATE)
            {
                pkgRecieved[i] = b;
                i++;
            }
        }
            
        
        //printf("[%d]st\n", state);

        res_old = res;
    }

    BCC2 = pkgRecieved[i-1];
    pkgSize = i - 1;
    
    //stats.RecivedI++;


    deStuffSize = byte_destuffing(pkgRecieved, pkgSize, (unsigned char *) packet);
    BCC2_reconstruido = createBCC2((unsigned char *) packet, deStuffSize);
    
    if(BCC2 != BCC2_reconstruido)
    {
        exit(-1);
    }
    

    r = 1 - s;
    createPkg(RR_pkg, buf);
    write(fd, buf, 5);
    return deStuffSize;
}

int llclose(int showStatistics)
{   
    unsigned char buf[5];
    unsigned char b;
    int state = START_STATE;
    ssize_t res;
    puts("entrou no close");
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
                printf("DISC [%d]st: ", state);
                printFlags(b);
                printf("-> ");
                state = StateMachineDISC(b, state);
                printf("[%d]st\n", state);
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
            printf("DISC [%d]st: ", state);
            printFlags(b);
            printf("-> ");
            state = StateMachineDISC(b, state);
            printf("[%d]st\n", state);
        }

        createPkg(DISC_pkg, buf);
        write(fd, buf, 5);

        state = START_STATE;
        puts("DISC pkgs exchenged");
        while(state != STOP_STATE)
        {
            read(fd, &b, 1);
            printf("UA2 [%d]st: ", state);
            printFlags(b);
            printf("-> ");
            state = StateMachineUA2(b, state);
            printf("[%d]st\n", state);
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

void llcopy(linkLayer connectionParameters)
{
    cP.role = connectionParameters.role;
    cP.timeOut = connectionParameters.timeOut;
    cP.numTries = connectionParameters.numTries;
}

void connectionConfig(linkLayer connectionParameters)
{
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

    newtio.c_cc[VTIME]    = 1;   /* inter-character timer unused */
    newtio.c_cc[VMIN]     = 0;   /* blocking read until 1 chars received */

    tcflush(fd, TCIOFLUSH);

    if ( tcsetattr(fd,TCSANOW,&newtio) == -1) {
      perror("tcsetattr");
      exit(-1);
    }
}
