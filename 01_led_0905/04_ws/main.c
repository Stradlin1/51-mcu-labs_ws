#include <reg52.h>

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

void Display_Init()
{
    P2 = (P2 & 0x1F) | 0xC0;
    P0 = 0x00;

    P2 = (P2 & 0x1F) | 0xE0;
    P0 = 0xFF;

    P2 = (P2 & 0x1F) | 0xA0;
    P0 = 0x00;
}

void LED_Write(unsigned char dat)
{
    P2 = (P2 & 0x1F) | 0x80;
    P0 = dat;

    P2 = (P2 & 0x1F) | 0xA0;
    P0 = 0x00;
}

void main()
{
    unsigned char round;
    unsigned char i;
    unsigned char led_data;

    Display_Init();

    LED_Write(0xFF);

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