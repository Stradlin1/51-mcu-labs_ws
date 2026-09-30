#include <reg52.h>
#include <absacc.h>

sfr AUXR = 0x8e;

sbit TX = P1^0;
sbit RX = P1^1;

#define LED_PORT   XBYTE[0x8000]
#define CTRL_PORT  XBYTE[0xA000]
#define SMG_WEI    XBYTE[0xC000]
#define SMG_DUAN   XBYTE[0xE000]

unsigned int distance = 999;

unsigned char code table[] =
{
    0xc0,0xf9,0xa4,0xb0,
    0x99,0x92,0x82,0xf8,
    0x80,0x90,0x88,0x83,
    0xc6,0xa1,0x86,0x8e
};

void HardwareInit()
{
    EA = 0;

    TR0 = 0;
    TR1 = 0;

    AUXR &= 0x3F;

    TMOD = 0x01;

    TF0 = 0;
    TF1 = 0;

    LED_PORT = 0xFF;
    CTRL_PORT = 0x00;
    SMG_WEI = 0x00;
    SMG_DUAN = 0xFF;

    TX = 0;
    RX = 1;
}

void Delay12us()
{
    TR0 = 0;
    TF0 = 0;

    TH0 = 0xFF;
    TL0 = 0xF4;

    TR0 = 1;

    while(TF0 == 0);

    TR0 = 0;
    TF0 = 0;
}

void Delay1ms()
{
    TR0 = 0;
    TF0 = 0;

    TH0 = 0xFC;
    TL0 = 0x18;

    TR0 = 1;

    while(TF0 == 0);

    TR0 = 0;
    TF0 = 0;
}

void SendWave()
{
    unsigned char i;

    for(i = 0; i < 8; i++)
    {
        TX = 1;
        Delay12us();

        TX = 0;
        Delay12us();
    }
}

void MeasureDistance()
{
    unsigned int time;

    TR1 = 0;
    TF1 = 0;

    TH1 = 0x00;
    TL1 = 0x00;

    SendWave();

    TR1 = 1;

    while((RX == 1) && (TF1 == 0));

    TR1 = 0;

    if(TF1 == 0)
    {
        time = TH1;
        time = (time << 8) | TL1;

        distance = ((time / 10) * 17) / 100 + 3;

        if(distance > 130)
        {
            distance = 999;
        }
    }
    else
    {
        TF1 = 0;
        distance = 999;
    }
}

void DisplayBit(unsigned char pos, unsigned char dat)
{
    SMG_WEI = 0x00;
    SMG_DUAN = 0xFF;

    SMG_WEI = (unsigned char)(0x01 << pos);
    SMG_DUAN = dat;

    Delay1ms();

    SMG_WEI = 0x00;
    SMG_DUAN = 0xFF;
}

void DisplayDistance()
{
    if(distance == 999)
    {
        DisplayBit(0, table[15]);
    }
    else
    {
        DisplayBit(5, table[distance / 100]);
        DisplayBit(6, table[(distance % 100) / 10]);
        DisplayBit(7, table[distance % 10]);
    }
}

void main()
{
    unsigned char i;

    HardwareInit();

    while(1)
    {
        MeasureDistance();

        for(i = 0; i < 50; i++)
        {
            DisplayDistance();
        }
    }
}