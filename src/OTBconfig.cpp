#include "OTBconfig.h"
#include <iostream>

unsigned char crc(unsigned char config[])
{
	unsigned char crc =0;
	for(int j=0;j<CONFIG_SIZE-1;j++)
		{
			unsigned char val = config[j];
			for(int i=0;i<8;i++)
				{
					if(crc%2 ^ val%2)
						crc = crc/2 ^ CRC_CODE;
					else
						crc/=2;
					val /= 2;
				}
		}
	return crc;
}

void printBIN(char n)
{
	int i;
	unsigned char mask = 0b10000000;
	for(i=0;i<8;i++)
		{
			std::cout << ( ((n & mask)!=0)?1:0 );
			mask = mask >> 1;
		}
	std::cout << std::endl;
    
}

static void init(void)
{
#ifdef WIN32
    WSADATA wsa;
    int err = WSAStartup(MAKEWORD(2, 2), &wsa);
    if(err < 0)
    {
        puts("WSAStartup failed !");
        exit(EXIT_FAILURE);
    }
#endif
}

static void end(void)
{
#ifdef WIN32
    WSACleanup();
#endif
}
