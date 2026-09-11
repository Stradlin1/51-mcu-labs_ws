#include <reg52.h>

unsigned char code table[] = {
    0xc0, 0xf9, 0xa4, 0xb0,
    0x99, 0x92, 0x82, 0xf8,
    0x80, 0x90, 0x88, 0x83,
    0xc6, 0xa1, 0x86, 0x8e
};

unsigned char code bit_table[] = {
    0x01, 0x02, 0x04, 0x08,
    0x10, 0x20, 0x40, 0x80
};

void Delay(unsigned int t)
{
    while(t--);
}

void InitBoard()
{
    P2 = (P2 & 0x1F) | 0xE0;
    P0 = 0xFF;

    P2 = (P2 & 0x1F) | 0xC0;
    P0 = 0x00;

    P2 = (P2 & 0x1F) | 0xA0;
    P0 = 0x00;

    P2 = (P2 & 0x1F) | 0xC0;
    P0 = 0xFF;

    P2 = (P2 & 0x1F) | 0x80;
    P0 = 0xFF;

    P2 = (P2 & 0x1F) | 0xC0;
    P0 = 0x00;
}

void Display(unsigned char position, unsigned char num)
{
    P0 = 0xFF;

    P2 = (P2 & 0x1F) | 0xC0;
    P0 = position;

    P2 = (P2 & 0x1F) | 0xE0;
    P0 = table[num];
}

void main()
{
    unsigned char i;
    unsigned char num;

    InitBoard();

    while(1)
    {
        for(i = 0; i < 8; i++)
        {
            for(num = 0; num < 16; num++)
            {
                Display(bit_table[i], num);
                Delay(60000);
            }
        }

        for(num = 0; num < 16; num++)
        {
            Display(0xFF, num);
            Delay(60000);
        }
    }
}