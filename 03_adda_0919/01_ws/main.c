#include <reg52.h>
#include <intrins.h>

sbit LED1 = P0^0;
sbit LED2 = P0^1;
sbit LED3 = P0^2;
sbit LED4 = P0^3;

sbit KEY2 = P3^0;
sbit KEY6 = P3^1;
sbit KEY10 = P3^2;
sbit KEY14 = P3^3;

void BoardInit(void)
{
    EA = 0;
    P3 = 0xFF;

    P2 &= 0x1F;
    P0 = 0x00;
    P2 |= 0xA0;
    _nop_();
    P2 &= 0x1F;

    P0 = 0x00;
    P2 |= 0xC0;
    _nop_();
    P2 &= 0x1F;

    P0 = 0xFF;
    P2 |= 0xE0;
    _nop_();
    P2 &= 0x1F;

    P0 = 0xFF;
    P2 |= 0x80;
    _nop_();
}

void main(void)
{
    BoardInit();

    while(1)
    {
        LED1 = KEY2;
        LED2 = KEY6;
        LED3 = KEY10;
        LED4 = KEY14;
    }
}
