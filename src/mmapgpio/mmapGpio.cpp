#include "mmapGpio.h"

// GPIO Registers that can be accessed as a part of the class
const unsigned int mmapGpio::GPFSET0;
const unsigned int mmapGpio::GPFCLR0;
const unsigned int mmapGpio::GPFLEV0;
const unsigned int mmapGpio::GPFSEL0;
const unsigned int mmapGpio::GPFSEL1;
const unsigned int mmapGpio::GPFSEL2;
const unsigned int mmapGpio::GPFSEL3;

//defines the two possible GPIO directions
const unsigned int mmapGpio::INPUT;
const unsigned int mmapGpio::OUTPUT;

//defines the two possible states of output/input pins
const unsigned int mmapGpio::LOW;
const unsigned int mmapGpio::HIGH;

//private constants 
const unsigned int mmapGpio::GPIO_BASE;
const unsigned int mmapGpio::GPIO_LEN;

/*******************************************************************
 * Default constructor....
 * Simply calls mapRegAddri() function to map the physical addresses
 * of the GPIO registers into local process memory
 * 
 * Parameters - None
 * Return Value - None
 *******************************************************************/
mmapGpio::mmapGpio(){
	this->gpio = mapRegAddr(GPIO_BASE);
}

/*******************************************************************
 * Destructor - unmaps GPIO registers (physical  memory)  from 
 * process memoy
 * 
 * Parameters - None
 * Return Value - None
 ******************************************************************/
mmapGpio::~mmapGpio(){
	
	//unmap GPIO registers (physicalmemory)  from process memoy
	if(munmap((void*)gpio, GPIO_LEN) < 0){
		perror("munmap (gpio) failed");
		exit(1);
	}
	
}

/*******************************************************************
 * writeGPIOReg() - Writes a 32-bit value to one of the GPIO 
 * addresses listed on lines 4-9. This function is not required for 
 * basic GPIO usage (low level access function) but is made available
 * anyways. 
 *
 * Parameters reg - Register address to write to.....see lines 4-9
 *            val - unsigned 32-bit value to write to the reg
 * Return Value - none
 * ****************************************************************/
void mmapGpio::writeGPIOReg(unsigned int reg, unsigned int val){
	*(this->gpio + reg) = val;
}

/*******************************************************************
 * readGPIOReg() - reads a 32-bit value from one of the GPIO 
 * addresses listed on lines 4-9. This function is not required for 
 * basic GPIO usage (low level access function) but is made available
 * anyways. 
 * 
 * Parameters reg - Register address to read from.....see lines 4-9
 *            val - Value of reg is written to val and passed back to
 *                  calling function/method by reference
 * Return Value - none
 * ****************************************************************/
void mmapGpio::readGPIOReg(unsigned int reg, unsigned int &val){
	val = *(this->gpio + reg);
	}

/*******************************************************************
 * setPinDir() - sets the direction of a pin to either input or 
 * output
 * 
 * Parameters - pinnum - GPIO pin number as per the RPI's  BCM2835's
 *                       standard definition
 *              dir - pin direction can be mmapGpio::INPUT for input
 *                    or mmapGpio::OUTPUT for output
 * Return Value -None
 * *****************************************************************/
void mmapGpio::setPinDir(unsigned int pinnum, const unsigned int &dir){
	if (dir == OUTPUT){
		switch(pinnum/10) {	
			case 0:
				*(this->gpio + GPFSEL0) &= ~(7<<(((pinnum)%10)*3));
				*(this->gpio + GPFSEL0) |=  (1<<(((pinnum)%10)*3));
				break;
			case 1:
				*(this->gpio + GPFSEL1) &= ~(7<<(((pinnum)%10)*3));
				*(this->gpio + GPFSEL1) |=  (1<<(((pinnum)%10)*3));
				break;
			case 2:
				*(this->gpio + GPFSEL2) &= ~(7<<(((pinnum)%10)*3));
				*(this->gpio + GPFSEL2) |=  (1<<(((pinnum)%10)*3));
				break;
			case 3:
				*(this->gpio + GPFSEL3) &= ~(7<<(((pinnum)%10)*3));
				*(this->gpio + GPFSEL3) |=  (1<<(((pinnum)%10)*3));
				break;
			default:
				break;
		}
	
	}
	else{
		switch(pinnum/10) {	
			case 0:
				*(this->gpio + GPFSEL0) &= ~(7<<(((pinnum)%10)*3));
				break;
			case 1:
				*(this->gpio + GPFSEL1) &= ~(7<<(((pinnum)%10)*3));
				break;
			case 2:
				*(this->gpio + GPFSEL2) &= ~(7<<(((pinnum)%10)*3));
				break;
			case 3:
				*(this->gpio + GPFSEL3) &= ~(7<<(((pinnum)%10)*3));
				break;
			default:
				break;
		}
	}
}

/*******************************************************************
 * readPin() - reads the state of a GPIO pin and returns its value
 * 
 * Parameter - pinnum - the pin number of the GPIO to read
 *
 * return Value - pin value. Either 1 (mmapGpio::HIGH) if pin state 
 *                is high or 0 (mmapGpio::LOW) if pin is low
 * ****************************************************************/
unsigned int mmapGpio::readPin(unsigned int pinnum){
	unsigned int retVal = 0;
	
	
	if ((*(this->gpio + GPFLEV0) & (1 << pinnum)) != 0)
		retVal = 1;
	
	return retVal;
}
	
/*******************************************************************
 * writePinState() - sets (to 1) or clears (to 0) the state of an
 * output GPIO. This function has no effect on input GPIOs.
 * For faster output GPIO pin setting/clearing..use inline functions
 * 'writePinHigh()' & 'writePinLow()' defined in the header file 
 * 
 * Parameters - pinnum - GPIO number as per RPI and BCM2835 
 *                       standard definition
 *              pinstate - value to write to output pin...either 
 *              mmapGpio::HIGH for setting or mmapGpio::LOW for 
 *              clearing
 * Return Value - None           
 * ****************************************************************/
void mmapGpio::writePinState(unsigned int pinnum, const unsigned int &pinstate){

	if(pinstate == HIGH)
		*(this->gpio + GPFSET0) = (1 << pinnum) ;
	else 	
		*(this->gpio + GPFCLR0) = (1 << pinnum);
}
/********************************************************************
 *	volatile unsigned *mmapGpio::mapRegAddr(unsigned long baseAddr)
 * This function maps a block of physical memory into the memory of 
 * the calling process. It enables a user space process to access 
 * registers in physical memory directly without having to interact 
 * with in kernel side code i.e. device drivers
 *
 * Parameter - baseAddr (unsigned long) - this is the base address of
 * a block of physical memory that will be mapped into the userspace 
 * process memory. 
 *******************************************************************/ 
volatile unsigned *mmapGpio::mapRegAddr(unsigned long baseAddr){
  int mem_fd = 0;
  void *regAddrMap = MAP_FAILED;

  /* open /dev/mem.....need to run program as root i.e. use sudo or su */
  if (!mem_fd) {
    if ((mem_fd = open("/dev/mem", O_RDWR|O_SYNC) ) < 0) {
     perror("can't open /dev/mem");
      exit (1);
    }
  }
  
   /* mmap IO */
  regAddrMap = mmap(
      NULL,             //Any adddress in our space will do
      GPIO_LEN,       //Map length
      PROT_READ|PROT_WRITE|PROT_EXEC,// Enable reading & writting to mapped memory
      MAP_SHARED|MAP_LOCKED,       //Shared with other processes
      mem_fd,           //File to map
      baseAddr         //Offset to base address
  );
    
  if (regAddrMap == MAP_FAILED) {
	  perror("mmap error");
	  close(mem_fd);
	  exit (1);
  }
  
  if(close(mem_fd) < 0){ //No need to keep mem_fd open after mmap
                         //i.e. we can close /dev/mem
	perror("couldn't close /dev/mem file descriptor");
    exit(1);
	}	
  return (volatile unsigned *)regAddrMap;
}