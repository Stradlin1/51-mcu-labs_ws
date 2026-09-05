#include <reg52.h>

sbit HC138_A = P2^5;
sbit HC138_B = P2^6;
sbit HC138_C = P2^7;

void delay()
{
    unsigned int i, j;

    for(i = 0; i < 500; i++)
    {
        for(j = 0; j < 120; j++);
    }
}

void LED_Write(unsigned char dat)
{
    HC138_C = 1;
    HC138_B = 0;
    HC138_A = 0;

    P0 = dat;

    HC138_C = 1;
    HC138_B = 0;
    HC138_A = 1;

    P0 = 0x00;
}

void main()
{
    unsigned char i;

    P0 = 0x00;

    HC138_C = 1;
    HC138_B = 0;
    HC138_A = 1;

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