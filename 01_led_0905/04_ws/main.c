#include <reg52.h>

sbit HC138_A = P2^5;
sbit HC138_B = P2^6;
sbit HC138_C = P2^7;


void Delay(unsigned int t)
{
    while(t--);
    while(t--);
}


void Delay500ms()
{
    Delay(60000);
    Delay(60000);
}


void Delay3s()
{
    unsigned char i;

    for(i = 0; i < 6; i++)
    {
        Delay500ms();
    }
}


void LED_Write(unsigned char dat)
{
    HC138_C = 1;
    HC138_B = 0;
    HC138_A = 0;

    P0 = dat;

    HC138_C = 0;
    HC138_B = 0;
    HC138_A = 0;

    P0 = 0x00;
}


void main()
{
    unsigned char round;
    unsigned char i;
    unsigned char led_data;

    P0 = 0x00;

    HC138_C = 0;
    HC138_B = 0;
    HC138_A = 0;

    for(round = 0; round < 3; round++)
    {
        led_data = 0xFF;

        for(i = 0; i < 8; i++)
        {
            led_data = led_data << 1;

            LED_Write(led_data);

            Delay500ms();
        }

        if(round < 2)
        {
            Delay3s();
        }
    }

    while(1);
}