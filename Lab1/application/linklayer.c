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
    unsigned char * buf;
    int state = START_STATE;

    ssize_t res;
    llcopy(connectionParameters);
    connectionConfig(connectionParameters);

    switch (cP.role)
    {
    case TRANSMITTER:
        buf = createPkg(SET_pkg);
    
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
            free(buf);
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

        buf = createPkg(UA_pkg);
        write(fd, buf, 5);
        break;

    default:
        exit(-1);
        break;
    }

    free(buf);
    sleep(1);
    return 1;
}

int llwrite(char* buf, int bufSize)
{   

    if(buf == NULL)  exit(-1);
    
    unsigned char * pkg = NULL;
    unsigned char b;
    int size, state = START_STATE;
    ssize_t res;

    s = r;
    pkg = createInfoPkg((unsigned char *)buf, bufSize, &size);
    
    printInfoPkg(size, pkg, -1);

    write(fd, pkg, size);

    while(state != STOP_STATE){
        
        res = read(fd, &b, 1);
        
        printf("RR [%d]st: ", state);
        printFlags(b);
        printf("-> ");
        state = StateMachineRR_REJ(b, state);
        printf("[%d]st\n", state);

    }   

    free(pkg); 
    return 0;
}

int llread(char* packet)
{  
    unsigned char * buf;
    int state = START_STATE;
    int i = 0, pkgSize, BCC2, BCC2_original;
    unsigned char pkgRecieved[MAX_PAYLOAD_SIZE * 2];

    while(state != STOP_STATE)
    {
        read(fd, &pkgRecieved[i], 1);
        printf("I [%d]st: ", state);
        printFlags(pkgRecieved[i]);
        printf("-> ");
        
        state = StateMachineI(pkgRecieved[i], state);
        printf("[%d]st\n", state);

        if(state == DATA_STATE)
            i++;
    }

    pkgSize = i - 1;
    BCC2 = pkgRecieved[pkgSize];
    
    stats.RecivedI++;

    pkgSize = byte_destuffing(pkgRecieved, i, (unsigned char *) packet);
    BCC2_original = createBCC2((unsigned char *) packet, pkgSize);
    printf("BCC2 recebido: %u | BCC2 reconstruido: %u\n", BCC2, BCC2_original);
    /*if(BCC2 != createBCC2((unsigned char *) packet, pkgSize))
     */

    r = 1 - s;
    buf = createPkg(RR_pkg);
    write(fd, buf, 5);
    free(buf);
    return 0;
}

int llclose(int showStatistics)
{   
    unsigned char * buf;
    unsigned char b;
    int state = START_STATE;
    ssize_t res;

    switch (cP.role)
    {
    case TRANSMITTER:
        buf = createPkg(DISC_pkg);

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
            free(buf);
        }

        buf = createPkg(UA2_pkg);
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

        buf = createPkg(DISC_pkg);
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
    free(buf);
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
