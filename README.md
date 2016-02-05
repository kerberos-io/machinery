#KERBEROS.**IO**

[![Build Status](https://travis-ci.org/kerberos-io/machinery.svg)](https://travis-ci.org/kerberos-io/machinery) [![Stories in Ready](https://badge.waffle.io/kerberos-io/machinery.svg?label=ready&title=Ready)](http://waffle.io/kerberos-io/machinery)  [![Join the chat at https://gitter.im/kerberos-io/hades](https://img.shields.io/badge/GITTER-join chat-green.svg)](https://gitter.im/kerberos-io/hades?utm_source=badge&utm_medium=badge&utm_campaign=pr-badge&utm_content=badge)

## Introduction

[Kerberos](http://kerberos.io) is a low-budget surveillance solution created for the Rapsberry Pi, but it also works on OSX and Linux. It uses computer vision algorithms to detect changes and stores images when motion is detected. Kerberos is open source, so you and others, can customize the source code to your needs and share it. It has a low-energy footprint when deploying on the Raspberry Pi and it's easy to install, you only need to transfer the image to the SD card and you're done.

Use your mobile phone, tablet or PC to keep an eye on your property. View the images taken by [Kerberos](http://kerberos.io) with our responsive and user-friendly web interface. Look at the dashboard to get a graphical overview of the past days. Multiple [Kerberos](http://kerberos.io) instances can be installed and can be viewed with only 1 web interface.

## Machinery

The machinery, is a computer vision framework, that takes images from your USB camera, IP camera or PI cameraboard and makes some calculations to recognize changes/motion. When motion is detected several IO operations can be executed (save an image to disk, store information to a DB, trigger an GPIO pin, etc). The machinery is divided in four logical parts; more information can be found on the [documentation website](http://doc.kerberos.io).

Developers can easily add new:

- Algorithms to detect motion, by using the OpenCV library.
- Create new heuristics to validate if detection was real.
- Integrate support for other output devices (a NOSQL database, E-mail, GPIO, TCP server and other notification services)

##Install

The machinery is mainly built for the Raspberry PI, but it also works on OSX and Linux. It should also work on Windows with little modifications to the source code and cmake files.

To built the source, you will need to have a C++ compiler and cmake installed and that's it..


    git clone https://github.com/kerberos-io/machinery kerberos-io
    cd kerberos-io
    mkdir build && cd build && cmake .. && make && make check && sudo make install

This will download all the dependencies and link the executables. Notice that this can take some time, on travis-ci it takes about 5 min.
    
##Configure

The configuration files can be found at **/etc/opt/kerberosio/config**. By default the Raspberry Pi Camera module is set as capture device. You can update the **config.xml** file to change it to **USBCamera** or **IPCamera**. Images are stored in the **/srv/capture** directory; this location can be changed by editing the **io.xml** file.

##Run

After kerberos is installed a binary is available at **/usr/bin/kerberosio**. Just run following command in your terminal to start kerberosio.

    kerberosio
    
##Contribute

Want to contribute? You're a Ph.D. in Computer Vision, or an ambitious programmer who wants to take kerberos.io to the next level? Then we like to welcome you to the community. Contributions are taken very seriously, besides your code, testing and documentation is very very ... very important! We only will accept pull-request with tests and documentation of a decent level and of course if everything works as expected. 
