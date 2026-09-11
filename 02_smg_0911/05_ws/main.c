#include <reg52.h>

unsigned char code table[] = {
    0xc0, 0xf9, 0xa4, 0xb0,
    0x99, 0x92, 0x82, 0xf8,
    0x80, 0x90, 0x88, 0x83,
    0xc6, 0xa1, 0x86, 0x8e,
    0xbf
};

unsigned char display_buf[8];

void Delay(unsigned int t)
{
    while(t--);
}

void InitBoard()
{
    P0 = 0x00;

    P2 = (P2 & 0x1F) | 0xA0;
    P0 = 0x00;

    P2 = (P2 & 0x1F) | 0xE0;
    P0 = 0xFF;

    P2 = (P2 & 0x1F) | 0xC0;
    P0 = 0x00;

    P2 = (P2 & 0x1F) | 0x80;
    P0 = 0xFF;

    P2 = (P2 & 0x1F) | 0xC0;
    P0 = 0x00;
}

void UpdateDisplay(unsigned char month)
{
    display_buf[0] = 2;
    display_buf[1] = 0;
    display_buf[2] = 2;
    display_buf[3] = 5;
    display_buf[4] = 16;
    display_buf[5] = 16;
    display_buf[6] = month / 10;
    display_buf[7] = month % 10;
}

void ScanDisplay()
{
    unsigned char i;
    unsigned char position;

    position = 0x01;

    for(i = 0; i < 8; i++)
    {
        P2 = (P2 & 0x1F) | 0xC0;
        P0 = 0x00;

        P2 = (P2 & 0x1F) | 0xE0;
        P0 = table[display_buf[i]];

        P2 = (P2 & 0x1F) | 0xC0;
        P0 = position;

        Delay(300);

        P0 = 0x00;

        position = position << 1;
    }
}

void main()
{
    unsigned char month;
    unsigned int t;

    InitBoard();

    month = 1;

    while(1)
    {
        UpdateDisplay(month);

        for(t = 0; t < 500; t++)
        {
            ScanDisplay();
        }

        month++;

        if(month > 12)
        {
            month = 1;
        }
    }
}