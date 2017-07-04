#ifndef MMAPGPIO_H
#define MMAPGPIO_H

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

// uncomment to compile for Raspberry Pi 2
#define RASPBERRYPI2

/***********************************************************************
 * Author: Hussam Al-Hertani (Hertaville.com)
 * Others are free to modify and use this code as they see fit so long as they
 * give credit to the author.
 * 
 * The author is not liable for any kind of damage caused by this software. 
 * 
 * Acknowledgements: This 'C++' class is based on 'C' code available from :
 * - code from http://elinux.org/RPi_Low-level_peripherals 
 * - code from http://www.raspberrypi.org/forums/viewtopic.php?f=42&t=75048
 * - Gertboard's C source code 
 * 
 *
 * The mmapGpio Class is able to control the direction and state of all GPIO pins
 * on the RPI's 26-pin header using the 'mmaping into /dev/mem' approach. The advantage of this approach is 
 * that GPIO toggling speeds of uptp 21MHz can be achieved whereas the toggling gpio
 * speed using the sysfs interface is in the 100s of KHz.
 *
 * This class is not comprehensive as it does not enable PAD manipulation, Pull-up control
 * or interrupts,but it is adequate for basic GPIO use
 *    
 * *********************************************************************/

class mmapGpio{

public:
	mmapGpio();// default constructor. Mmaps into /dev/mem'
	~mmapGpio();// removes mapping between process memory
                // & physical memory
	void writeGPIOReg( unsigned int reg,  unsigned int val);
	void readGPIOReg(unsigned int reg, unsigned int &val);
    // The two methods above are able to read from and write to
    // the GPIO registers listed on lines 54-59 
	
    // sets  GPIO direction
    void setPinDir(unsigned int pinnum, const unsigned int &dir);
	
    //reads the value of a pin
    unsigned int readPin(unsigned int pinnum);
	
    //writes the state of output pins to either high or low.
    //For maximum speed use the two functions 'writePinHigh' 
    //or 'writePinLow' to perform the same task
    void writePinState(unsigned int pinnum, const unsigned int &pinstate);
	void inline writePinHigh(unsigned int pinnum){*(this->gpio + GPFSET0) = (1 << pinnum);}
	void inline writePinLow(unsigned int pinnum){*(this->gpio + GPFCLR0) = (1 << pinnum);}

    //gpio registers
	static const unsigned int GPFSET0 = 7;
	static const unsigned int GPFCLR0 = 10;
	static const unsigned int GPFLEV0 = 13;
	static const unsigned int GPFSEL0 = 0;
	static const unsigned int GPFSEL1 = 1;
	static const unsigned int GPFSEL2 = 2;
	static const unsigned int GPFSEL3 = 3;
	// two possible states for pin direction
    static const unsigned int INPUT = 0;
	static const unsigned int OUTPUT = 1;
	// two possible states for output pins
    static const unsigned int LOW = 0;
	static const unsigned int HIGH = 1;

private:
#ifdef RASPBERRYPI2
	static const unsigned int GPIO_BASE = 0x3f200000;// gpio registers base address PI2
#else
	static const unsigned int GPIO_BASE = 0x20200000;// gpio registers base address PI1
#endif
	
	static const unsigned int GPIO_LEN =   0xB4;// need only map B4 registers

	volatile unsigned int *mapRegAddr(unsigned long baseAddr);//performs mmaping into '/dev/mem'

	volatile unsigned int *gpio; 
};

#endif
