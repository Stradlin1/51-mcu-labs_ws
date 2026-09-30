#include <reg52.h>

sbit led1=P0^0;
sbit led2=P0^1;
sbit led3=P0^2;
sbit led4=P0^3;

sbit key2=P3^0;
sbit key6=P3^1;
sbit key10=P3^2;
sbit key14=P3^3;

sbit HC138_A=P2^5;
sbit HC138_B=P2^6;
sbit HC138_C=P2^7;

void delay(unsigned int t)
{
    while(t--);
    while(t--);
}

void keyscan()
{
    if(key2==0)
    {
        delay(600);
        if(key2==0)
        {
            led1=0;
            while(key2==0);
            led1=1;
        }
    }

    if(key6==0)
    {
        delay(600);
        if(key6==0)
        {
            led2=0;
            while(key6==0);
            led2=1;
        }
    }

    if(key10==0)
    {
        delay(600);
        if(key10==0)
        {
            led3=0;
            while(key10==0);
            led3=1;
        }
    }

    if(key14==0)
    {
        delay(600);
        if(key14==0)
        {
            led4=0;
            while(key14==0);
            led4=1;
        }
    }
}

void main()
{
    HC138_C=1;
    HC138_B=0;
    HC138_A=0;

    P0=0xFF;

    while(1)
    {
        keyscan();
    }
}