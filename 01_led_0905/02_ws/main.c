#include <reg52.h>

sbit HC138_A = P2^5;
sbit HC138_B = P2^6;
sbit HC138_C = P2^7;

sbit RELAY = P0^4;
sbit BEEP  = P0^6;


void Delay(unsigned int t)
{
    while(t--);
    while(t--);
}



void LED_Write(unsigned char dat)
{
    
    HC138_C = 1;
    HC138_B = 0;
    HC138_A = 0;

    
    P0 = dat;

    HC138_C = 1;
    HC138_B = 0;
    HC138_A = 1;


    P0 = 0x00;
}


void main()
{
    unsigned char i;

    P0 = 0x00;

    HC138_A = 0;
    HC138_B = 0;
    HC138_C = 0;



    for(i = 0; i < 3; i++)
    {
        LED_Write(0x00);      
        Delay(60000);
        Delay(60000);

        LED_Write(0xFF);      

        Delay(60000);
        Delay(60000);
    }



    LED_Write(0xFF);


    

    RELAY = 1;       

    BEEP = 1;        

    Delay(60000);
    Delay(60000);

    BEEP = 0;        

   

    while(1);
}