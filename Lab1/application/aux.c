#include "linklayer.h"
#include "aux.h"


int get_baud(int baud);
void printFLAGS(unsigned char x);
void createPkg(char *mode, unsigned char * pkg);
int StateMachine(unsigned char tx, int state);
void atende();

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
