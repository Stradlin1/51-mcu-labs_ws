#include <reg52.h>

unsigned char code table[] = {
    0xc0, 0xf9, 0xa4, 0xb0,
    0x99, 0x92, 0x82, 0xf8,
    0x80, 0x90, 0x88, 0x83,
    0xc6, 0xa1, 0x86, 0x8e
};

void Delay(unsigned int t)
{
    while(t--);
}

void main()
{
    unsigned char num;

    P2 = (P2 & 0x1F) | 0xA0;
    P0 = 0x00;

    P2 = (P2 & 0x1F) | 0xC0;
    P0 = 0xFF;

    P2 = (P2 & 0x1F) | 0x80;
    P0 = 0xFF;

    P2 = (P2 & 0x1F) | 0xC0;
    P0 = 0xFF;

    P2 = (P2 & 0x1F) | 0xE0;
    P0 = table[0];

    while(1)
    {
        for(num = 0; num < 16; num++)
        {
            P0 = table[num];

            Delay(60000);
            Delay(60000);
        }
    }
}