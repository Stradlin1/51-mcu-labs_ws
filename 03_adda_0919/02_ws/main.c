#include <reg52.h>
#include <intrins.h>

#define FOSC 12000000UL
#define TIMER1_RELOAD (65536UL - FOSC / 12UL / 1000UL)

sfr AUXR = 0x8E;

unsigned char code table[16] = {
    0xC0, 0xF9, 0xA4, 0xB0,
    0x99, 0x92, 0x82, 0xF8,
    0x80, 0x90, 0x88, 0x83,
    0xC6, 0xA1, 0x86, 0x8E
};

volatile bit tick_1ms = 0;

volatile unsigned char number = 0;
volatile unsigned int elapsed_ms = 0;
volatile bit auto_run = 0;

void LatchWrite(unsigned char select, unsigned char value)
{
    P2 &= 0x1F;
    P0 = value;
    P2 |= select;
    _nop_();
    P2 &= 0x1F;
    P0 = 0x00;
}

void BoardInit(void)
{
    EA = 0;
    P2 &= 0x1F;
    P0 = 0x00;
    P2 |= 0xA0;
    _nop_();
    P2 &= 0x1F;
    P2 |= 0xC0;
    _nop_();
    P2 &= 0x1F;
    P0 = 0xFF;
    P2 |= 0xE0;
    _nop_();
    P2 &= 0x1F;
    P2 |= 0x80;
    _nop_();
    P2 &= 0x1F;
    P0 = 0x00;
    P3 = 0xFF;
}

void Timer1Init(void)
{
    TR1 = 0;
    AUXR &= 0xBF;
    TMOD &= 0x0F;
    TL1 = (unsigned char)TIMER1_RELOAD;
    TH1 = (unsigned char)(TIMER1_RELOAD >> 8);
    TF1 = 0;
    ET1 = 1;
    TR1 = 1;
    EA = 1;
}

unsigned char KeyRead(void)
{
    unsigned char value;

    value = (unsigned char)(~P3) & 0x0F;
    switch(value)
    {
        case 0x00: return 0;
        case 0x01: return 1;
        case 0x02: return 2;
        case 0x04: return 3;
        case 0x08: return 4;
        default: return 0xFF;
    }
}

unsigned char KeyEvent(void)
{
    static unsigned char previous = 0;
    static unsigned char stable = 0;
    static unsigned char samples = 0;
    static unsigned char armed = 1;
    unsigned char raw;

    raw = KeyRead();
    if(raw != previous)
    {
        previous = raw;
        samples = 0;
    }
    else if(samples < 20)
    {
        samples++;
    }
    if(samples == 20 && raw != stable)
    {
        stable = raw;
        if(raw == 0)
        {
            armed = 1;
        }
        else if(armed)
        {
            armed = 0;
            if(raw != 0xFF)
            {
                return raw;
            }
        }
    }
    return 0;
}

void ApplyKey(unsigned char key)
{
    if(key == 0)
    {
        return;
    }
    EA = 0;
    switch(key)
    {
        case 1:
            if(number == 59)
            {
                number = 0;
            }
            else
            {
                number++;
            }
            break;
        case 2:
            if(number == 0)
            {
                number = 59;
            }
            else
            {
                number--;
            }
            break;
        case 3:
            number = 0;
            break;
        case 4:
            auto_run = !auto_run;
            break;
    }
    elapsed_ms = 0;
    EA = 1;
}

void DisplayScan(void)
{
    static unsigned char pos = 0;
    static unsigned char frame = 0;
    unsigned char digit;

    if(pos == 0)
    {
        frame = number;
        digit = frame / 10;
    }
    else
    {
        digit = frame % 10;
    }
    LatchWrite(0xC0, 0x00);
    LatchWrite(0xE0, table[digit]);
    LatchWrite(0xC0, (unsigned char)(1 << pos));
    pos ^= 1;
}

void main(void)
{
    unsigned char key;

    BoardInit();
    Timer1Init();
    while(1)
    {
        if(tick_1ms)
        {
            tick_1ms = 0;
            key = KeyEvent();
            ApplyKey(key);
        }
    }
}

void Timer1_ISR(void) interrupt 3
{
    if(auto_run)
    {
        elapsed_ms++;
        if(elapsed_ms >= 1000)
        {
            elapsed_ms = 0;
            number++;
            if(number >= 60)
            {
                number = 0;
            }
        }
    }
    DisplayScan();
    tick_1ms = 1;
}
