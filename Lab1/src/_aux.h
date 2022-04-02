#ifndef AUX
#define AUX

#define FLAG 0x7E
#define A 0x03
#define C 0x03
#define BCC A^C

#define START_STATE 0
#define FLAG_STATE  1
#define A_STATE     2
#define C_STATE     3
#define BCC_STATE   4
#define STOP_STATE  5

typedef struct linkLayer{
    char serialPort[50];
    int role; //defines the role of the program: 0==Transmitter, 1=Receiver
    int baudRate;
    int numTries;
    int timeOut;
} linkLayer;

// returns the current state
int StateMachine(unsigned char tx, unsigned char rx, int state);

#endif