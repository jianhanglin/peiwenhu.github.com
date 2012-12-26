---
layout: post
title: "Simple motion detection by background subtraction"
description: ""
category: projects
tags: FPGA Digilent Nexys Atlys Verilog
---
{% include JB/setup %}

This is a project that my team in EC551 _Advanced digital design with Verilog and FPGA_ has been doing. 

We kind of created the prototype of and due to time limit it was submitted as our final project in that class actually. 
But much more improvements are needed, so we are still planning to get it on a right and decent track.

So far what we have done can be watched here:

<object width="560" height="315"><param name="movie" value="http://www.youtube.com/v/eRzhTgve97k?hl=en_US&amp;version=3"></param><param name="allowFullScreen" value="true"></param><param name="allowscriptaccess" value="always"></param><embed src="http://www.youtube.com/v/eRzhTgve97k?hl=en_US&amp;version=3" type="application/x-shockwave-flash" width="560" height="315" allowscriptaccess="always" allowfullscreen="true"></embed></object>
  
  
#Introduction

In a nutshell, this project is for detecting moving objects in the camera view by a weighted average method, and is implemented on a FPGA board. 
The devices we were planning to use are:  

* Digilent Nexys 3 board (with Spartan-6 FPGA, VGA interface and the specific, strange VHDCI connector)
* VGA monitor
* Digilent Vmod Cam( connecting with the board through VHDCI)

And the final stuff we used are:  


* Digilent Atlys board(with Spartan-6 FPGA, HDMI interface and VHDCI connector)
* HDMI monitor
* Digilent Vmod Cam


The purpose for this post is mainly to put down the failure we'd been through so that they might be helpful. 
The implementation, which I'll not pay much attention here, is simple and will vary depending on different approaches or equipments. 

#Roadmap

* Make the specification on what we are going to do. 
* Divide the project into separate modules.
* Working on Camera to get input.
* Working on memory and video processing unit.

#Specification and module distribution

We planned to use the Nexys board, because we've only got this guy in our class. This is not so comfortable by any means. 
The block memory in the FPGA is merely 574 kb or so, making it hardly a good device to store any image. The formula we need is  

    background = (127 * background + new frame) / 128  

which means that we must store 1 entire frame at least, if calculated in a pixel fashion. 
That's ok if we downsample it or cut the resolution. This board has SRAM but we didn't use that.

Why we turned to Atlys board is that we failed to make the Vmod Cam work on this board.

#Working on Camera

At early phase when we were working hard like horses trying to get that camera to work, 
we already noticed that this board may not get alone well with the camera, despite they are from the same company.  

The company says on their website that the VHDCI interface on Nexys is typically used for output rather than streaming from the camera. 
The only suggestion they give is to change the board to Atlys, which also has a support code for the camera in VHDL.  

We went on working on it whatsoever since we didn't want to surrender without a fight and we didn't want to buy the Atlys board. But after several desperate days' attempts it still didn't stream data. 
The [control code we used](/assets/motion_detection/myiic.v) mainly consists of two state machines, one of which is to switch among __IDLE__, __POWER\_UP__, __CONFIGURATION__, and __STREAMING__, 
the other is a small one only used to transmit I2C control to write the registers in the camera, during the __CONFIGURATION__ stage.

The best result we got up to that point was, when we entered the __CONFIGURATION__ stage and started transmitting, the led light we set to indicate the pixel clk blinked for one or two times. 
That might indicate that the camera did respond to us but somehow it then stopped and the pixel clock, which should be consistently provided by the camera, never clocked.

So maybe it's true that this board is not the Mr.right. I don't know. Would be more than excited to hear any successful implementation.  

Then we bought the Atlys board anyways. And by adapting [the VHDL code](http://www.digilentinc.com/Products/Detail.cfm?NavPath=2,648,931&Prod=VMOD-CAM) from their website, 
finally the monitor displayed what was saw by the camera. Though, this board does not have a VGA interface so we have to use a HDMI monitor.

TBC..