#include <reg52.h>
#include <intrins.h>
#include <stdio.h>

sfr AUXR = 0x8E;

sbit SDA = P2^1;
sbit SCL = P2^0;

#define LDR 0x41
#define VA  0x43
#define DELAY_TIME 5

unsigned char pos = 0;
char string[10];
unsigned char buf[8] = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};
unsigned char delay_seg = 0;
unsigned char ADC_value = 0;

void I2CDelay(void)
{
    unsigned char i;
    for(i = 0; i < DELAY_TIME; i++)
    {
        _nop_();
    }
}

void I2CStart(void)
{
    SDA = 1;
    SCL = 1;
    I2CDelay();
    SDA = 0;
    I2CDelay();
    SCL = 0;
    I2CDelay();
}

void I2CStop(void)
{
    SDA = 0;
    SCL = 0;
    I2CDelay();
    SCL = 1;
    I2CDelay();
    SDA = 1;
    I2CDelay();
}

void I2CSendByte(unsigned char dat)
{
    unsigned char i;

    for(i = 0; i < 8; i++)
    {
        SDA = (dat & 0x80) ? 1 : 0;
        I2CDelay();
        SCL = 1;
        I2CDelay();
        SCL = 0;
        I2CDelay();
        dat <<= 1;
    }

    SDA = 1;
}

bit I2CWaitAck(void)
{
    bit ack;

    SDA = 1;
    I2CDelay();
    SCL = 1;
    I2CDelay();
    ack = SDA;
    SCL = 0;
    I2CDelay();

    return ack;
}

unsigned char I2CReceiveByte(void)
{
    unsigned char i;
    unsigned char dat = 0;

    SDA = 1;

    for(i = 0; i < 8; i++)
    {
        SCL = 1;
        I2CDelay();
        dat <<= 1;

        if(SDA)
        {
            dat |= 0x01;
        }

        SCL = 0;
        I2CDelay();
    }

    return dat;
}

void I2CSendAck(bit ack)
{
    SDA = ack;
    I2CDelay();
    SCL = 1;
    I2CDelay();
    SCL = 0;
    I2CDelay();
    SDA = 1;
}

unsigned char PCF8591_ADC(unsigned char addr)
{
    unsigned char temp;

    I2CStart();
    I2CSendByte(0x90);
    I2CWaitAck();

    I2CSendByte(addr);
    I2CWaitAck();

    I2CStart();
    I2CSendByte(0x91);
    I2CWaitAck();

    temp = I2CReceiveByte();
    I2CSendAck(1);
    I2CStop();

    return temp;
}

void InitSystem(void)
{
    P0 = 0xFF;
    P2 = (P2 & 0x1F) | 0x80;
    P2 &= 0x1F;

    P0 = 0x00;
    P2 = (P2 & 0x1F) | 0xA0;
    P2 &= 0x1F;

    P0 = 0x00;
    P2 = (P2 & 0x1F) | 0xC0;
    P2 &= 0x1F;

    P0 = 0xFF;
    P2 = (P2 & 0x1F) | 0xE0;
    P2 &= 0x1F;
}

void Timer1Init(void)
{
    AUXR &= 0xBF;
    TMOD &= 0x0F;
    TL1 = 0x18;
    TH1 = 0xFC;
    TF1 = 0;
    TR1 = 1;
    ET1 = 1;
}

void Display_Seg(unsigned char p, unsigned char *b)
{
    P0 = 0xFF;
    P2 = (P2 & 0x1F) | 0xE0;
    P2 &= 0x1F;

    P0 = 0x01 << p;
    P2 = (P2 & 0x1F) | 0xC0;
    P2 &= 0x1F;

    P0 = b[p];
    P2 = (P2 & 0x1F) | 0xE0;
    P2 &= 0x1F;
}

void Tran_string(char *str, unsigned char *b)
{
    unsigned char i = 0;
    unsigned char j = 0;
    unsigned char temp;

    for(i = 0; i < 8; i++, j++)
    {
        switch(str[j])
        {
            case '0': temp = 0xC0; break;
            case '1': temp = 0xF9; break;
            case '2': temp = 0xA4; break;
            case '3': temp = 0xB0; break;
            case '4': temp = 0x99; break;
            case '5': temp = 0x92; break;
            case '6': temp = 0x82; break;
            case '7': temp = 0xF8; break;
            case '8': temp = 0x80; break;
            case '9': temp = 0x90; break;
            case 'A': temp = 0x88; break;
            case 'B': temp = 0x83; break;
            case 'C': temp = 0xC6; break;
            case 'D': temp = 0xA1; break;
            case 'E': temp = 0x86; break;
            case 'F': temp = 0x8E; break;
            case '.': temp = 0x7F; break;
            case '-': temp = 0xBF; break;
            default: temp = 0xFF; break;
        }

        if(str[j + 1] == '.')
        {
            temp &= 0x7F;
            j++;
        }

        b[i] = temp;
    }
}

void UpdateADCDisplay(void)
{
    ADC_value = PCF8591_ADC(VA);
    sprintf(string, "-----%03u", (unsigned int)ADC_value);
    Tran_string(string, buf);
}

void Seg_Process(void)
{
    if(delay_seg)
    {
        return;
    }

    delay_seg = 1;
    UpdateADCDisplay();
}

void ServiceTimer1(void) interrupt 3
{
    if(++delay_seg == 200)
    {
        delay_seg = 0;
    }

    if(++pos == 8)
    {
        pos = 0;
    }

    Display_Seg(pos, buf);
}

void main(void)
{
    InitSystem();

    SDA = 1;
    SCL = 1;

    UpdateADCDisplay();
    delay_seg = 1;

    Timer1Init();
    EA = 1;

    while(1)
    {
        Seg_Process();
    }
}