#ifndef AUX
#define AUX

#include <time.h>

#define FLAG 0x7E
#define ESC 0x7d
#define STUFF 0x20
#define A 0x03
#define A_2 0x01

#define C_SET       0x03
#define C_UA        0x07
#define C_DISC      0x0b

#define C_I(x)      (0x00 | (x << 1))
#define C_RR(x)     (0x01 | (x << 5))
#define C_REJ(x)    (0x05 | (x << 5))

#define BCC_SET     (A^C_SET)
#define BCC_UA      (A^C_UA)
#define BCC_DISC    (A^C_DISC)

#define START_STATE 0
#define FLAG_STATE  1
#define A_STATE     2
#define C_STATE     3
#define BCC_STATE   4
#define STOP_STATE  5
#define DATA_STATE  6
#define C_REJ_STATE 7 
#define BCC_REJ_STATE 8
#define STOP_REJ_STATE 9

/* These defines are meant to be used with the createPkg() function */
#define SET_pkg 1
#define UA_pkg 2
#define UA2_pkg 4
#define DISC_pkg 3
#define RR_pkg 5
#define REJ_pkg 6

/* Bits de paridade */
extern int parity_bit;

typedef struct stats
{
    int numBytesStuff;
    int numBytesFile;
    int numREJ;
    int numTimeouts;
    int numIframes;
    int numRetransmissions;
} varStatistics;

extern varStatistics stats;

/* Cada State Machine permite detetar os diferentes tipos de tramas
 * 
 * Todas as maquinas de estado dao return do novo estado correspondente
 * ao byte tx fornecido
 */
int StateMachineUA(unsigned char tx, int state);
int StateMachineDISC(unsigned char tx, int state);
int StateMachineSET(unsigned char tx, int state);
int StateMachineUA2(unsigned char tx, int state);
int StateMachineRR_REJ(unsigned char tx, int state);
int StateMachineI(unsigned char tx, int state);

/* retorna em apontador o package criado
 * 
 * Nota: No caso da trama RR o valor do Control Field e' fornecido atraves das 
 * variaveis globais dos bits de paridade. Nesse caso e´ necessario estabelecer 
 * corretamente a paridade antes de a executar.
 */
void createPkg(unsigned int type, unsigned char * pkg);

/* Cria a trama de Informacao
 *
 * Recebe os dados antes do Stuffing e o seu respetivo tamanho
 * Aloca o tamannho estritamente necessario para a trama apos o Stuffing
 * Chama a funcao byte_stuffing
 * Chama a funcao createBCC2
 * Devolve na variavel pkg a trama I completa
 */
unsigned char * createInfoPkg(unsigned char * data, int sizeData, int * finalSize);
int byte_stuffing(unsigned char *buf, int bufSize, unsigned char *stuffedBuf);
unsigned char createBCC2(unsigned char *data, int lenght);

int byte_destuffing(unsigned char *StuffedBuf, int StuffSize, unsigned char *deStuffedBuf);

/* utility functions */
int getBaud(int baud);
void timeOut();
void connectionConfig(linkLayer connectionParameters);
void llcopy(linkLayer connectionParameters);
void printstatistics ();

/* For debugging only */
void printFlags(unsigned char x);
void printInfoPkg(int size, unsigned char *pkg, unsigned char BCC2);
#endif