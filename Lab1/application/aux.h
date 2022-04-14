#ifndef AUX
#define AUX

#define SET_pkg 1
#define UA_pkg 2
#define UA2_pkg 4
#define DISC_pkg 3
#define RR_pkg 5

#define FLAG 0x7E
#define ESC 0x7d
#define STUFF 0x20
#define A 0x03
#define A_2 0x01

#define C_SET 0x03
#define C_UA 0x07
#define C_DISC 0x0b

#define BCC_SET (A^C_SET)
#define BCC_UA (A^C_UA)
#define BCC_DISC (A^C_DISC)

#define START_STATE 0
#define FLAG_STATE  1
#define A_STATE     2
#define C_STATE     3
#define BCC_STATE   4
#define STOP_STATE  5

typedef struct stats{
    int role; //defines the role of the program: 0==Transmitter, 1=Receiver
    int timeOuts;
    int RecivedI;
    int RetransmitedFrames;
} stats;

// returns the current state
int StateMachineUA(unsigned char tx, int state);
int StateMachineDISC(unsigned char tx, int state);
int StateMachineSET(unsigned char tx, int state);
int StateMachineUA2(unsigned char tx, int state);
int StateMachineRR(unsigned char tx, int state, unsigned char *Ns);
int StateMachineI(unsigned char tx, int state, unsigned char *Nr);

int getBaud(int baud);
void printFlags(unsigned char x);
void createPkg(unsigned int type, unsigned char * pkg);
void timeOut();
void connectionConfig(linkLayer connectionParameters);
void llcopy(linkLayer connectionParameters);
int createInfoPkg(unsigned char * data, int sizeData, unsigned char Ns, unsigned char * pkg);
int byte_stuffing(unsigned char *buf, int bufSize, unsigned char *newbuf);
int byte_destuffing(unsigned char *newBuf, int bufSize, unsigned char *buf);
unsigned char createBCC2(unsigned char *data, int lenght);
#endif