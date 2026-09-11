#include <reg52.h>

unsigned char led_data;
unsigned char num;

void InitBoard()
{
    P0 = 0x00;

    P2 = (P2 & 0x1F) | 0xA0;
    P0 = 0x00;

    P2 = (P2 & 0x1F) | 0xE0;
    P0 = 0xFF;

    P2 = (P2 & 0x1F) | 0xC0;
    P0 = 0x00;

    led_data = 0xFF;

    P2 = (P2 & 0x1F) | 0x80;
    P0 = led_data;
}

void Timer0Init()
{
    TMOD = 0x01;

    TH0 = (65536 - 50000) / 256;
    TL0 = (65536 - 50000) % 256;

    EA = 1;
    ET0 = 1;
    TR0 = 1;
}

void main()
{
    InitBoard();
    Timer0Init();

    while(1);
}

void Timer0() interrupt 1
{
    TH0 = (65536 - 50000) / 256;
    TL0 = (65536 - 50000) % 256;

    num++;

    if(num >= 10)
    {
        num = 0;

        led_data ^= 0x01;

        P2 = (P2 & 0x1F) | 0x80;
        P0 = led_data;
    }
}