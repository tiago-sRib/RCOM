/*Non-Canonical Input Processing*/

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "linklayer.h"

#define BAUDRATE B38400
#define MODEMDEVICE "/dev/ttyS1"
#define _POSIX_SOURCE 1 /* POSIX compliant source */
#define FALSE 0
#define TRUE 1

#define FLAG 0x7E
#define A 0x03
#define C 0x03
#define BCC A^C

volatile int STOP=FALSE;

int main(int argc, char** argv)
{
    int fd,c, res;
    struct termios oldtio,newtio;
    unsigned char buf[255];
    unsigned char UA[255];
    unsigned char SET[255];

    int i, sum = 0, speed = 0;
    
    if ( (argc < 2) || 
  	     ((strcmp("/dev/ttyS0", argv[1])!=0) && 
  	      (strcmp("/dev/ttyS1", argv[1])!=0) )) {
      printf("Usage:\tnserial SerialPort\n\tex: nserial /dev/ttyS1\n");
      exit(1);
    }


  /*
    Open serial port device for reading and writing and not as controlling tty
    because we don't want to get killed if linenoise sends CTRL-C.
  */


    fd = open(argv[1], O_RDWR | O_NOCTTY );
    if (fd <0) {perror(argv[1]); exit(-1); }

    if ( tcgetattr(fd,&oldtio) == -1) { /* save current port settings */
      perror("tcgetattr");
      exit(-1);
    }

    bzero(&newtio, sizeof(newtio));
    newtio.c_cflag = BAUDRATE | CS8 | CLOCAL | CREAD;
    newtio.c_iflag = IGNPAR;
    newtio.c_oflag = 0;

    /* set input mode (non-canonical, no echo,...) */
    newtio.c_lflag = 0;

    newtio.c_cc[VTIME]    = 1;   /* inter-character timer unused */
    newtio.c_cc[VMIN]     = 0;   /* blocking read until 5 chars received */



  /* 
    VTIME e VMIN devem ser alterados de forma a proteger com um temporizador a 
    leitura do(s) pr�ximo(s) caracter(es)
  */



    tcflush(fd, TCIOFLUSH);

    if ( tcsetattr(fd,TCSANOW,&newtio) == -1) {
      perror("tcsetattr");
      exit(-1);
    }

    printf("New termios structure set\n");




//**********************CODIGO DA PRIMEIRA AULA****************************

  char str[255];
  int len;

  fgets(str, sizeof(str), stdin);
  len = strlen(str);

    for (i = 0; i < len; i++) {
      buf[i] = str[i];
    }
    printf("len = %d\n", len);
    buf[len-1] = '\0';
    printf("Mensagem a enviar: %s\n", buf);
    
    res = write(fd,buf,len);   
    printf("%d bytes written\n", res);
 
    read(fd, buf, len);
     printf("Mensagem recebida: %s\n", buf);
//***************************************************************************




//**********************CODIGO DA SEGUNDA AULA****************************
SET[0]  =0x49;
SET[1]=FLAG;
SET[2]=A;
SET[3]=C;
SET[4]=FLAG;
SET[5]=C;
SET[6]=FLAG;
SET[7]=A;
SET[8]=C;

SET[9]=BCC;
SET[10]=FLAG;

res = write(fd, SET, 255);

int state=0;
i=0;


while (STOP==FALSE) {
  res = read(fd,&buf[i],1);   
  
  switch(state)
  {
    case 0: 
          if(buf[i] == FLAG)          {bufUA[state]=SET[i];   state = 1;}
          break;
    case 1:      
          if(buf[i] == A)             {bufUA[state]=SET[i];   state = 2;}
          else if(buf[i] == FLAG)     {bufUA[0]=SET[i];       state = 1;}
          else state = 0;
          break;
    case 2:
          if(buf[i] == C)             {bufUA[state]=SET[i];   state = 3;}
          else if(buf[i] == FLAG)     {bufUA[0]=SET[i];       state = 1;}
          else state = 0;       
          break;
    case 3:
          if(buf[i] == BCC)           {bufUA[state]=bufSET[i];   state = 4;}
          else if(buf[i] == FLAG)     {bufUA[0]=bufSET[i];       state = 1;}
          else state = 0;
          break;
    case 4:
          if(buf[i] == FLAG)          {bufUA[state]=bufSET[i];   state = 5;}
          else state = 0;
          break;
    case 5:
          STOP = TRUE;
          printf("%d, success\n", i);
          break; 
  }
  
  i++;
}


//***************************************************************************


    if ( tcsetattr(fd,TCSANOW,&oldtio) == -1) {
      perror("tcsetattr");
      exit(-1);
    }

    close(fd);
    return 0;
}
