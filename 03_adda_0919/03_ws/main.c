#include <reg52.h>
#include <intrins.h>

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

void DelayShort(void)
{
    unsigned char i;

    for(i = 0; i < 50; i++)
    {
        _nop_();
    }
}

void DelayKey(void)
{
    unsigned int i;

    for(i = 0; i < 5000; i++)
    {
        _nop_();
    }
}

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

void DisplayStatic(unsigned char value)
{
    LatchWrite(0xC0, 0x00);
    LatchWrite(0xE0, table[value]);
    LatchWrite(0xC0, 0x0F);
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
    DelayShort();

    if(ROW1 == 0)
    {
        COL4 = 1;
        return 3;
    }

    if(ROW2 == 0)
    {
        COL4 = 1;
        return 7;
    }

    if(ROW3 == 0)
    {
        COL4 = 1;
        return 11;
    }

    if(ROW4 == 0)
    {
        COL4 = 1;
        return 15;
    }

    COL4 = 1;
    DelayShort();

    COL1 = 0;
    DelayShort();

    if(ROW1 == 0)
    {
        COL1 = 1;
        return 0;
    }

    if(ROW2 == 0)
    {
        COL1 = 1;
        return 4;
    }

    if(ROW3 == 0)
    {
        COL1 = 1;
        return 8;
    }

    if(ROW4 == 0)
    {
        COL1 = 1;
        return 12;
    }

    COL1 = 1;
    DelayShort();

    COL2 = 0;
    DelayShort();

    if(ROW1 == 0)
    {
        COL2 = 1;
        return 1;
    }

    if(ROW2 == 0)
    {
        COL2 = 1;
        return 5;
    }

    if(ROW3 == 0)
    {
        COL2 = 1;
        return 9;
    }

    if(ROW4 == 0)
    {
        COL2 = 1;
        return 13;
    }

    COL2 = 1;
    DelayShort();

    COL3 = 0;
    DelayShort();

    if(ROW1 == 0)
    {
        COL3 = 1;
        return 2;
    }

    if(ROW2 == 0)
    {
        COL3 = 1;
        return 6;
    }

    if(ROW3 == 0)
    {
        COL3 = 1;
        return 10;
    }

    if(ROW4 == 0)
    {
        COL3 = 1;
        return 14;
    }

    COL3 = 1;

    return 0xFF;
}

unsigned char KeyEvent(void)
{
    static unsigned char locked = 0;
    unsigned char key;

    key = KeyScan();

    if(locked == 0)
    {
        if(key < 16)
        {
            DelayKey();

            if(KeyScan() == key)
            {
                locked = 1;
                return key;
            }
        }
    }
    else
    {
        if(key == 0xFF)
        {
            DelayKey();

            if(KeyScan() == 0xFF)
            {
                locked = 0;
            }
        }
    }

    return 0xFF;
}

void main(void)
{
    unsigned char key;

    BoardInit();

    while(1)
    {
        key = KeyEvent();

        if(key < 16)
        {
            DisplayStatic(key);
        }
    }
}