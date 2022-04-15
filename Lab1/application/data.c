#include "linklayer.h"
#include "aux.h"

unsigned char * createInfoPkg(unsigned char * data, int sizeData, int* finalSize)
{
    int extraSize = 0;
    unsigned char BCC2;
    unsigned char * pkg;

    for(int i = 0; i < sizeData; i++)
    {
        if(data[i] == FLAG || data[i] == ESC)
            extraSize++;
    }

    *finalSize = 6 + sizeData + extraSize;
    pkg = calloc(sizeof(unsigned char), *finalSize);
    
    pkg[0] = FLAG;
    pkg[1] = A;
    pkg[2] = C_I(s);
    pkg[3] = (A ^ pkg[2]);
    BCC2 = createBCC2(data, sizeData);
    pkg[*finalSize - 2] = BCC2;
    pkg[*finalSize - 1] = FLAG; 

    printf("Final Size: %d | sizeData: %d\n\n", *finalSize, sizeData);

    byte_stuffing(data, sizeData, (pkg + 4));

    return pkg;
}

int byte_stuffing(unsigned char *buf, int bufSize, unsigned char *stuffedBuf)
{
    if (!buf || bufSize < 0 || !stuffedBuf)
    {
        puts("Bad parameters in byte_stuffing()");
        exit(-1);
    }
    
    int i, j;
    for (i=0, j=0; i<bufSize; i++, j++)
    {
        if ((buf[j] == FLAG))
        {
            stuffedBuf[i] = ESC;
            stuffedBuf[i+1] = (FLAG^STUFF);
            i++;
        }
        else if (buf[j] == ESC)
        {
            stuffedBuf[i] = ESC;
            stuffedBuf[i+1] = (ESC^STUFF);
            i++;
        }
        else stuffedBuf[i] = buf[j];
    }
    
    return 0;
}

int byte_destuffing(unsigned char *stuffedBuf, int bufSize, unsigned char *buf)
{
    int i, j;
    for (i=0, j=0; i<bufSize; i++, j++)
    {
        if (stuffedBuf[i] == ESC)
        {
            if (stuffedBuf[i+1] == (FLAG^STUFF)){
                buf[j] = FLAG;
                i++;
            }
                
            else if (stuffedBuf[i+1] == (ESC^STUFF)){
                buf[j] = ESC;
                i++;
            }

            else
            {
                puts("Error destuffing"); 
                exit(-1);
            }
        }
        else buf[j] = stuffedBuf[i];
    }
    return j;
}

unsigned char createBCC2(unsigned char *data, int lenght)
{
    unsigned char buf = 0x00;
    for (int i=0; i<lenght; i++)
        buf ^= data[i];

    return buf;
}
