#include <reg52.h>

unsigned char code table[] = {
    0xc0, 0xf9, 0xa4, 0xb0,
    0x99, 0x92, 0x82, 0xf8,
    0x80, 0x90
};

volatile unsigned char count0 = 0;
volatile unsigned char count1 = 0;
volatile unsigned char second = 0;
volatile unsigned char led_flag = 0;

unsigned char led_data = 0xFF;

void Delay(unsigned int t)
{
    while(t--);
}

void WriteLED()
{
    P2 = (P2 & 0x1F) | 0x80;
    P0 = led_data;

    P2 = (P2 & 0x1F) | 0xC0;
    P0 = 0x00;
}

void InitBoard()
{
    P2 = (P2 & 0x1F) | 0xA0;
    P0 = 0x00;

    P2 = (P2 & 0x1F) | 0xE0;
    P0 = 0xFF;

    P2 = (P2 & 0x1F) | 0xC0;
    P0 = 0x00;

    led_data = 0xFF;
    WriteLED();
}

void TimerInit()
{
    TMOD = 0x11;

    TH0 = 0x3C;
    TL0 = 0xB0;

    TH1 = 0x3C;
    TL1 = 0xB0;

    EA = 1;
    ET0 = 1;
    ET1 = 1;

    TR0 = 1;
    TR1 = 1;
}

void ScanDisplay()
{
    unsigned char tens;
    unsigned char units;

    tens = second / 10;
    units = second % 10;

    P2 = (P2 & 0x1F) | 0xC0;
    P0 = 0x00;

    P2 = (P2 & 0x1F) | 0xE0;
    P0 = table[tens];

    P2 = (P2 & 0x1F) | 0xC0;
    P0 = 0x01;

    Delay(300);

    P0 = 0x00;

    P2 = (P2 & 0x1F) | 0xE0;
    P0 = table[units];

    P2 = (P2 & 0x1F) | 0xC0;
    P0 = 0x02;

    Delay(300);

    P0 = 0x00;
}

void main()
{
    InitBoard();
    TimerInit();

    while(1)
    {
        ScanDisplay();

        if(led_flag)
        {
            led_flag = 0;
            led_data ^= 0x01;
            WriteLED();
        }
    }
}

void Timer0() interrupt 1
{
    TH0 = 0x3C;
    TL0 = 0xB0;

    count0++;

    if(count0 >= 4)
    {
        count0 = 0;
        led_flag = 1;
    }
}

void Timer1() interrupt 3
{
    TH1 = 0x3C;
    TL1 = 0xB0;

    count1++;

    if(count1 >= 20)
    {
        count1 = 0;

        second++;

        if(second >= 60)
        {
            second = 0;
        }
    }
}