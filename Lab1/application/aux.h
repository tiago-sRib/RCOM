#ifndef AUX
#define AUX

#define FLAG 0x7E
#define A 0x03
#define C 0x03
#define BCC (A^C)

#define START_STATE 0
#define FLAG_STATE  1
#define A_STATE     2
#define C_STATE     3
#define BCC_STATE   4
#define STOP_STATE  5

// returns the current state
int StateMachine(unsigned char tx, int state);
int get_baud(int baud);
void printFLAGS(unsigned char x);
void createPkg(char *mode, unsigned char * pkg);
void atende();
int llopenfd (linkLayer connectionParameters);
#endif