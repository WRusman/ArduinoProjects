Seeed Inductive Sensor LDC1612
=====================================  


Introduction:  
------------------------  
The LDC1612 is 2 channel,28-bit inductance to digital converters (LDCs) for inductive sensing solutions. With multiple channels and support for remote sensing,the LDC1612 enable the performance and reliability benefits of inductive sensing to be realized at minimal cost and power.  
***



Usage:
==========
Download all the source files.
There are three examples for user to use.
>* **examples/basic_demo/basic_demo.ino**,this example shows a demo for single channel measurement.
>* **examples/muti_channel_demo/muti_channel_demo.ino**,this example shows multiple channels measurement.LDC1612 support two channels.
>* **examples/basic_demo/coin_test_demo.ino**,this example shows a demo for the sensor detects the distance of the coin. Note: the sensor is suspended during testing.


Notice:
============
>1. Set the Rb(resistance unit-kom),L(inductance unit-uH),C(capatance unit-pf) by the APIs if you don't use the Coil Inductance we provided,Used to calculate and set the corresponding frequency register of the sensor.
>2. There is a INT-pad on the sensor board,It turns to low level when the conversion is complete,overwise stays high,but you should configure the corresponding register of the sensor. 


Reference:
===============
Refer to the **/doc** to get more detail.
This is just a simple and universal template.

***
This software is written by downey  for seeed studio<br>
Email:dao.huang@seeed.cc
and is licensed under [The MIT License](http://opensource.org/licenses/mit-license.php). Check License.txt for more information.<br>

Contributing to this software is warmly welcomed. You can do this basically by<br>
[forking](https://help.github.com/articles/fork-a-repo), committing modifications and then [pulling requests](https://help.github.com/articles/using-pull-requests) (follow the links above<br>
for operating guide). Adding change log and your contact into file header is encouraged.<br>
Thanks for your contribution.

Seeed Studio is an open hardware facilitation company based in Shenzhen, China. <br>
Benefiting from local manufacture power and convenient global logistic system, <br>
we integrate resources to serve new era of innovation. Seeed also works with <br>
global distributors and partners to push open hardware movement.<br>