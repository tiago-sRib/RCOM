#include "linklayer.h"
#include "aux.h"

int StateMachineSET(unsigned char tx, int state)
{   
    switch(state)
    {
        case START_STATE: 
            if(tx == FLAG)              {state = FLAG_STATE;}
            break;

        case FLAG_STATE:      
            if(tx == A)                 {state = A_STATE;}
            else if(tx == FLAG)         {state = FLAG_STATE;}
            else state = START_STATE;
            break;

        case A_STATE:
            if(tx == C_SET)             {state = C_STATE;}
            else if(tx == FLAG)         {state = FLAG_STATE;}
            else                        {state = START_STATE;}
            break;

        case C_STATE:
            if(tx == BCC_SET)           {state = BCC_STATE;}
            else if(tx == FLAG)         {state = FLAG_STATE;}
            else state = START_STATE;
            break;

        case BCC_STATE:
            if(tx == FLAG)              {state = STOP_STATE;}
            else                        {state = START_STATE;}
            break;

        case STOP_STATE:
            break; 
    }

    printf("Going to [%d]st\n", state);
    return state;
}

int StateMachineUA(unsigned char tx, int state)
{   
    switch(state)
    {
        case START_STATE: 
            if(tx == FLAG)              {state = FLAG_STATE;}
            break;

        case FLAG_STATE:      
            if(tx == A)                 {state = A_STATE;}
            else if(tx == FLAG)         {state = FLAG_STATE;}
            else                        {state = START_STATE;}
            break;

        case A_STATE:
            if(tx == C_UA)              {state = C_STATE;}
            else if(tx == FLAG)         {state = FLAG_STATE;}
            else                        {state = START_STATE;}
            break;

        case C_STATE:
            if(tx == BCC_UA)            {state = BCC_STATE;}
            else if(tx == FLAG)         {state = FLAG_STATE;}
            else state = START_STATE;
            break;

        case BCC_STATE:
            if(tx == FLAG)              {state = STOP_STATE;}
            else                        {state = FLAG_STATE;}
            break;

        case STOP_STATE:
            break; 
    }

    printf("Going to [%d]st\n", state);
    return state;
}


int StateMachineUA2(unsigned char tx, int state)
{   
    switch(state)
    {
        case START_STATE: 
            if(tx == FLAG)              {state = FLAG_STATE;}
            break;

        case FLAG_STATE:      
            if(tx == A_2)               {state = A_STATE;}
            else if(tx == FLAG)         {state = FLAG_STATE;}
            else                        {state = START_STATE;}
            break;

        case A_STATE:
            if(tx == C_UA)              {state = C_STATE;}
            else if(tx == FLAG)         {state = FLAG_STATE;}
            else                        {state = START_STATE;}
            break;

        case C_STATE:
            if(tx == (A_2^C_UA))          {state = BCC_STATE;}
            else if(tx == FLAG)         {state = FLAG_STATE;}
            else state = START_STATE;
            break;

        case BCC_STATE:
            if(tx == FLAG)              {state = STOP_STATE;}
            else                        {state = FLAG_STATE;}
            break;

        case STOP_STATE:
            break; 
    }

    printf("Going to [%d]st\n", state);
    return state;
}


int StateMachineDISC(unsigned char tx, int state)
{   
    switch(state)
    {
        case START_STATE: 
            if(tx == FLAG)              {state = FLAG_STATE;}
            break;

        case FLAG_STATE:      
            if(tx == A)                 {state = A_STATE;}
            else if(tx == FLAG)         {state = FLAG_STATE;}
            else state = START_STATE;
            break;

        case A_STATE:
            if(tx == C_DISC)            {state = C_STATE;}
            else if(tx == FLAG)         {state = FLAG_STATE;}
            else                        {state = START_STATE;}       
            break;

        case C_STATE:
            if(tx == BCC_DISC)          {state = BCC_STATE;}
            else if(tx == FLAG)         {state = FLAG_STATE;}
            else state = START_STATE;
            break;

        case BCC_STATE:
            if(tx == FLAG)              {state = STOP_STATE;}
            else                        {state = FLAG_STATE;}
            break;

        case STOP_STATE:
            break; 
    }

    printf("Going to [%d]st\n", state);
    return state;
}

void printFlags(unsigned char x){
    switch (x)
    {
    case FLAG:
        printf("FLAG");
        break;
    case BCC_DISC:
        printf("BCC_DISC");
        break;
    case BCC_SET:
        printf("BCC_SET");
        break;
    case BCC_UA:
        printf("BCC_UA");
        break;
    case A:
        printf("A\\C");
        break; 
    case C_DISC:
        printf("C_DISC");
        break;
    case C_UA:
        printf("C_UA");
        break;
    
    default:
        printf("%u*", x);
        break;
    }
}

int getBaud(int baud)
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

void createPkg(unsigned int type, unsigned char * pkg)
{
    switch (type)
    {
    case SET_pkg:
        pkg[0] = FLAG;  
        pkg[1] = A;
        pkg[2] = C_SET;
        pkg[3] = BCC_SET;
        pkg[4] = FLAG; 
        break;

    case UA_pkg:
        pkg[0] = FLAG;  
        pkg[1] = A;
        pkg[2] = C_UA;
        pkg[3] = BCC_UA;
        pkg[4] = FLAG; 
        break;

    case UA2_pkg:
        pkg[0] = FLAG;  
        pkg[1] = A_2;
        pkg[2] = C_UA;
        pkg[3] = A_2^C_UA;
        pkg[4] = FLAG; 
        break;

    case DISC_pkg:
        pkg[0] = FLAG;  
        pkg[1] = A;
        pkg[2] = C_DISC;
        pkg[3] = BCC_DISC;
        pkg[4] = FLAG; 
        break;
    case 2000:
        pkg[0] = 'T';  
        pkg[1] = 'E';
        pkg[2] = 'S';
        pkg[3] = 'T';
        pkg[4] = 'E'; 
        pkg[5] = '\0';
        break;
    }
}
