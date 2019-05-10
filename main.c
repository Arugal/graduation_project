#include<reg52.h>
#include<intrins.h>

#define MAIN_Fosc
#define PCF8591_ADDR    0x90
#define DACOUT_EN        0x40
typedef unsigned char INT8U;
typedef unsigned char uchar;
typedef unsigned int INT16U;
typedef unsigned int uint;
sbit SDA = P2 ^0;
sbit SCL = P2 ^1;
sbit DU = P2 ^6;
sbit WE = P2 ^7;
sbit key2 = P3 ^0;
sbit key3 = P3 ^1;
sbit key4 = P3 ^2;
sbit key5 = P3 ^3;
uchar AD_Value, f;
uchar code
table[]={
0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7F,0x6F, 0x77, 0x7C, 0x39, 0x5E, 0x79,0x71, 0x40, 0x80, 0x00};
uchar code
T_COM[] = {
0xfe, 0xfd, 0xfb, 0xf7, 0xef, 0xdf, 0xbf, 0x7f};

void init();

void Delay_Ms(INT16U ms);

void Delay5us();

void Display(INT8U Value);

void I2C_Start();

void I2C_Stop();

void Master_ACK(bit i);

bit Test_ACK();

void I2C_send_byte(uchar byte);

uchar I2C_read_byte();

bit DAC_OUT(uchar DAT);

bit ADC_Read(uchar CON);

void judge();

void main() {
    init();
    while (1) {
        judge();
        switch (f) {
            case 1: {
                ADC_Read(0x02);
                DAC_OUT(AD_Value);
                Display(AD_Value);
                DAC_OUT(ADC_Read);
                Delay_Ms(5);
            }
                break;
            case 2: {
                ADC_Read(0x02);
                Display(AD_Value);
                DAC_OUT(70);
            }
            case 3: {
                ADC_Read(0x02);
                Display(AD_Value);
                DAC_OUT(140);
            }
            case 4: {
                ADC_Read(0x02);
                Display(AD_Value);
                DAC_OUT(255);
            }
        }
    }
}

void Delay_Ms(INT16U ms) {
    INT16U i;
    do {
        i = MAIN_Fosc / 96000;
        while (--i);   //96T per loop
    } while (--ms);
}

void Delay5us() {
#if MAIN_Fosc == 11059200
    _nop_();
#elif MAIN_Fosc == 12000000
    _nop_()
#elif MAIN_Fosc == 22118400
    _nop_(); _nop_(); _nop_();
#endif
}

void Display(INT8U Value) {
    DU = 1;
    P0 = table[Value / 100];
    DU = 0;
    P0 = 0xff;
    WE = 1;
    P0 = T_COM[0];
    WE = 0;
    Delay_Ms(5);
    DU = 1;
    P0 = table[Value % 100 / 10];
    DU = 0;
    P0 = 0xff;
    WE = 1;
    P0 = T_COM[1];
    WE = 0;
    Delay_Ms(5);
    DU = 1;
    P0 = table[Value % 10];
    DU = 0;
    P0 = 0xff;
    WE = 1;
    P0 = T_COM[2];
    WE = 0;
    Delay_Ms(5);
}

void init() {
    P3 = 0xff;
    SDA = 1;
    _nop_();
    SCL = 1;
    _nop_();
}

void I2C_Start() {
    SCL = 1;
    _nop_();
    SDA = 1;
    Delay5us();
    SDA = 0;
    Delay5us();
}

void I2C_Stop() {
    SDA = 0;
    _nop_();
    SCL = 1;
    Delay5us();
    SDA = 1;
    Delay5us();
}

void Master_ACK(bit i) {
    SCL = 0;
    _nop_();
    if (i) {
        SDA = 0;
    } else {
        SDA = 1;
    }
    _nop_();
    SCL = 1;
    _nop_();
    SCL = 0;
    _nop_();
    SDA = 1;
    _nop_();
}

bit Test_ACK() {
    SCL = 1;
    Delay5us();
    if (SDA) {
        SCL = 0;
        I2C_Stop();
        return (0);
    } else {
        SCL = 0;
        return (1);
    }
}

void I2C_send_byte(uchar byte) {
    uchar i;
    for (i = 0; i < 8; i++) {
        SCL = 0;
        _nop_();
        if (byte & 0x80)    //
        {
            SDA = 1;
            _nop_();
        } else {
            SDA = 0;
            _nop_();
        }
        SCL = 1;
        _nop_();
        byte <<= 1;
    }
    SCL = 0;
    _nop_();
    SDA = 1;
    _nop_();
}

uchar I2C_read_byte() {
    uchar i, dat;
    SCL = 0;
    _nop_();
    SDA = 1;
    _nop_();
    for (i = 0; i < 8; i++) {
        SCL = 1;
        _nop_();
        dat <<= 1;
        if (SDA) {
            dat |= 0x01;
        }
        _nop_();
        SCL = 0;
        _nop_();
    }
    return (dat);
}

bit DAC_OUT(uchar DAT) {
    I2C_Start();
    I2C_send_byte(PCF8591_ADDR + 0);
    if (!Test_ACK()) {
        return (0);
    }
    I2C_send_byte(DACOUT_EN);
    if (!Test_ACK()) {
        return (0);
    }
    I2C_send_byte(DAT);
    if (!Test_ACK()) {
        return (0);
    }
    I2C_Stop();
    return (1);
}

bit ADC_Read(uchar CON) {
    I2C_Start();
    I2C_send_byte(PCF8591_ADDR + 0);
    if (!Test_ACK()) {
        return (0);
    }
    I2C_send_byte(CON);
    Master_ACK(0);
    I2C_Start();
    I2C_send_byte(PCF8591_ADDR + 1);
    if (!Test_ACK()) {
        return (0);
    }
    AD_Value = I2C_read_byte();
    Master_ACK(0);
    I2C_Stop();

    return (1);
}

void judge() {

    if (key2 == 0)
        f = 1;
    if (key3 == 0)
        f = 2;
    if (key4 == 0)
        f = 3;
    if (key5 == 0)
        f = 4;
}