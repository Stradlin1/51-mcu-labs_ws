#include <reg52.h>

sbit HC138_A = P2^5;
sbit HC138_B = P2^6;
sbit HC138_C = P2^7;


/* ???? */
void Delay(unsigned int t)
{
    while(t--);
    while(t--);
}


/* ?? 500ms */
void Delay500ms()
{
    Delay(60000);
    Delay(60000);
}


/* ? LED ?????? */
void LED_Write(unsigned char dat)
{
    /* 100:?? Y4 */
    HC138_C = 1;
    HC138_B = 0;
    HC138_A = 0;

    /* LED ???? 74HC573 */
    P0 = dat;

    /*
       101:?? Y4
       74HC573 ????? LED ??
    */
    HC138_C = 1;
    HC138_B = 0;
    HC138_A = 1;

    /*
       ???? P0

       P0.6 = 0 ? ?????
       P0.4 = 0 ? ?????

       LED ????,???????
    */
    P0 = 0x00;
}


void main()
{
    unsigned char i;

    /*
       ???????:
       ???????????
    */
    P0 = 0x00;

    for(i = 0; i < 3; i++)
    {
        /* LED ?? */
        LED_Write(0x00);
        Delay500ms();

        /* LED ?? */
        LED_Write(0xFF);
        Delay500ms();

        /*
           ??????

           0x40 = 0100 0000
                   ?
                 P0.6 = 1

           P0.4 ?? = 0
           ?????????
        */
        P0 = 0x40;
        Delay500ms();

        /* ????? */
        P0 = 0x00;
    }

    /* ????? LED ?? */
    LED_Write(0xFF);

    /* ??????????? */
    P0 = 0x00;

    while(1);
}