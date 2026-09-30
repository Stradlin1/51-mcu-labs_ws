#include <reg52.h>

sfr AUXR = 0x8e;

unsigned char urdat = 0x00;
bit led_pending = 0;

void BusSafe()
{
    P2 &= 0x1F;
    P0 = 0x00;
}

void LED_Write(unsigned char dat)
{
    BusSafe();

    P2 = (P2 & 0x1F) | 0x80;
    P0 = dat;

    BusSafe();
}

void HardwareInit()
{
    BusSafe();

    P2 = (P2 & 0x1F) | 0xA0;
    P0 = 0x00;
    BusSafe();

    P2 = (P2 & 0x1F) | 0xC0;
    P0 = 0x00;
    BusSafe();

    P2 = (P2 & 0x1F) | 0xE0;
    P0 = 0xFF;
    BusSafe();

    LED_Write(0xFF);

    BusSafe();
}

void InitUart()
{
    TMOD = 0x20;
    TH1 = 0xFD;
    TL1 = 0xFD;
    TR1 = 1;

    SCON = 0x50;
    AUXR = 0x00;

    TI = 0;
    RI = 0;
}

void SendByte(unsigned char dat)
{
    SBUF = dat;
    while(TI == 0);
    TI = 0;
}

void SendString(unsigned char code *str)
{
    while(*str != '\0')
    {
        SendByte(*str);
        str++;
    }
}

void ServiceUart() interrupt 4
{
    if(RI == 1)
    {
        RI = 0;
        urdat = SBUF;
        led_pending = 1;
    }
}

void main()
{
    unsigned char dat;
    unsigned char code hello[] = "hello world";

    HardwareInit();
    InitUart();

    SendString(hello);

    RI = 0;
    ES = 1;
    EA = 1;

    while(1)
    {
        if(led_pending == 1)
        {
            ES = 0;
            dat = urdat;
            led_pending = 0;
            ES = 1;

            LED_Write(dat ^ 0xFF);
        }
    }
}