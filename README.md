#KERBEROS.**IO**

[![Build Status](https://travis-ci.org/kerberos-io/machinery.svg)](https://travis-ci.org/kerberos-io/machinery) [![Stories in Ready](https://badge.waffle.io/kerberos-io/machinery.svg?label=ready&title=Ready)](http://waffle.io/kerberos-io/machinery)  [![Join the chat at https://gitter.im/kerberos-io/hades](https://img.shields.io/badge/GITTER-join chat-green.svg)](https://gitter.im/kerberos-io/hades?utm_source=badge&utm_medium=badge&utm_campaign=pr-badge&utm_content=badge)

[![Kerberos.io - video surveillance](https://kerberos.io/images/kerberos.png)](https://kerberos.io)

## Vote for features

[![Feature Requests](http://feathub.com/kerberos-io/machinery?format=svg)](http://feathub.com/kerberos-io/machinery)

#CC-NC-ND license

THE WORK (AS DEFINED BELOW) IS PROVIDED UNDER THE TERMS OF THIS CREATIVE COMMONS PUBLIC LICENSE ("CCPL" OR "LICENSE"). THE WORK IS PROTECTED BY COPYRIGHT AND/OR OTHER APPLICABLE LAW. ANY USE OF THE WORK OTHER THAN AS AUTHORIZED UNDER THIS LICENSE OR COPYRIGHT LAW IS PROHIBITED.

More information [**about this license**](https://doc.kerberos.io/license).

## Why Kerberos.io?

As burgalary is very common, we believe that video surveillance is a **trivial tool** in our daily lifes which helps us to **feel** a little bit more **secure**. Responding to this need, a lot of companies have started developing their own video surveillance software in the past few years.

Nowadays we have a myriad of **expensive** camera's, recorders and software solutions which are mainly **outdated** and **difficult** to install and use. Kerberos.io's goal is to solve these problems and to provide every human being in this world to have its own **ecological**, **affordable**, **easy-to-use** and **innovative** surveillance solution.

## Introduction

[Kerberos.io](http://kerberos.io) is a **low-budget** video surveillance solution, that uses computer vision algorithms to detect changes, and that can trigger other devices. [Kerberos.io](http://kerberos.io) is open source so everyone can customize the source code to its needs and share it with the community under the [**CC-NC-ND license model**](https://doc.kerberos.io/license). When deployed on the Raspberry Pi, it has a **green footprint** and it's **easy to install**; you only need to transfer the [Kerberos.io OS (KIOS)](https://doc.kerberos.io/2.0/installation/KiOS) to your SD card and that's it.

Use your mobile phone, tablet or PC to keep an eye on your property. View the images taken by [Kerberos.io](http://kerberos.io) with our responsive and user-friendly web interface. Look at the dashboard to get a graphical overview of the past days. Multiple [Kerberos.io](http://kerberos.io) instances can be installed and can be viewed with only 1 web interface.

## Machinery

The machinery is responsible for the processing. It's **an image processing framework** which takes images from the type of camera (USB-, IP- or RPi-camera) you've configured in the configuration files and executes one ore more algorithms and post-processes (e.g. save a snapshot). The configuration files allow you to define the type of camera, post-processes, conditions and much more; it's **highly configurable**. It's important to note that the machinery, out-of-the-box, can handle only one camera at a time; more information can be found on the [documentation website](http://doc.kerberos.io).

Developers can easily add new:

- Algorithms to detect motion, by using the OpenCV library.
- Create new heuristics to validate if detection was real.
- Integrate support for other output devices (a NOSQL database, E-mail, GPIO, TCP server and other notification services)

## How does it work?

The machinery is an **image processing framework** which is devided into four steps:

* condition
* algorithm
* expositor
* heuristic

The steps belong to a four passway; illustrated on the image below. In each cycle a sequence of images is processed. Each step will process the sequence, and will return some result to the next step. Please check out the [demo environment](https//doc.kerberos.io) if you want to see a real life example.

[Read more](https://doc.kerberos.io/2.0/machinery/introduction)

##Installation

The reason why you're reading this paragraph is because you want to know how to install the machinery on your Raspberry Pi, local working station, server or whatever machine you prefer. The good news is that we have **different approaches** from basic to advanced; it depends on how you want to install it.

###KiOS (for Raspberry Pi)

[KiOS](https://github.com/kerberos-io/kios) is a custom linux OS (created by buildroot) which runs Kerberos.io out-of-the- (it contains both the machinery and the web). KiOS is **installed like every other OS** for the Raspberry Pi, you need to flash the OS (.img) to a SD card, update your network configration and you're up and running; no manual compilation or horrible configurations. This is the **most simple** and **basic** installation procedure.

[Read more](https://doc.kerberos.io/2.0/installation/KiOS)

###Raspbian (for Raspberry Pi)

If you already have a Raspberry Pi running with Raspbian, you probably don't want to reflash your SD-card. Therefore you can install the different parts of Kerberos.io (the machinery and the web) manual.

[Read more](https://doc.kerberos.io/2.0/installation/Raspbian)

###Armbian (for Orange Pi, PCDuino, etc)

Kerberos.io can also be installed on other boards, which run the Armbian OS. 

[Read more](https://doc.kerberos.io/2.0/installation/Armbian)

###Generic

If you want to install the machinery on your working station, than you'll need to compile the source code yourself; it's a little bit more work.

Update the packages and kernel.

    sudo apt-get update && sudo apt-get upgrade

Install development tools (c++, cmake) and V4L utils.

    sudo apt-get install git libav-tools cmake subversion dh-autoreconf libcurl4-openssl-dev

Go to home directory and pull the machinery from github.

	cd && sudo git clone https://github.com/kerberos-io/machinery

Compile the machinery; this can take some time.

    cd machinery && mkdir build && cd build
    cmake .. && make && make check && sudo make install

Start the machinery on start-up, and reboot the system.

     sudo systemctl enable kerberosio
     sudo reboot

##Configure

The configuration files can be found at **/etc/opt/kerberosio/config**. By default the Raspberry Pi Camera module is set as capture device. You can update the **config.xml** file to change it to **USBCamera** or **IPCamera**. Images are stored in the **/etc/opt/kerberosio/capture** directory; this location can be changed by editing the **io.xml** file.

##Run

After kerberos is installed a binary is available at **/usr/bin/kerberosio**. Just run following command in your terminal to start kerberosio.

    kerberosio
    
##Contribute

Want to contribute? You're a Ph.D. in Computer Vision, or an ambitious programmer who wants to take kerberos.io to the next level? Then we like to welcome you to the community. Contributions are taken very seriously, besides your code, testing and documentation is very very ... very important! We only will accept pull-request with tests and documentation of a decent level and of course if everything works as expected. 
