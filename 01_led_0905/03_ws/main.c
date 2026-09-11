#include <reg52.h>

void Delay(unsigned int t)
{
    while(t--);
    while(t--);
}

void Delay500ms()
{
    Delay(60000);
    Delay(60000);
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

void Actuator_Write(unsigned char dat)
{
    P2 = (P2 & 0x1F) | 0xA0;
    P0 = dat;
}

void main()
{
    unsigned char i;

    System_Init();

    for(i = 0; i < 3; i++)
    {
        LED_Write(0x00);
        Delay500ms();

        LED_Write(0xFF);
        Delay500ms();

        Actuator_Write(0x40);
        Delay500ms();

        Actuator_Write(0x00);
    }

    LED_Write(0xFF);
    Actuator_Write(0x00);

    while(1);
}