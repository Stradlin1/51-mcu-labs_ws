#include <reg52.h>
#include <intrins.h>

#define FOSC 12000000UL
#define TIMER1_RELOAD (65536UL - FOSC / 12UL / 1000UL)

sfr AUXR = 0x8E;

#define ADC_CONTROL 0x03

sbit I2C_SCL = P2^0;
sbit I2C_SDA = P2^1;

unsigned char code table[16] = {
    0xC0, 0xF9, 0xA4, 0xB0,
    0x99, 0x92, 0x82, 0xF8,
    0x80, 0x90, 0x88, 0x83,
    0xC6, 0xA1, 0x86, 0x8E
};

volatile unsigned char display_buf[8] = {
    0xBF, 0xBF, 0xBF, 0xBF, 0xBF, 0xBF, 0xBF, 0xBF
};
volatile bit adc_due = 1;

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
}

void Timer1Init(void)
{
    TR1 = 0;
    AUXR &= 0xBF;
    TMOD &= 0x0F;
    TL1 = (unsigned char)TIMER1_RELOAD;
    TH1 = (unsigned char)(TIMER1_RELOAD >> 8);
    TF1 = 0;
    ET1 = 1;
    TR1 = 1;
    EA = 1;
}

void I2CDelay(void)
{
    unsigned char i;

    for(i = 0; i < 40; i++)
    {
        _nop_();
    }
}

void I2CStart(void)
{
    I2C_SDA = 1;
    I2CDelay();
    I2C_SCL = 1;
    I2CDelay();
    I2C_SDA = 0;
    I2CDelay();
    I2C_SCL = 0;
    I2CDelay();
}

void I2CStop(void)
{
    I2C_SCL = 0;
    I2C_SDA = 0;
    I2CDelay();
    I2C_SCL = 1;
    I2CDelay();
    I2C_SDA = 1;
    I2CDelay();
}

unsigned char I2CSendByte(unsigned char value)
{
    unsigned char i;
    unsigned char ack;

    for(i = 0; i < 8; i++)
    {
        I2C_SCL = 0;
        I2C_SDA = (value & 0x80) ? 1 : 0;
        I2CDelay();
        I2C_SCL = 1;
        I2CDelay();
        I2C_SCL = 0;
        value <<= 1;
    }
    I2C_SDA = 1;
    I2CDelay();
    I2C_SCL = 1;
    I2CDelay();
    ack = I2C_SDA ? 0 : 1;
    I2C_SCL = 0;
    I2CDelay();
    return ack;
}

unsigned char I2CReceiveByte(void)
{
    unsigned char i;
    unsigned char value;

    value = 0;
    I2C_SDA = 1;
    for(i = 0; i < 8; i++)
    {
        I2C_SCL = 0;
        I2CDelay();
        I2C_SCL = 1;
        I2CDelay();
        value <<= 1;
        if(I2C_SDA)
        {
            value |= 1;
        }
        I2C_SCL = 0;
    }
    return value;
}

void I2CSendAck(unsigned char nack)
{
    I2C_SCL = 0;
    I2C_SDA = nack ? 1 : 0;
    I2CDelay();
    I2C_SCL = 1;
    I2CDelay();
    I2C_SCL = 0;
    I2CDelay();
    I2C_SDA = 1;
}

unsigned char PCF8591Read(unsigned char *result)
{
    I2CStart();
    if(!I2CSendByte(0x90))
    {
        I2CStop();
        return 0;
    }
    if(!I2CSendByte(ADC_CONTROL))
    {
        I2CStop();
        return 0;
    }
    I2CStart();
    if(!I2CSendByte(0x91))
    {
        I2CStop();
        return 0;
    }
    I2CReceiveByte();
    I2CSendAck(0);
    *result = I2CReceiveByte();
    I2CSendAck(1);
    I2CStop();
    return 1;
}

void DisplayADC(unsigned char value, unsigned char valid)
{
    unsigned char hundreds;
    unsigned char tens;
    unsigned char units;

    hundreds = 0xBF;
    tens = 0xBF;
    units = 0xBF;
    if(valid)
    {
        hundreds = table[value / 100];
        tens = table[value / 10 % 10];
        units = table[value % 10];
    }
    EA = 0;
    display_buf[5] = hundreds;
    display_buf[6] = tens;
    display_buf[7] = units;
    EA = 1;
}

void DisplayScan(void)
{
    static unsigned char pos = 0;

    LatchWrite(0xC0, 0x00);
    LatchWrite(0xE0, display_buf[pos]);
    LatchWrite(0xC0, (unsigned char)(1 << pos));
    pos++;
    if(pos == 8)
    {
        pos = 0;
    }
}

void main(void)
{
    unsigned char value;
    unsigned char valid;

    BoardInit();
    I2C_SCL = 1;
    I2C_SDA = 1;
    value = 0;
    Timer1Init();
    while(1)
    {
        if(adc_due)
        {
            adc_due = 0;
            valid = PCF8591Read(&value);
            DisplayADC(value, valid);
        }
    }
}

void Timer1_ISR(void) interrupt 3
{
    static unsigned char elapsed = 0;

    DisplayScan();
    elapsed++;
    if(elapsed >= 200)
    {
        elapsed = 0;
        adc_due = 1;
    }
}
