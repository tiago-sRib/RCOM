 /*Non-Canonical Input Processing*/

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define BAUDRATE B38400
#define _POSIX_SOURCE 1  //POSIX compliant source
#define FALSE 0
#define TRUE 1

volatile int STOP=FALSE;

int main(int argc, char** argv)
{
    int fd,c, res;
    struct termios oldtio,newtio;
    char buf[255];

    if ( (argc < 2) || 
  	     ((strcmp("/dev/ttyS10", argv[1])!=0) && 
  	      (strcmp("/dev/ttyS11", argv[1])!=0) )) {
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

    newtio.c_cc[VTIME]    = 0;   /* inter-character timer unused */
    newtio.c_cc[VMIN]     = 1;   /* blocking read until 5 chars received */



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


/**********************CODIGO DA PRIMEIRA AULA****************************
    int i=0;
    while (STOP==FALSE) {         //loop for input 
      res = read(fd,&buf[i],1);   // returns after 5 chars have been input 
      if (buf[i]=='\0') STOP=TRUE;
      i++;
    }

    printf("Mensagem enviada: %s \n", buf);
    
    write(fd,buf,i);
    printf("Mensagem a enviar: %s\n", buf);
***************************************************************************/


while (STOP==FALSE) {
  res = read(fd,&buf[i],1);   
  
  switch(state)
  {
    case 0: 
          if(buf[i] == FLAG)          {bufUA[state]=buf[i];   state = 1;}
    
    case 1:      
          if(buf[i] == A)             {bufUA[state]=buf[i];   state = 2;}
          else if(buf[i] == FLAG)     {bufUA[0]=buf[i];       state = 1;}
          else state = 0;

    case 2:
          if(buf[i] == C)             {bufUA[state]=buf[i];   state = 3;}
          else if(buf[i] == FLAG)     {bufUA[0]=buf[i];       state = 1;}
          else state = 0;       
  
    case 3:
          if(buf[i] == BCC)           {bufUA[state]=buf[i];   state = 4;}
          else if(buf[i] == FLAG)     {bufUA[0]=buf[i];       state = 1;}
          else state = 0;

    case 4:
          if(buf[i] == FLAG)          {bufUA[state]=buf[i];   state = 5;}
          else state = 0;
    
    case 5:
          STOP = TRUE;
          printf("%d, success\n", i);
  }

  i++;
}



    tcsetattr(fd,TCSANOW,&oldtio);
    close(fd);
    sleep(1);
    return 0;
}
