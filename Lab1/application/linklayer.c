#include "linklayer.h"
#include "aux.h"

int attempts=1, timeOutFLAG=1;
int fd;
struct termios oldtio,newtio;
linkLayer cP;
int parity_bit = 0;

varStatistics stats;

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
    ssize_t res = 0, res_old = 0;

    llcopy(connectionParameters);
    connectionConfig(connectionParameters);

    switch (cP.role)
    {
    case TRANSMITTER:
        createPkg(SET_pkg, buf);
    
        (void) signal(SIGALRM, timeOut);
        stats.numTimeouts--;

        while(attempts <= cP.numTries && state != STOP_STATE){
            
            res += read(fd, &b, 1);
            
            if(res == res_old && timeOutFLAG)
            {
                write(fd, buf, 5);
                alarm(cP.timeOut);
                timeOutFLAG = 0;

                stats.numTimeouts++;
            }
            
            if(res > res_old)
            {
                alarm(0);
                state = StateMachineUA(b, state);
                timeOutFLAG = 1;                
            }

            res_old = res;
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

    if(buf == NULL || bufSize > MAX_PAYLOAD_SIZE) 
        exit(-1);

    unsigned char * pkg = NULL;
    unsigned char b;
    int size, state = START_STATE;
    ssize_t res = 0, res_old = 0;
   
    pkg = createInfoPkg((unsigned char *)buf, bufSize, &size);

    //debugging
    /*
    if(pkg[2] == C_I(0))
        printf("Ns(0)\n");

    else if(pkg[2] == C_I(1))
        printf("Ns(1)\n");

    else printf("%x\n", pkg[2]);
    */

    stats.numTimeouts--;
    (void) signal(SIGALRM, timeOut);

    attempts = timeOutFLAG = 1;
    
    while(attempts <= cP.numTries && state != STOP_STATE)
    {    
        res += read(fd, &b, 1);
        
        if(state == STOP_REJ_STATE)
        {   
            alarm(0);
            write(fd, pkg, size);
            state = START_STATE;

            stats.numIframes++;
            stats.numREJ++;

            puts("Rej was triggered. A new package was sent");
        }

        else if(res > res_old)
        {
            alarm(0);
            state = StateMachineRR_REJ(b, state);
            timeOutFLAG = 1;
        }
        
        else if(res == res_old && timeOutFLAG)
        {
            write(fd, pkg, size);
            alarm(cP.timeOut);
            timeOutFLAG = 0;

            stats.numTimeouts++;
            stats.numIframes++;
        }
        
        res_old = res;
    }   
    
   
    if(attempts > cP.numTries)
    {
        puts("Number of tries exceded");
        free(pkg);
        exit(-1);
    }

    parity_bit = 1 - parity_bit;
    free(pkg); 
    return 0;
}

int llread(char* packet)
{  
    unsigned char buf[5], b, pkgRecieved[MAX_PAYLOAD_SIZE * 2];
    int state = START_STATE;
    int i = 0, pkgSize, deStuffSize, BCC2, BCC2_r;
    ssize_t res = 0, res_old = 0;
    
    while(state != STOP_STATE && i < MAX_PAYLOAD_SIZE * 2)
    {
        res += read(fd, &b, 1);
        
        if(res > res_old)
        {
            state = StateMachineI(b, state);

            if(state == DATA_STATE)
            {
                pkgRecieved[i] = b;
                i++;
            }
        }

        res_old = res;
    }
    
    BCC2 = pkgRecieved[i-1];
    pkgSize = i - 1;
    
    deStuffSize = byte_destuffing(pkgRecieved, pkgSize, (unsigned char *) packet);
    BCC2_r = createBCC2((unsigned char *) packet, deStuffSize);
    
    stats.numBytesFile += deStuffSize;

    if(BCC2 != BCC2_r)
    {
        createPkg(REJ_pkg, buf);
        write(fd, buf, 5);
        
        puts("Package rejected");
        return 0;
    }

    else
    {   
        parity_bit = 1 - parity_bit;
        createPkg(RR_pkg, buf);
        write(fd, buf, 5);

        //debugging
        if(buf[2] == C_RR(0))
            printf("Nr(0)\n");

        else if(buf[2] == C_RR(1))
            printf("Nr(1)\n");

        else printf("Completly wrong\n");
        
    }
    
    return deStuffSize;
}

int llclose(int showStatistics)
{   
    unsigned char buf[5], b;
    int state = START_STATE;
    ssize_t res = 0, res_old = 0;

    switch (cP.role)
    {
    case TRANSMITTER:
        createPkg(DISC_pkg, buf);

        (void) signal(SIGALRM, timeOut);
        attempts = timeOutFLAG = 1;
        stats.numTimeouts--;

        while(attempts <= cP.numTries && state != STOP_STATE){
            
            res += read(fd, &b, 1);
            
            if(res == res_old && timeOutFLAG)
            {
                write(fd, buf, 5);
                alarm(cP.timeOut);
                timeOutFLAG = 0;

                stats.numTimeouts++;
            }

            if(res > res_old)
            {
                alarm(0);
                state = StateMachineDISC(b, state);
                timeOutFLAG = 1;
            }

            res_old = res;
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
            res += read(fd, &b, 1);
            
            if(res > res_old)
                state = StateMachineDISC(b, state);

            res_old = res;
        }

        createPkg(DISC_pkg, buf);
        write(fd, buf, 5);

        state = START_STATE;
        while(state != STOP_STATE)
        {
            res += read(fd, &b, 1);
            
            if(res > res_old)
                state = StateMachineUA2(b, state);
        
            res_old = res;
        }
        puts("Everything went smooth");
    break;

    default:
        exit(-1);
        break;
    }

    close(fd);

    if(showStatistics)
        printstatistics ();

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

    newtio.c_cc[VTIME]    = connectionParameters.timeOut * 10;   /* inter-character timer unused */
    newtio.c_cc[VMIN]     = 0;   /* blocking read until 1 chars received */

    tcflush(fd, TCIOFLUSH);

    if ( tcsetattr(fd,TCSANOW,&newtio) == -1) {
      perror("tcsetattr");
      exit(-1);
    }
}

void printstatistics ()
{
    printf("\nStatistics: \n");

    printf("Number of Bytes of the file stuffed: %d B\n", stats.numBytesStuff);
    printf("Number of Bytes of the file recieved: %d B\n", stats.numBytesFile);
    printf("Number of REJ frames: %d\n", stats.numREJ);
    printf("Number of I frames sent: %d\n", stats.numIframes);
    printf("Number of Timeouts: %d\n", stats.numTimeouts);
    stats.numRetransmissions = stats.numREJ + stats.numTimeouts;
    printf("Number of Retransmissions: %d\n", stats.numRetransmissions);

}