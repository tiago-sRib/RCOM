#include "linklayer.h"
#include "aux.h"

unsigned char * createInfoPkg(unsigned char * data, int sizeData, int* finalSize)
{
    if (!data || sizeData > MAX_PAYLOAD_SIZE || !finalSize)
    {
        puts("Couldnt createInfoPkg");
        exit(-1);
    }

    int extraSize = 0;
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
    pkg[2] = C_I(parity_bit);
    pkg[3] = (A ^ pkg[2]);
    pkg[*finalSize - 2] = createBCC2(data, sizeData);
    pkg[*finalSize - 1] = FLAG; 

    byte_stuffing(data, sizeData, (pkg + 4));
    
    stats.numBytesStuff += extraSize;

    // testar o rej    
    /*
    unsigned char val;
    int randPos[10];
    if(cc > 0)
    {
        for(int i = 0; i < 10; i++)
        {
            randPos[i] = rand() % sizeData;
            if(randPos[i] < 4) randPos[i] += 4;
            val = rand();
            pkg[randPos[i]] = val;
            printf("%d: Pkg[%d] = %u\n", i, randPos[i], val);
        }
        cc--;
    }  
    */

    return pkg;
}

int byte_stuffing(unsigned char *buf, int bufSize, unsigned char *stuffedBuf)
{
    if (!buf || bufSize < 0 || !stuffedBuf || bufSize > MAX_PAYLOAD_SIZE)
    {
        puts("Bad parameters in byte_stuffing()");
        exit(-1);
    }
    
    int i, j = 0;

    for (i=0; i< bufSize; i++)
    {
        if ((buf[i] == FLAG))
        {
            stuffedBuf[j] = ESC;
            stuffedBuf[j+1] = (FLAG^STUFF);
            j += 2; 
        }
        else if (buf[i] == ESC)
        {
            stuffedBuf[j] = ESC;
            stuffedBuf[j+1] = (ESC^STUFF);
            j += 2;
        }
        else 
        {
            stuffedBuf[j] = buf[i];
            j++;
        }
        
    }

    return j;
}

int byte_destuffing(unsigned char *stuffedBuf, int StuffSize, unsigned char *buf)
{
    if (!stuffedBuf || !buf || StuffSize > 2*MAX_PAYLOAD_SIZE)
    {
        puts("Bad parameters destuffing");
        exit(-1);
    }
    int i, newpos;
    for (i=0, newpos = 0; i < StuffSize; i++, newpos++)
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
    }
    return newpos;
}

unsigned char createBCC2(unsigned char *data, int lenght)
{
    if (!data || lenght < 0)
    {
        puts("Couldnt create BCC");
        exit(-1);
    }
    
    unsigned char buf = 0x00;
    for (int i=0; i<lenght; i++)
        buf ^= data[i];

    return buf;
}
