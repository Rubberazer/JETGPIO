// In C: gcc -Wall -g -o gpio_test gpio_test.c
// run with:
//  sudo ./gpio_test
//
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <errno.h>
#include <unistd.h>
#include <sys/fcntl.h>
#include <sys/mman.h>

/* Tegra X1 SoC Technical Reference Manual, version 1.3
 *
 * See Chapter 9 "Multi-Purpose I/O Pins", section 9.13 "GPIO Registers"
 * (table 32: GPIO Register Address Map)
 *
 * The GPIO hardware shares PinMux with up to 4 Special Function I/O per
 * pin, and only one of those five functions (SFIO plus GPIO) can be routed to
 * a pin at a time, using the PixMux.
 *
 * In turn, the PinMux outputs signals to Pads using Pad Control Groups. Pad
 * control groups control things like "drive strength" and "slew rate," and
 * need to be reset after deep sleep. Also, different pads have different
 * voltage tolerance. Pads marked "CZ" can be configured to be 3.3V tolerant
 * and driving; and pads marked "DD" can be 3.3V tolerant when in open-drain
 * mode (only.)
 *
 * The CNF register selects GPIO or SFIO, so setting it to 1 forces the GPIO
 * function. This is convenient for those who have a different pinmux at boot.
 */

#define GPIO_1 0x6000d000 					// Base address for all the GPIOs
#define PINMUX_AUX 0x70003000 				// Base address for all the GPIOs PINMUX_AUX config
#define APB_MISC 0x70000000 				//Base address for PAD control registers
#define PINMUX_AUX_GPIO_PE6_0 0x70003248  	//Header pin 33, GPIO_PE6. Typical value for output: 0x00000008
#define GPIO_PE6_CFG 0x700009c8 			//Header pin 33, GPIO_PE6. Typical value for output: 0x01f1f000

//  layout based on the definitions above
//  Each GPIO controller has four ports, each port controls 8 pins, each
//  register is interleaved for the four ports, so
//  REGX: port0, port1, port2, port3
//  REGY: port0, port1, port2, port3

typedef struct {
    uint32_t CNF[4];
    uint32_t OE[4];
    uint32_t OUT[4];
    uint32_t IN[4];
    uint32_t INT_STA[4];
    uint32_t INT_ENB[4];
    uint32_t INT_LVL[4];
    uint32_t INT_CLR[4];
} GPIO_mem;

uint32_t PINMUX_AUX_SPI1_MOSI_0;

int main(void)
{
    //  read physical memory (needs root)
    int fd = open("/dev/mem", O_RDWR | O_SYNC);
    if (fd < 0) {
        perror("/dev/mem");
        fprintf(stderr, "please run this program as root (for example with sudo)\n");
        exit(1);
    }

    //  map a particular physical address into our address space
    int pagesize = sysconf(_SC_PAGESIZE);    //getpagesize();
    int pagemask = pagesize-1;
    
    //  This page will actually contain all the GPIO controllers, because they are co-located
    void *base = mmap(0, pagesize, PROT_READ | PROT_WRITE, MAP_SHARED, fd, (GPIO_1 & ~pagemask));
    if (base == NULL) {
        perror("mmap()");
        exit(1);
    }
    //  This page will contain all pin_mux registers
    void *base_mux = mmap(0, pagesize, PROT_READ | PROT_WRITE, MAP_SHARED, fd, (PINMUX_AUX_GPIO_PE6_0 & ~pagemask));
    if (base == NULL) {
        perror("mmap()");
        exit(1);
    }
	
	 //  This page will contain all cfg registers
    void *base_cfg = mmap(0, pagesize, PROT_READ | PROT_WRITE, MAP_SHARED, fd, (GPIO_PE6_CFG & ~pagemask));
    if (base == NULL) {
        perror("mmap()");
        exit(1);
    }
	
	
    //  set up a pointer for convenient access -- this pointer is to the selected GPIO controller
    GPIO_mem volatile *pin = (GPIO_mem volatile *)((char *)base + (GPIO_1 & pagemask));
    
    //  set up a pointer for convenient access -- this pointer is to the selected pin_mux register
    uint32_t volatile *pin_mux = (uint32_t volatile *)((char *)base_mux + (PINMUX_AUX_GPIO_PE6_0 & pagemask));
    
    //  set up a pointer for convenient access -- this pointer is to the selected pin_cfg register
    uint32_t volatile *pin_cfg = (uint32_t volatile *)((char *)base_cfg + (GPIO_PE6_CFG & pagemask));
    
    printf("pin: %p\n", pin);
    printf("base: %p\n", base);
    printf("base_mux: %p\n", base_mux);
    printf("pin_mux: %p\n", pin_mux);
    
     
    int offset = 0x100;
     
    pin = (GPIO_mem volatile *)((char *)pin + offset);
    
	printf("pin+offset:%p\n", pin->CNF);
	printf("*pin->CNF value:%x\n", pin->CNF[0]);
	printf("*pin.OE:%p\n", pin->OE);
	printf("*pin->OE value:%x\n", pin->OE[0]);
	printf("*pin.OUT:%p\n", pin->OUT);
	printf("*pin->OUT value:%x\n", pin->OUT[0]);
	
	*pin_mux = 0x00000008;
	*pin_cfg = 0x01F1F000;
	
    printf("NOW configure the CNF\n");
    
    pin->CNF[0] = 0x00000040;
    
    printf("NOW select output\n");
    pin->OE[0] = 0x00000040;
    
    int x =0;
    while (x<5) {
        pin->OUT[0] = 0x00000040;
        printf("HIGH\n");
        sleep(5);
        pin->OUT[0] = 0x00000000;
        printf("LOW\n");
        sleep(5);
		x++;
    }
    pin->OUT[0] = 0x00000000;
    *pin_mux = 0x00000014;
    *pin_cfg = 0x00000000;
    
	/* unmap */
    munmap(base, pagesize);

    /* close the /dev/mem */
    close(fd);
	
    return 0 ;
}
