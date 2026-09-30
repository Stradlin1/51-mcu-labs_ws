#include <reg52.h>
#include <absacc.h>
#include <intrins.h>

sbit DS1302_RST  = P1^3;
sbit DS1302_SCLK = P1^7;
sbit DS1302_IO   = P2^3;

#define LED_PORT   XBYTE[0x8000]
#define CTRL_PORT  XBYTE[0xA000]
#define SMG_WEI    XBYTE[0xC000]
#define SMG_DUAN   XBYTE[0xE000]

#define SHOW_COUNT 400

unsigned char year;
unsigned char month;
unsigned char date;
unsigned char week;
unsigned char hour;
unsigned char minute;
unsigned char second;

unsigned char code table[] =
{
    0xc0,0xf9,0xa4,0xb0,
    0x99,0x92,0x82,0xf8,
    0x80,0x90
};

void DelaySMG(unsigned int t)
{
    while(t--);
}

void HardwareInit()
{
    EA = 0;

    LED_PORT = 0xFF;
    CTRL_PORT = 0x00;

    SMG_WEI = 0x00;
    SMG_DUAN = 0xFF;

    DS1302_RST = 0;
    DS1302_SCLK = 0;
    DS1302_IO = 0;
}

void DS1302_WriteByte(unsigned char dat)
{
    unsigned char i;

    for(i = 0; i < 8; i++)
    {
        DS1302_SCLK = 0;

        DS1302_IO = dat & 0x01;
        _nop_();

        DS1302_SCLK = 1;
        _nop_();

        DS1302_SCLK = 0;

        dat >>= 1;
    }
}

void DS1302_WriteReg(unsigned char addr, unsigned char dat)
{
    DS1302_RST = 0;
    DS1302_SCLK = 0;

    DS1302_RST = 1;

    DS1302_WriteByte(addr);
    DS1302_WriteByte(dat);

    DS1302_RST = 0;
    DS1302_SCLK = 0;
}

unsigned char DS1302_ReadReg(unsigned char addr)
{
    unsigned char i;
    unsigned char dat = 0x00;

    DS1302_RST = 0;
    DS1302_SCLK = 0;

    DS1302_RST = 1;

    DS1302_WriteByte(addr);

    DS1302_IO = 1;

    for(i = 0; i < 8; i++)
    {
        dat >>= 1;

        if(DS1302_IO)
        {
            dat |= 0x80;
        }

        DS1302_SCLK = 1;
        _nop_();

        DS1302_SCLK = 0;
        _nop_();
    }

    DS1302_RST = 0;
    DS1302_SCLK = 0;
    DS1302_IO = 0;

    return dat;
}

void DS1302_Init()
{
    DS1302_WriteReg(0x8E, 0x00);

    DS1302_WriteReg(0x80, 0x80);

    DS1302_WriteReg(0x82, 0x09);
    DS1302_WriteReg(0x84, 0x16);

    DS1302_WriteReg(0x86, 0x30);
    DS1302_WriteReg(0x88, 0x09);
    DS1302_WriteReg(0x8A, 0x03);
    DS1302_WriteReg(0x8C, 0x26);

    DS1302_WriteReg(0x80, 0x00);

    DS1302_WriteReg(0x8E, 0x80);
}

unsigned char BCDToDec(unsigned char bcd)
{
    return (bcd / 16) * 10 + (bcd % 16);
}

void ReadCalendar()
{
    second = DS1302_ReadReg(0x81);
    minute = DS1302_ReadReg(0x83);
    hour   = DS1302_ReadReg(0x85);

    date   = DS1302_ReadReg(0x87);
    month  = DS1302_ReadReg(0x89);
    week   = DS1302_ReadReg(0x8B);
    year   = DS1302_ReadReg(0x8D);

    second &= 0x7F;
    hour &= 0x3F;

    second = BCDToDec(second);
    minute = BCDToDec(minute);
    hour   = BCDToDec(hour);

    date   = BCDToDec(date);
    month  = BCDToDec(month);
    year   = BCDToDec(year);
}

void DisplayBit(unsigned char pos, unsigned char dat)
{
    SMG_WEI = 0x00;
    SMG_DUAN = 0xFF;

    SMG_WEI = (unsigned char)(0x01 << pos);
    SMG_DUAN = dat;

    DelaySMG(500);

    SMG_WEI = 0x00;
    SMG_DUAN = 0xFF;
}

void DisplayDate()
{
    DisplayBit(0, table[year / 10]);
    DisplayBit(1, table[year % 10]);

    DisplayBit(2, 0xBF);

    DisplayBit(3, table[month / 10]);
    DisplayBit(4, table[month % 10]);

    DisplayBit(5, 0xBF);

    DisplayBit(6, table[date / 10]);
    DisplayBit(7, table[date % 10]);
}

void DisplayWeek()
{
    DisplayBit(0, 0xBF);
    DisplayBit(1, 0xBF);
    DisplayBit(2, 0xBF);

    if(week >= 1 && week <= 7)
    {
        DisplayBit(3, table[week]);
    }
    else
    {
        DisplayBit(3, 0xFF);
    }

    DisplayBit(4, 0xBF);
    DisplayBit(5, 0xBF);
    DisplayBit(6, 0xBF);
    DisplayBit(7, 0xBF);
}

void DisplayTime()
{
    DisplayBit(0, table[hour / 10]);
    DisplayBit(1, table[hour % 10]);

    DisplayBit(2, 0xBF);

    DisplayBit(3, table[minute / 10]);
    DisplayBit(4, table[minute % 10]);

    DisplayBit(5, 0xBF);

    DisplayBit(6, table[second / 10]);
    DisplayBit(7, table[second % 10]);
}

void ShowDate()
{
    unsigned int i;

    for(i = 0; i < SHOW_COUNT; i++)
    {
        ReadCalendar();
        DisplayDate();
    }
}

void ShowWeek()
{
    unsigned int i;

    for(i = 0; i < SHOW_COUNT; i++)
    {
        ReadCalendar();
        DisplayWeek();
    }
}

void ShowTime()
{
    unsigned int i;

    for(i = 0; i < SHOW_COUNT; i++)
    {
        ReadCalendar();
        DisplayTime();
    }
}

void main()
{
    HardwareInit();
    DS1302_Init();

    while(1)
    {
        ShowDate();
        ShowWeek();
        ShowTime();
    }
}