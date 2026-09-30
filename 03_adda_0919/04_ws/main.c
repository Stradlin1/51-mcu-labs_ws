#include <reg52.h>
#include <intrins.h>

#define FOSC 12000000UL
#define TIMER1_RELOAD (65536UL - FOSC / 12UL / 1000UL)

sfr AUXR = 0x8E;
sfr P4 = 0xC0;

sbit COL0 = P3^4;
sbit COL1 = P3^5;
sbit COL2 = P4^2;
sbit COL3 = P4^4;

unsigned char code table[16] = {
    0xC0, 0xF9, 0xA4, 0xB0,
    0x99, 0x92, 0x82, 0xF8,
    0x80, 0x90, 0x88, 0x83,
    0xC6, 0xA1, 0x86, 0x8E
};

unsigned char code rows[4] = {
    0xFE, 0xFD, 0xFB, 0xF7
};

volatile bit tick_1ms = 0;

volatile unsigned char display_buf[4] = {
    0xFF, 0xFF, 0xFF, 0xC0
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

    COL0 = 1;
    COL1 = 1;
    COL2 = 1;
    COL3 = 1;
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

    for(i = 0; i < 20; i++)
    {
        _nop_();
    }
}

unsigned char KeyRead(void)
{
    unsigned char row;
    unsigned char col;
    unsigned char count;

    row = 0;
    col = 0;
    count = 0;

    P3 = 0xF0;

    COL0 = 1;
    COL1 = 1;
    COL2 = 1;
    COL3 = 1;

    KeySettle();

    if(!COL0)
    {
        col = 0;
        count++;
    }

    if(!COL1)
    {
        col = 1;
        count++;
    }

    if(!COL2)
    {
        col = 2;
        count++;
    }

    if(!COL3)
    {
        col = 3;
        count++;
    }

    if(count == 0)
    {
        P3 = 0xFF;
        COL0 = 1;
        COL1 = 1;
        COL2 = 1;
        COL3 = 1;
        return 0;
    }

    if(count != 1)
    {
        P3 = 0xFF;
        COL0 = 1;
        COL1 = 1;
        COL2 = 1;
        COL3 = 1;
        return 0xFF;
    }

    P3 = 0xFF;

    COL0 = 1;
    COL1 = 1;
    COL2 = 1;
    COL3 = 1;

    if(col == 0)
    {
        COL0 = 0;
    }
    else if(col == 1)
    {
        COL1 = 0;
    }
    else if(col == 2)
    {
        COL2 = 0;
    }
    else
    {
        COL3 = 0;
    }

    KeySettle();

    count = 0;

    if(!(P3 & 0x01))
    {
        row = 0;
        count++;
    }

    if(!(P3 & 0x02))
    {
        row = 1;
        count++;
    }

    if(!(P3 & 0x04))
    {
        row = 2;
        count++;
    }

    if(!(P3 & 0x08))
    {
        row = 3;
        count++;
    }

    P3 = 0xFF;

    COL0 = 1;
    COL1 = 1;
    COL2 = 1;
    COL3 = 1;

    if(count != 1)
    {
        return 0xFF;
    }

    return row * 4 + col + 1;
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

    if(pos == 4)
    {
        pos = 0;
    }
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
void main(void)
{
    unsigned char key;
    unsigned char key2;

    BoardInit();
    Timer1Init();

    while(1)
    {
        key = KeyRead();

        if(key >= 1 && key <= 16)
        {
            DelayMs(20);

            key2 = KeyRead();

            if(key2 == key)
            {
                DisplaySquare(key);

                while(KeyRead() != 0);

                DelayMs(20);
            }
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