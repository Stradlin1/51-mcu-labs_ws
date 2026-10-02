#include <reg52.h>
#include <intrins.h>

#define FOSC 12000000UL
#define TIMER1_RELOAD (65536UL - FOSC / 12UL / 1000UL)

sfr AUXR = 0x8E;
sfr P4 = 0xC0;

sbit ROW1 = P3^0;
sbit ROW2 = P3^1;
sbit ROW3 = P3^2;
sbit ROW4 = P3^3;

sbit COL1 = P4^4;
sbit COL2 = P4^2;
sbit COL3 = P3^5;
sbit COL4 = P3^4;

unsigned char code table[16] = {
    0xC0, 0xF9, 0xA4, 0xB0,
    0x99, 0x92, 0x82, 0xF8,
    0x80, 0x90, 0x88, 0x83,
    0xC6, 0xA1, 0x86, 0x8E
};

volatile bit tick_1ms = 0;

volatile unsigned char display_buf[4] = {
    0xFF, 0xFF, 0xFF, 0xFF
};

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

    LatchWrite(0x80, 0xFF);
    LatchWrite(0xA0, 0x00);
    LatchWrite(0xC0, 0x00);
    LatchWrite(0xE0, 0xFF);

    P2 &= 0x1F;
    P0 = 0x00;

    ROW1 = 1;
    ROW2 = 1;
    ROW3 = 1;
    ROW4 = 1;

    COL1 = 1;
    COL2 = 1;
    COL3 = 1;
    COL4 = 1;
}

void Timer1Init(void)
{
    TR1 = 0;

    AUXR &= 0xBF;

    TMOD &= 0x0F;
    TMOD |= 0x10;

    TH1 = (unsigned char)(TIMER1_RELOAD >> 8);
    TL1 = (unsigned char)TIMER1_RELOAD;

    TF1 = 0;
    ET1 = 1;
    TR1 = 1;
    EA = 1;
}

void KeySettle(void)
{
    unsigned char i;

    for(i = 0; i < 50; i++)
    {
        _nop_();
    }
}

unsigned char KeyScan(void)
{
    ROW1 = 1;
    ROW2 = 1;
    ROW3 = 1;
    ROW4 = 1;

    COL1 = 1;
    COL2 = 1;
    COL3 = 1;
    COL4 = 1;

    COL4 = 0;
    KeySettle();

    if(ROW1 == 0)
    {
        COL4 = 1;
        return 4;
    }

    if(ROW2 == 0)
    {
        COL4 = 1;
        return 8;
    }

    if(ROW3 == 0)
    {
        COL4 = 1;
        return 12;
    }

    if(ROW4 == 0)
    {
        COL4 = 1;
        return 16;
    }

    COL4 = 1;
    KeySettle();

    COL1 = 0;
    KeySettle();

    if(ROW1 == 0)
    {
        COL1 = 1;
        return 1;
    }

    if(ROW2 == 0)
    {
        COL1 = 1;
        return 5;
    }

    if(ROW3 == 0)
    {
        COL1 = 1;
        return 9;
    }

    if(ROW4 == 0)
    {
        COL1 = 1;
        return 13;
    }

    COL1 = 1;
    KeySettle();

    COL2 = 0;
    KeySettle();

    if(ROW1 == 0)
    {
        COL2 = 1;
        return 2;
    }

    if(ROW2 == 0)
    {
        COL2 = 1;
        return 6;
    }

    if(ROW3 == 0)
    {
        COL2 = 1;
        return 10;
    }

    if(ROW4 == 0)
    {
        COL2 = 1;
        return 14;
    }

    COL2 = 1;
    KeySettle();

    COL3 = 0;
    KeySettle();

    if(ROW1 == 0)
    {
        COL3 = 1;
        return 3;
    }

    if(ROW2 == 0)
    {
        COL3 = 1;
        return 7;
    }

    if(ROW3 == 0)
    {
        COL3 = 1;
        return 11;
    }

    if(ROW4 == 0)
    {
        COL3 = 1;
        return 15;
    }

    COL3 = 1;

    return 0;
}

void DelayMs(unsigned int ms)
{
    while(ms)
    {
        if(tick_1ms)
        {
            tick_1ms = 0;
            ms--;
        }
    }
}

unsigned char KeyEvent(void)
{
    static unsigned char locked = 0;
    unsigned char key;

    key = KeyScan();

    if(locked == 0)
    {
        if(key >= 1 && key <= 16)
        {
            DelayMs(10);

            if(KeyScan() == key)
            {
                locked = 1;
                return key;
            }
        }
    }
    else
    {
        if(key == 0)
        {
            DelayMs(10);

            if(KeyScan() == 0)
            {
                locked = 0;
            }
        }
    }

    return 0;
}

void DisplaySquare(unsigned char key)
{
    unsigned int value;
    unsigned char hundreds;
    unsigned char tens;
    unsigned char units;

    value = (unsigned int)key * key;

    hundreds = 0xFF;
    tens = 0xFF;

    if(value >= 100)
    {
        hundreds = table[value / 100];
    }

    if(value >= 10)
    {
        tens = table[(value / 10) % 10];
    }

    units = table[value % 10];

    EA = 0;

    display_buf[0] = 0xFF;
    display_buf[1] = hundreds;
    display_buf[2] = tens;
    display_buf[3] = units;

    EA = 1;
}

void DisplayScan(void)
{
    static unsigned char pos = 0;

    LatchWrite(0xC0, 0x00);
    LatchWrite(0xE0, display_buf[pos]);
    LatchWrite(0xC0, (unsigned char)(1 << pos));

    pos++;

    if(pos >= 4)
    {
        pos = 0;
    }
}

void main(void)
{
    unsigned char key;

    BoardInit();
    Timer1Init();

    while(1)
    {
        key = KeyEvent();

        if(key >= 1 && key <= 16)
        {
            DisplaySquare(key);
        }
    }
}

void Timer1_ISR(void) interrupt 3
{
    TH1 = (unsigned char)(TIMER1_RELOAD >> 8);
    TL1 = (unsigned char)TIMER1_RELOAD;

    DisplayScan();

    tick_1ms = 1;
}