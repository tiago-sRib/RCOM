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
    
    int i, counter = 0, newpos;
    for (i=0; i<bufSize; i++)
    {
        newpos = i+counter;
        if ((buf[i] == FLAG))
        {
            stuffedBuf[newpos] = ESC;
            stuffedBuf[newpos+1] = (FLAG^STUFF);
            counter++;
            
        }
        else if (buf[i] == ESC)
        {
            stuffedBuf[newpos] = ESC;
            stuffedBuf[newpos+1] = (ESC^STUFF);
            counter++;
        }
        else stuffedBuf[newpos] = buf[i];
    }

    return newpos;
}

int byte_destuffing(unsigned char *stuffedBuf, int StuffSize, unsigned char *buf)
{
    int i, newpos;
    for (i=0; i<StuffSize; i++)
    {
        if (stuffedBuf[i] == ESC)
        {
            if (stuffedBuf[i+1] == (FLAG^STUFF)){
                buf[newpos] = FLAG;
                i++;
            }
                
            else if (stuffedBuf[i+1] == (ESC^STUFF)){
                buf[newpos] = ESC;
                i++;  
            }
        }
        else buf[newpos] = stuffedBuf[i];
        newpos++;
    }
    
    return newpos;
}

unsigned char createBCC2(unsigned char *data, int lenght)
{
    unsigned char buf = 0x00;
    for (int i=0; i<lenght; i++)
        buf ^= data[i];

    return buf;
}
