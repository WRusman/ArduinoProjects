/*
 * Seeed_LDC1612.cpp
 * Driver for DIGITAL I2C HUMIDITY AND TEMPERATURE SENSOR
 *  
 * Copyright (c) 2018 Seeed Technology Co., Ltd.
 * Website    : www.seeed.cc
 * Author     : downey
 * Create Time: June 2018
 * Change Log :
 *
 * The MIT License (MIT)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */


#include "Seeed_LDC1612.h"
#include "math.h"

/**@brief read sensor's information
 * 
 * */
void LDC1612::read_sensor_infomation()
{
    u16 value=0;
    IIC_read_16bit(READ_MANUFACTURER_ID,&value);
    Serial.print("manufacturer id =0x");
    Serial.println(value,HEX);
    IIC_read_16bit(READ_DEVICE_ID,&value);
    Serial.print("DEVICE id =0x");
    Serial.println(value,HEX);
    return ;
}


/**@brief constructor,support set IIC addr,default iic addr is 0x2b.
 * 
 * */
LDC1612::LDC1612(u8 IIC_ADDR)
{
    set_iic_addr(IIC_ADDR);
}


/**@brief config sensor
 * 
 * */
s32 LDC1612::init()
{
    /*Start IIC communication!*/
    IIC_begin();

    /*reset sensor*/
    return 0;
}

s32 LDC1612::single_channel_config(u8 channel)
{
    /*Set coil inductor parameter first.*/
    /*20 TURNS*/
    set_Rp(CHANNEL_0,15.727);
    set_L(CHANNEL_0,18.147);
    set_C(CHANNEL_0,100);
    set_Q_factor(CHANNEL_0,35.97);

    // /*25 TURNS*/
    // set_Rp(CHANNEL_0,24.9);
    // set_L(CHANNEL_0,53.95);
    // set_C(CHANNEL_0,100);
    // set_Q_factor(CHANNEL_0,32.57);

    /*36 TURNS,single layer*/

    // set_Rp(CHANNEL_0,28.18);
    // set_L(CHANNEL_0,18.56);
    // set_C(CHANNEL_0,100);
    // set_Q_factor(CHANNEL_0,43.25);

    /*40 TURNS*/
    // set_Rp(CHANNEL_0,57.46);
    // set_L(CHANNEL_0,85.44);
    // set_C(CHANNEL_0,100);
    // set_Q_factor(CHANNEL_0,40.7);

    
    if(set_FIN_FREF_DIV(CHANNEL_0))
    {
        return -1;
    }

    set_LC_stabilize_time(CHANNEL_0);

    /*Set conversion interval time*/
    set_conversion_time(CHANNEL_0,0x0546);

    /*Set driver current!*/
    set_driver_current(CHANNEL_0,0xa000);

    /*single conversion*/
    set_mux_config(0x20c);
    /*start channel 0*/
    u16 config=0x1601;
    select_channel_to_convert(CHANNEL_0,&config);
    set_sensor_config(config);
    return 0;
}

s32 LDC1612::LDC1612_mutiple_channel_config()
{
    /*Set coil inductor parameter first.*/
    /*20 TURNS*/
    set_Rp(CHANNEL_0,15.727);
    set_L(CHANNEL_0,18.147);
    set_C(CHANNEL_0,100);
    set_Q_factor(CHANNEL_0,35.97);

    /*25 TURNS*/
    set_Rp(CHANNEL_1,15.727);
    set_L(CHANNEL_1,18.147);
    set_C(CHANNEL_1,100);
    set_Q_factor(CHANNEL_1,35.97);

    // /*36 TURNS single layer*/
    // set_Rp(CHANNEL_0,28.18);
    // set_L(CHANNEL_0,18.56);
    // set_C(CHANNEL_0,100);
    // set_Q_factor(CHANNEL_0,43.25);

    // /*40 TURNS*/
    // set_Rp(CHANNEL_1,57.46);
    // set_L(CHANNEL_1,85.44);
    // set_C(CHANNEL_1,100);
    // set_Q_factor(CHANNEL_0,40.7);


    if(set_FIN_FREF_DIV(CHANNEL_0))
    {
        return -1;
    }
    set_FIN_FREF_DIV(CHANNEL_1);

    /* 16*38/Fref=30us ,wait 30us for LC sensor stabilize before initiation of a conversion.*/
    set_LC_stabilize_time(CHANNEL_0);
    set_LC_stabilize_time(CHANNEL_1);

    /*Set conversion interval time*/
    set_conversion_time(CHANNEL_0,0x0546);
    set_conversion_time(CHANNEL_1,0x0546);

    /*Set driver current!*/
    set_driver_current(CHANNEL_0,0xa000);
    set_driver_current(CHANNEL_1,0xa000);

    
    /*mutiple conversion*/
    set_mux_config(0x820c);
    //set_mux_config(0x20c);
    /*start channel 0*/
    set_sensor_config(0x1601);
    //u16 config=0x1601;
    //select_channel_to_convert(0,&config);
    return 0;
}




/**@brief parse the data which read from data register.
 * @param channel LDC1612 has total two channels.
 * @param raw_result the raw data which read from data register,it contains error codes and sensor value;
 * */
s32 LDC1612::parse_result_data(u8 channel,u32 raw_result,u32* result)
{
    u8 value=0;
    *result=raw_result&0x0fffffff;
    if(0xfffffff==*result)
    {
        Serial.println("can't detect coil Coil Inductance!!!");
        *result=0;
        return -1;
    }
    // else if(0==*result)
    // {
    //     Serial.println("result is none!!!");
    // }
    value=raw_result>>24;
    if(value&0x80)
    {
        Serial.print("channel ");
        Serial.print(channel);
        Serial.println(": ERR_UR-Under range error!!!");
    }
    if(value&0x40)
    {
        Serial.print("channel ");
        Serial.print(channel);
        Serial.println(": ERR_OR-Over range error!!!");
    }
    if(value&0x20)
    {
        Serial.print("channel ");
        Serial.print(channel);
        Serial.println(": ERR_WD-Watch dog timeout error!!!");
    }
    if(value&0x10)
    {
        Serial.print("channel ");
        Serial.print(channel);
        Serial.println(": ERR_AE-error!!!");
    }
    return 0;
}


/**@brief read the raw channel result from register.
 * @param channel LDC1612 has total two channels.
 * @param result raw data
 * */
s32 LDC1612::get_channel_result(u8 channel,u32* result)
{
    u32 raw_value=0;
    if(NULL==result)
    {
        return -1;
    }
    u16 value=0;
    IIC_read_16bit(CONVERTION_RESULT_REG_START+channel*2,&value);
    raw_value|=(u32)value<<16;
    IIC_read_16bit(CONVERTION_RESULT_REG_START+channel*2+1,&value);
    raw_value|=(u32)value;
    parse_result_data(channel,raw_value,result);
    return 0;
}

/**@brief set conversion interval time.
 * @param channel LDC1612 has total two channels.
 * @param result The value to be set.
 * */
s32 LDC1612::set_conversion_time(u8 channel,u16 value)
{
    return IIC_write_16bit(SET_CONVERSION_TIME_REG_START+channel,value);
}


/**@brief set conversion offset.
 * @param channel LDC1612 has total two channels.
 * @param result The value to be set.
 * */
s32 LDC1612::set_conversion_offset(u8 channel,u16 value)
{
    return IIC_write_16bit(SET_CONVERSION_OFFSET_REG_START+channel,value);
}

/**@brief Before conversion,wait LC sensor stabilize for a short time.
 * @param channel LDC1612 has total two channels.
 * @param result The value to be set.
 * */
s32 LDC1612::set_LC_stabilize_time(u8 channel)
{
    u16 value=0;
    value=30;
    return IIC_write_16bit(SET_LC_STABILIZE_REG_START+channel,value);
}


/**@brief set input frequency divide and fref divide.
 * @param channel LDC1612 has total two channels.
 * @param FIN_DIV FIN input divide
 * @param FREF_DIV fref,reference frequency of sensor.
 * */
s32 LDC1612::set_FIN_FREF_DIV(u8 channel)
{
    u16 value;
    u16 FIN_DIV,FREF_DIV;

    Fsensor[channel]=1/(2*3.14*sqrt(inductance[channel]*capacitance[channel]*pow(10,-18)))*pow(10,-6);
    Serial.print("fsensor =");
    Serial.println(Fsensor[channel]);


    FIN_DIV=(u16)(Fsensor[channel]/8.75+1);


    if(Fsensor[channel]*4<40)
    {
        FREF_DIV=2;
        Fref[channel]=40/2;
    }
    else
    {
        FREF_DIV=4;
        Fref[channel]=40/4;
    }

    value=FIN_DIV<<12;
    value|=FREF_DIV;
    return IIC_write_16bit(SET_FREQ_REG_START+channel,value);
}


/**@brief Error output config.
 * @param result The value to be set.
 * */
s32 LDC1612::set_ERROR_CONFIG(u16 value)
{
    return IIC_write_16bit(ERROR_CONFIG_REG,value);
}



/**@brief mux  config.
 * @param result The value to be set.
 * */
s32 LDC1612::set_mux_config(u16 value)
{
    return IIC_write_16bit(MUL_CONFIG_REG,value);
}

/**@brief reset sensor.
 * 
 * */
s32 LDC1612::reset_sensor()
{
    return IIC_write_16bit(SENSOR_RESET_REG,0x8000);
}

/**@brief set drive current of sensor.
 * @param result The value to be set.
 * */
s32 LDC1612::set_driver_current(u8 channel,u16 value)
{
    return IIC_write_16bit(SET_DRIVER_CURRENT_REG+channel,value);
}


/**@brief Main config part of sensor.Contains select channel、start conversion、sleep mode、sensor activation mode、INT pin disable ..
 * @param result The value to be set.
 * */                                                                                                                                                                  
s32 LDC1612::set_sensor_config(u16 value)
{
    return IIC_write_16bit(SENSOR_CONFIG_REG,value);
}


/**@brief select channel to convert
 * 
 * */
void LDC1612::select_channel_to_convert(u8 channel,u16 *value)
{
    switch(channel)
    {
        case 0:*value &=0x3fff;
        break;
        case 1:*value &=0x7fff;
                *value |=0x4000;
        break;
        case 2:*value &=0xbfff;
                *value |=0x8000;
        break;
        case 3:*value |=0xc000;
        break;
    }   
}


void LDC1612::set_Rp(u8 channel,float n_kom)
{
    resistance[channel]=n_kom;
}

void LDC1612::set_L(u8 channel,float n_uh)
{
    inductance[channel]=n_uh;
}

void LDC1612::set_C(u8 channel,float n_pf)
{
    capacitance[channel]=n_pf;
}


void LDC1612::set_Q_factor(u8 channel,float q)
{
    Q_factor[channel]=q;
}


const char *status_str[]={"conversion under range error","conversion over range error",
                            "watch dog timeout error","Amplitude High Error",
                            "Amplitude Low Error","Zero Count Error",
                            "Data Ready","unread conversion is present for channel 0",
                            " unread conversion is present for Channel 1.",
                            "unread conversion ispresent for Channel 2.",
                            "unread conversion is present for Channel 3."};


/**@brief parse sensor statu data.
 * 
 * */
s32 LDC1612::sensor_status_parse(u16 value)
{
    u16 section=0;
    section=value>>14;
    switch(section)
    {
        case 0:Serial.println("Channel 0 is source of flag or error.");
        break;
        case 1:Serial.println("Channel 1 is source of flag or error.");
        break;
        /*Only support LDC1614*/
        case 2:Serial.println("Channel 2 is source of flag or error.");
        break;
        case 3:Serial.println("Channel 3 is source of flag or error.");
        break;
        default:
        break;
    }
    for(u32 i=0;i<6;i++)
    {
        if(value & (u16)1<<(8+i) )
        {
            Serial.println(status_str[6-i]);
        }
    }
    if(value&(1<<6))
    {
        Serial.println(status_str[6]);
    }
    for(u32 i=0;i<4;i++)
    {
        if(value & (1<<i) )
        {
            Serial.println(status_str[10-i]);
        }
    }
}

/**@brief get sensor status
 * 
 * */
u32 LDC1612::get_sensor_status()
{
    u16 value=0;
    IIC_read_16bit(SENSOR_STATUS_REG,&value);
    
    // Serial.print("status =");
    // Serial.println(value,HEX);

    sensor_status_parse(value);
    return value;
}



/**********************************************************************************************************/
/************************************************IIC PART************************************************/
/**********************************************************************************************************/

s32 LDC1612_IIC_OPRTS::IIC_write_byte(u8 reg,u8 byte)
{
    Wire.beginTransmission(_IIC_ADDR);
    Wire.write(reg);
    Wire.write(byte);
    return Wire.endTransmission();
}


s32 LDC1612_IIC_OPRTS::IIC_write_16bit(u8 reg,u16 value)
{
    Wire.beginTransmission(_IIC_ADDR);
    Wire.write(reg);
    
    Wire.write((u8)(value>>8));
    Wire.write((u8)value);
    return Wire.endTransmission();
}




void LDC1612_IIC_OPRTS::IIC_read_byte(u8 reg,u8* byte)
{
    Wire.beginTransmission(_IIC_ADDR);
    Wire.write(reg);
    Wire.endTransmission(false);

    Wire.requestFrom(_IIC_ADDR,(u8)1);
    while(1!=Wire.available());
    *byte=Wire.read();
    
}

void LDC1612_IIC_OPRTS::IIC_read_16bit(u8 start_reg,u16 *value)
{
    u8 val=0;
    *value=0;
    Wire.beginTransmission(_IIC_ADDR);
    Wire.write(start_reg);
    Wire.endTransmission(false);

    Wire.requestFrom(_IIC_ADDR,sizeof(u16));
    while(sizeof(u16)!=Wire.available());
    val=Wire.read();
    *value|=(u16)val<<8;
    val=Wire.read();
    *value|=val;
}



void LDC1612_IIC_OPRTS::set_iic_addr(u8 IIC_ADDR)
{
    _IIC_ADDR=IIC_ADDR;
}