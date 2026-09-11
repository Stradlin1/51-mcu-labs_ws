#include <reg52.h>

void delay()
{
    unsigned int i, j;

    for(i = 0; i < 500; i++)
    {
        for(j = 0; j < 120; j++);
    }
}

void System_Init()
{
    P2 = (P2 & 0x1F) | 0xE0;
    P0 = 0xFF;

    P2 = (P2 & 0x1F) | 0xC0;
    P0 = 0x00;

    P2 = (P2 & 0x1F) | 0x80;
    P0 = 0xFF;

    P2 = (P2 & 0x1F) | 0xA0;
    P0 = 0x00;
}

void LED_Write(unsigned char dat)
{
    P2 = (P2 & 0x1F) | 0x80;
    P0 = dat;

    P2 = (P2 & 0x1F) | 0xA0;
    P0 = 0x00;
}

void main()
{
    unsigned char i;

    System_Init();

    for(i = 0; i < 3; i++)
    {
        LED_Write(0x00);
        delay();

        LED_Write(0xFF);
        delay();
    }

    LED_Write(0xFF);

    while(1);
}