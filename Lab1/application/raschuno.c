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

int llwrite(char* buf, int bufSize)
{   

    if(buf == NULL)  exit(-1);
    
    unsigned char * pkg = NULL;
    unsigned char b;
    int size, state = START_STATE;
    ssize_t res;

    s = r;
    size = createInfoPkg((unsigned char *)buf, bufSize, pkg);
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
            state = StateMachineRR_REJ(b, state); 
        }   
    }   

    free(pkg); 
    return 0;
}

int llread(char* packet)
{  
    unsigned char buf[5];
    int state = START_STATE;
    int i = 0, pkgSize, BCC2;
    unsigned char pkgRecieved[MAX_PAYLOAD_SIZE * 2];


    while(state != STOP_STATE)
        {
            read(fd, &pkgRecieved[i], 1);
            printf("[%d]st: ", state);
            printFlags(pkgRecieved[i]);
            printf("-> ");
            
            state = StateMachineI(pkgRecieved[i], state);

            if(state == DATA_STATE)
                i++;
        }

        pkgSize = i - 1;
        BCC2 = pkgRecieved[pkgSize];
        
        stats.RecivedI++;

        pkgSize = byte_destuffing(pkgRecieved, i, (unsigned char *) packet);
        if(BCC2 != createBCC2((unsigned char *) packet, pkgSize))
        {

            return -1;
        }

        r = (s == 0) ? 1 : 0;
        createPkg(RR_pkg, buf);
        write(fd, buf, 5);
    
    return 0;
}

int StateMachineRR_REJ(unsigned char tx, int state)
{
    switch (state)
    {
    case START_STATE:
        if (tx == FLAG)
            state = FLAG_STATE;
        
        break;

    case FLAG_STATE:
        if (tx == A)
            state = A_STATE;
        
        else if (tx == FLAG)
           state = FLAG_STATE;
        
        else
            state = START_STATE;
        break;

    case A_STATE:
        if (tx == C_RR(0) || tx == C_RR(1))
            state = C_STATE;

        else if (tx == FLAG)
            state = FLAG_STATE;
        
        else
            state = START_STATE;
        
        break;

    case C_STATE:
        if (tx == (A^C_RR(0)) || tx == (A^C_RR(1)) )
            state = BCC_STATE;
        
        else if (tx == FLAG)
            state = FLAG_STATE;
        
        else
            state = START_STATE;
        
        break;

    case BCC_STATE:
        if (tx == FLAG)
            state = STOP_STATE;
        
        else
            state = FLAG_STATE;
        
        break;

    case STOP_STATE:
        break;
    }

    return state;
}


int StateMachineI(unsigned char tx, int state)
{
    switch (state)
    {
    case START_STATE:
        if (tx == FLAG)
            state = FLAG_STATE;
        
        break;

    case FLAG_STATE:
        if (tx == A)
            state = A_STATE;
        
        else if (tx == FLAG)
            state = FLAG_STATE;
        
        else
            state = START_STATE;
        break;

    case A_STATE:
        if (tx == C_I(0) || tx == C_I(1))
            state = C_STATE;

        else if (tx == C_REJ(0) || tx == C_REJ(1))
            state = C_REJ_STATE;

        else if (tx == FLAG)
            state = FLAG_STATE;
        
        else
            state = START_STATE;
        
        break;

    case C_STATE:
        if (tx == (A ^ C_I(1)) || tx == (A ^ C_I(0)) )
            state = BCC_STATE;
        
        else if (tx == FLAG)
            state = FLAG_STATE;
        
        else
            state = START_STATE;
        break;

    case BCC_STATE:
        if (tx == FLAG)
            state = FLAG_STATE;
        
        else
            state = DATA_STATE;
        
        break;

    case DATA_STATE:
        if (tx == FLAG)
            state = STOP_STATE;
        
        break;
    case STOP_STATE:
        break;

    case C_REJ_STATE:
        if (tx == (A^C_REJ(0)) || tx == (A^C_REJ(1)) )
            state = BCC_REJ_STATE;
        
        else if (tx == FLAG)
            state = FLAG_STATE;
        
        else 
            state = START_STATE;
        
        break;

    case BCC_REJ_STATE:
        if (tx == FLAG)
            state = STOP_STATE;
        
        else 
            state = START_STATE;
        
        break;
    }

    return state;
}
