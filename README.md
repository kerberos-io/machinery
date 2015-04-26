#KERBEROS.**IO**


[![Build Status](https://travis-ci.org/kerberos-io/machinery.svg)](https://travis-ci.org/kerberos-io/machinery) [![Stories in Ready](https://badge.waffle.io/kerberos-io/machinery.svg?label=ready&title=Ready)](http://waffle.io/kerberos-io/machinery)

## Introduction

[Kerberos](http://kerberos.io) is a low-budget surveillance solution created for the Rapsberry Pi, but it also works on OSX and Linux. It uses a motion detection algorithm to detect changes and stores images when motion is detected. Kerberos is open source, so you and others, can customize the source code to your needs and share it. It has a low-energy footprint when deploying on the Raspberry Pi and it's easy to install, you only need to transfer the image to the SD card and you're done.

Use your mobile phone, tablet or PC to keep an eye on your property. View the images taken by [Kerberos](http://kerberos.io) with our responsive and user-friendly web interface. Look at the dashboard to get a graphical overview of the past days. Multiple [Kerberos](http://kerberos.io) instances can be installed and can be viewed with only 1 web interface.

## Machinery

The machinery, is a motion detection framework, that takes images from your USB-camera or PI cameraboard and makes some calculations to recognize changes/motion. When motion is detected several IO operations can be executed (save an image to disk, store information to a DB, trigger an GPIO pin, etc). This project is built for the Open Source community, thus maintenance and expansion was/is very important. Therefore the machinery is divided in four logical parts; more information can be found on the [documentation website](http://doc.kerberos.io).

Developers can easily add new:

- Algorithms to detect motion, by using the OpenCV library.
- Create new controllers/heuristics to validate if detection was real.
- Integrate support for other output devices (a NOSQL database, E-mail, GPIO, TCP server and other notification services)

##Installation

The machinery is mainly built for the Raspberry PI, but it also works on OSX and Linux. It should also work on Windows with little modifications to the source code and cmake files.

To built the source, you will need to have a C++ compiler and cmake installed and that's it..


    git clone https://github.com/kerberos-io/machinery kerberos-io
    cd kerberos-io
    mkdir build && cd build && cmake .. && make && make check


This will download all the dependencies and link the executables. Notice that this can take some time, on tavis-ci it takes about 5 min.

##Contribute

Want to contribute? You're a Ph.D. in Computer Vision, or an ambitious programmer who wants to take kerberos.io to the next level? Then we like to welcome you to the community. Contributions are taken very seriously, besides your code, testing and documentation is very very ... very important! We only will accept pull-request with tests and documentation of a decent level and of course if everything works as expected. 
