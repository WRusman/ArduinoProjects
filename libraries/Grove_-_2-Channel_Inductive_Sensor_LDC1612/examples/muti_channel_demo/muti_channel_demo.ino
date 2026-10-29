/*
 * basic_demo.ino
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

LDC1612 sensor;

void setup()
{
    Serial.begin(115200);
    delay(100);
    Serial.println("start!");

    sensor.init();

    /*Enable INT-PIN,Interrupted after measurement is completed.*/
    //sensor.set_ERROR_CONFIG(0x01);

    /*multiple channel use case configuration.*/
    if(sensor.LDC1612_mutiple_channel_config())
    {
        Serial.println("can't detect sensor!");
        while(1);
    }
}


void loop()
{
    u32 result_channel0=0;
    u32 result_channel1=0;

    /*shows the status of sensor.*/
    //sensor.get_sensor_status();

    /*sensor result value.you can make a lot of application according to its changes.*/
    sensor.get_channel_result(0,&result_channel0);
    sensor.get_channel_result(1,&result_channel1);

    if(0!=result_channel0)
    {
        Serial.print("result_channel0 is ");
        Serial.println(result_channel0);
    }
    if(0!=result_channel1)
    {
        Serial.print("result_channel1 is ");
        Serial.println(result_channel1);
    }
    delay(1000);
}





