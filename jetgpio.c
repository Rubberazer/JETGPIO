/*
This is free and unencumbered software released into the public domain.
Anyone is free to copy, modify, publish, use, compile, sell, or
distribute this software, either in source code form or as a compiled
binary, for any purpose, commercial or non-commercial, and by any
means.
In jurisdictions that recognize copyright laws, the author or authors
of this software dedicate any and all copyright interest in the
software to the public domain. We make this dedication for the benefit
of the public at large and to the detriment of our heirs and
successors. We intend this dedication to be an overt act of
relinquishment in perpetuity of all present and future rights to this
software under copyright law.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.
For more information, please refer to <http://unlicense.org/>
*/

/* jetgpio version 0.1 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <errno.h>
#include <unistd.h>
#include <sys/fcntl.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/file.h>
#include <sys/types.h>
#include <linux/i2c-dev.h>
#include <linux/i2c.h>
#include <linux/types.h>

#include "jetgpio.h"

static int fd_GPIO;

static volatile GPIO_CNF_Init pin_CNF;
static volatile GPIO_PINMUX_Init pin_MUX;
static volatile GPIO_CFG_Init pin_CFG;
static volatile GPIO_PWM pinPWM_Init;
static volatile GPIO_PWM *pinPWM;

static i2cInfo_t i2cInfo[2];
static int i2c_speed[2];

static volatile GPIO_CNF *pin3;
static volatile GPIO_CNF *pin5;
static volatile GPIO_CNF *pin7;
static volatile GPIO_CNF *pin8;
static volatile GPIO_CNF *pin10;
static volatile GPIO_CNF *pin11;
static volatile GPIO_CNF *pin12;
static volatile GPIO_CNF *pin13;
static volatile GPIO_CNF *pin15;
static volatile GPIO_CNF *pin16;
static volatile GPIO_CNF *pin18;
static volatile GPIO_CNF *pin19;
static volatile GPIO_CNF *pin21;
static volatile GPIO_CNF *pin22;
static volatile GPIO_CNF *pin23;
static volatile GPIO_CNF *pin24;
static volatile GPIO_CNF *pin26;
static volatile GPIO_CNF *pin27;
static volatile GPIO_CNF *pin28;
static volatile GPIO_CNF *pin29;
static volatile GPIO_CNF *pin31;
static volatile GPIO_CNF *pin32;
static volatile GPIO_CNF *pin33;
static volatile GPIO_CNF *pin35;
static volatile GPIO_CNF *pin36;
static volatile GPIO_CNF *pin37;
static volatile GPIO_CNF *pin38;
static volatile GPIO_CNF *pin40;

static volatile uint32_t *pinmux3;
static volatile uint32_t *pinmux5;
static volatile uint32_t *pinmux7;
static volatile uint32_t *pinmux8;
static volatile uint32_t *pinmux10;
static volatile uint32_t *pinmux11;
static volatile uint32_t *pinmux12;
static volatile uint32_t *pinmux13;
static volatile uint32_t *pinmux15;
static volatile uint32_t *pinmux16;
static volatile uint32_t *pinmux18;
static volatile uint32_t *pinmux19;
static volatile uint32_t *pinmux21;
static volatile uint32_t *pinmux22;
static volatile uint32_t *pinmux23;
static volatile uint32_t *pinmux24;
static volatile uint32_t *pinmux26;
static volatile uint32_t *pinmux27;
static volatile uint32_t *pinmux28;
static volatile uint32_t *pinmux29;
static volatile uint32_t *pinmux31;
static volatile uint32_t *pinmux32;
static volatile uint32_t *pinmux33;
static volatile uint32_t *pinmux35;
static volatile uint32_t *pinmux36;
static volatile uint32_t *pinmux37;
static volatile uint32_t *pinmux38;
static volatile uint32_t *pinmux40;

static volatile uint32_t *pincfg3;
static volatile uint32_t *pincfg5;
static volatile uint32_t *pincfg7;
static volatile uint32_t *pincfg8;
static volatile uint32_t *pincfg10;
static volatile uint32_t *pincfg11;
static volatile uint32_t *pincfg12;
static volatile uint32_t *pincfg13;
static volatile uint32_t *pincfg15;
static volatile uint32_t *pincfg16;
static volatile uint32_t *pincfg18;
static volatile uint32_t *pincfg19;
static volatile uint32_t *pincfg21;
static volatile uint32_t *pincfg22;
static volatile uint32_t *pincfg23;
static volatile uint32_t *pincfg24;
static volatile uint32_t *pincfg26;
static volatile uint32_t *pincfg27;
static volatile uint32_t *pincfg28;
static volatile uint32_t *pincfg29;
static volatile uint32_t *pincfg31;
static volatile uint32_t *pincfg32;
static volatile uint32_t *pincfg33;
static volatile uint32_t *pincfg35;
static volatile uint32_t *pincfg36;
static volatile uint32_t *pincfg37;
static volatile uint32_t *pincfg38;
static volatile uint32_t *pincfg40;

static void *baseCNF;

static void *basePINMUX;

static void *baseCFG;

static void *basePWM;


int gpioInitialise(void)
{
	int status = 1;
	//  Getting the page size
    int pagesize = sysconf(_SC_PAGESIZE);    //getpagesize();
    //int pagemask = pagesize-1;
	
	//  read physical memory (needs root)
	fd_GPIO = open("/dev/mem", O_RDWR | O_SYNC);
    if (fd_GPIO < 0) {
        perror("/dev/mem");
        fprintf(stderr, "please run this program as root (for example with sudo)\n");
        status = -1;
    }
    //  Mapping GPIO_CNF
    baseCNF = mmap(0, pagesize, PROT_READ | PROT_WRITE, MAP_SHARED, fd_GPIO, base_CNF);
    if (baseCNF == NULL) {
        perror("mmap()");
        status = -3;
    }
    
    //  Mapping GPIO_PINMUX
	basePINMUX = mmap(0, pagesize, PROT_READ | PROT_WRITE, MAP_SHARED, fd_GPIO, base_PINMUX);
    if (basePINMUX == NULL) {
        perror("mmap()");
        status = -3;
    }
    
        //  Mapping GPIO_CFG
	baseCFG = mmap(0, pagesize, PROT_READ | PROT_WRITE, MAP_SHARED, fd_GPIO, base_CFG);
    if (baseCFG == NULL) {
        perror("mmap()");
        status = -3;
    }
    
       //  Mapping GPIO_PWM
	basePWM = mmap(0, pagesize, PROT_READ | PROT_WRITE, MAP_SHARED, fd_GPIO, base_PWM);
    if (basePWM == NULL) {
        perror("mmap()");
        status = -3;
    }
    
     // Pointer to PM3_PWMx
    pinPWM = (GPIO_PWM volatile *)((char *)basePWM + PM3_PWM0);
    pinPWM_Init = *pinPWM;
    
    // Pointer to CNF3
    pin3 = (GPIO_CNF volatile *)((char *)baseCNF + CNF_3);
    pin_CNF.CNF3 = pin3->CNF[0];
    
    
    // Pointer to PINMUX3
    pinmux3 = (uint32_t volatile *)((char *)basePINMUX + PINMUX_3);
    pin_MUX.PINMUX3 = *pinmux3;
    

    // Pointer to PINCFG3
    pincfg3 = (uint32_t volatile *)((char *)baseCFG + CFG_3);
    pin_CFG.CFG3 = *pincfg3;
    
    // Pointer to CNF5
    pin5 = (GPIO_CNF volatile *)((char *)baseCNF + CNF_5);
    pin_CNF.CNF5 = pin5->CNF[0];
    
    // Pointer to PINMUX5
    pinmux5 = (uint32_t volatile *)((char *)basePINMUX + PINMUX_5);
    pin_MUX.PINMUX5 = *pinmux5;
    
    // Pointer to PINCFG5
    pincfg5 = (uint32_t volatile *)((char *)baseCFG + CFG_5);
    pin_CFG.CFG5 = *pincfg5;
    
    // Pointer to CNF7
    pin7 = (GPIO_CNF volatile *)((char *)baseCNF + CNF_7);
    pin_CNF.CNF7 = pin7->CNF[0];
    
    // Pointer to PINMUX7
    pinmux7 = (uint32_t volatile *)((char *)basePINMUX + PINMUX_7);
    pin_MUX.PINMUX7 = *pinmux7;
    
    // Pointer to PINCFG7
    pincfg7 = (uint32_t volatile *)((char *)baseCFG + CFG_7);
    pin_CFG.CFG7 = *pincfg7;
    
    // Pointer to CNF8
    pin8 = (GPIO_CNF volatile *)((char *)baseCNF + CNF_8);
    pin_CNF.CNF8 = pin8->CNF[0];
    
    // Pointer to PINMUX8
    pinmux8 = (uint32_t volatile *)((char *)basePINMUX + PINMUX_8);
    pin_MUX.PINMUX8 = *pinmux8;
    
    // Pointer to PINCFG8
    pincfg8 = (uint32_t volatile *)((char *)baseCFG + CFG_8);
    pin_CFG.CFG8 = *pincfg8;
    
    // Pointer to CNF10
    pin10 = (GPIO_CNF volatile *)((char *)baseCNF + CNF_10);
    pin_CNF.CNF10 = pin10->CNF[0];
    
    // Pointer to PINMUX10
    pinmux10 = (uint32_t volatile *)((char *)basePINMUX + PINMUX_10);
    pin_MUX.PINMUX10 = *pinmux10;
    
    // Pointer to PINCFG10
    pincfg10 = (uint32_t volatile *)((char *)baseCFG + CFG_10);
    pin_CFG.CFG10 = *pincfg10;
    
    // Pointer to CNF11
    pin11 = (GPIO_CNF volatile *)((char *)baseCNF + CNF_11);
    pin_CNF.CNF11 = pin11->CNF[0];
   
    // Pointer to PINMUX11
    pinmux11 = (uint32_t volatile *)((char *)basePINMUX + PINMUX_11);
    pin_MUX.PINMUX11 = *pinmux11;
    
    // Pointer to PINCFG11
    pincfg11 = (uint32_t volatile *)((char *)baseCFG + CFG_11);
    pin_CFG.CFG11 = *pincfg11;
    
    // Pointer to CNF12
    pin12 = (GPIO_CNF volatile *)((char *)baseCNF + CNF_12);
    pin_CNF.CNF12 = pin12->CNF[0];
    
    // Pointer to PINMUX12
    pinmux12 = (uint32_t volatile *)((char *)basePINMUX + PINMUX_12);
    pin_MUX.PINMUX12 = *pinmux12;
    
    // Pointer to PINCFG12
    pincfg12 = (uint32_t volatile *)((char *)baseCFG + CFG_12);
    pin_CFG.CFG12 = *pincfg12;
    
    // Pointer to CNF13
    pin13 = (GPIO_CNF volatile *)((char *)baseCNF + CNF_13);
    pin_CNF.CNF13 = pin13->CNF[0];
    
    // Pointer to PINMUX13
    pinmux13 = (uint32_t volatile *)((char *)basePINMUX + PINMUX_13);
    pin_MUX.PINMUX13 = *pinmux13;
    
    // Pointer to PINCFG13
    pincfg13 = (uint32_t volatile *)((char *)baseCFG + CFG_13);
    pin_CFG.CFG13 = *pincfg13;
    
    // Pointer to CNF15
    pin15 = (GPIO_CNF volatile *)((char *)baseCNF + CNF_15);
    pin_CNF.CNF15 = pin15->CNF[0];
    
    // Pointer to PINMUX15
    pinmux15 = (uint32_t volatile *)((char *)basePINMUX + PINMUX_15);
    pin_MUX.PINMUX15 = *pinmux15;
    
    // Pointer to PINCFG15
    pincfg15 = (uint32_t volatile *)((char *)baseCFG + CFG_15);
    pin_CFG.CFG15 = *pincfg15;
    
    // Pointer to CNF16
    pin16 = (GPIO_CNF volatile *)((char *)baseCNF + CNF_16);
    pin_CNF.CNF16 = pin16->CNF[0];
    
    // Pointer to PINMUX16
    pinmux16 = (uint32_t volatile *)((char *)basePINMUX + CNF_16);
    pin_MUX.PINMUX16 = *pinmux16;
   
    // Pointer to PINCFG16
    pincfg16 = (uint32_t volatile *)((char *)baseCFG + CFG_16);
    pin_CFG.CFG16 = *pincfg16;
    
    // Pointer to CNF18
    pin18 = (GPIO_CNF volatile *)((char *)baseCNF + CNF_18);
    pin_CNF.CNF18 = pin18->CNF[0];
    
    // Pointer to PINMUX18
    pinmux18 = (uint32_t volatile *)((char *)basePINMUX + PINMUX_18);
    pin_MUX.PINMUX18 = *pinmux18;
    
    // Pointer to PINCFG18
    pincfg18 = (uint32_t volatile *)((char *)baseCFG + CFG_18);
    pin_CFG.CFG18 = *pincfg18;
    
    // Pointer to CNF19
    pin19 = (GPIO_CNF volatile *)((char *)baseCNF + CNF_19);
    pin_CNF.CNF19 = pin19->CNF[0];
    
    // Pointer to PINMUX19
    pinmux19 = (uint32_t volatile *)((char *)basePINMUX + PINMUX_19);
    pin_MUX.PINMUX19 = *pinmux19;
    
    // Pointer to PINCFG19
    pincfg19 = (uint32_t volatile *)((char *)baseCFG + CFG_19);
    pin_CFG.CFG19 = *pincfg19;
    
    // Pointer to CNF21
    pin21 = (GPIO_CNF volatile *)((char *)baseCNF + CNF_21);
    pin_CNF.CNF21 = pin21->CNF[0];
    
    // Pointer to PINMUX21
    pinmux21 = (uint32_t volatile *)((char *)basePINMUX + PINMUX_21);
    pin_MUX.PINMUX21 = *pinmux21;
    
    // Pointer to PINCFG21
    pincfg21 = (uint32_t volatile *)((char *)baseCFG + CFG_21);
    pin_CFG.CFG21 = *pincfg21;
    
    // Pointer to CNF22
    pin22 = (GPIO_CNF volatile *)((char *)baseCNF + CNF_22);
    pin_CNF.CNF22 = pin22->CNF[0];
    
    // Pointer to PINMUX22
    pinmux22 = (uint32_t volatile *)((char *)basePINMUX + PINMUX_22);
    pin_MUX.PINMUX22 = *pinmux22;
    
    // Pointer to PINCFG22
    pincfg22 = (uint32_t volatile *)((char *)baseCFG + CFG_22);
    pin_CFG.CFG22 = *pincfg22;
    
    // Pointer to CNF23
    pin23 = (GPIO_CNF volatile *)((char *)baseCNF + CNF_23);
    pin_CNF.CNF23 = pin23->CNF[0];
    
    // Pointer to PINMUX23
    pinmux23 = (uint32_t volatile *)((char *)basePINMUX + PINMUX_23);
    pin_MUX.PINMUX23 = *pinmux23;
    
    // Pointer to PINCFG23
    pincfg23 = (uint32_t volatile *)((char *)baseCFG + CFG_23);
    pin_CFG.CFG23 = *pincfg23;
    
    // Pointer to CNF24
    pin24 = (GPIO_CNF volatile *)((char *)baseCNF + CNF_24);
    pin_CNF.CNF24 = pin24->CNF[0];
    
    // Pointer to PINMUX24
    pinmux24 = (uint32_t volatile *)((char *)basePINMUX + PINMUX_24);
    pin_MUX.PINMUX24 = *pinmux24;
    
    // Pointer to PINCFG24
    pincfg24 = (uint32_t volatile *)((char *)baseCFG + CFG_24);
    pin_CFG.CFG24 = *pincfg24;
    
    // Pointer to CNF26
    pin26 = (GPIO_CNF volatile *)((char *)baseCNF + CNF_26);
    pin_CNF.CNF26 = pin26->CNF[0];
    
    // Pointer to PINMUX26
    pinmux26 = (uint32_t volatile *)((char *)basePINMUX + PINMUX_26);
    pin_MUX.PINMUX26 = *pinmux26;
    
    // Pointer to PINCFG26
    pincfg26 = (uint32_t volatile *)((char *)baseCFG + CFG_26);
    pin_CFG.CFG26 = *pincfg26;
    
    // Pointer to CNF27
    pin27 = (GPIO_CNF volatile *)((char *)baseCNF + CNF_27);
    pin_CNF.CNF27 = pin27->CNF[0];
    
    // Pointer to PINMUX27
    pinmux27 = (uint32_t volatile *)((char *)basePINMUX + PINMUX_27);
    pin_MUX.PINMUX27 = *pinmux27;
    
    // Pointer to PINCFG27
    pincfg27 = (uint32_t volatile *)((char *)baseCFG + CFG_27);
    pin_CFG.CFG27 = *pincfg27;
    
    // Pointer to CNF28
    pin28 = (GPIO_CNF volatile *)((char *)baseCNF + CNF_28);
    pin_CNF.CNF28 = pin28->CNF[0];
    
    // Pointer to PINMUX28
    pinmux28 = (uint32_t volatile *)((char *)basePINMUX + PINMUX_28);
    pin_MUX.PINMUX28 = *pinmux28;
    
    // Pointer to PINCFG28
    pincfg28 = (uint32_t volatile *)((char *)baseCFG + CFG_28);
    pin_CFG.CFG28 = *pincfg28;
    
    // Pointer to CNF29
    pin29 = (GPIO_CNF volatile *)((char *)baseCNF + CNF_29);
    pin_CNF.CNF29 = pin29->CNF[0];
    
    // Pointer to PINMUX29
    pinmux29 = (uint32_t volatile *)((char *)basePINMUX + PINMUX_29);
    pin_MUX.PINMUX29 = *pinmux29;
    
    // Pointer to PINCFG29
    pincfg29 = (uint32_t volatile *)((char *)baseCFG + CFG_29);
    pin_CFG.CFG29 = *pincfg29;
    
    // Pointer to CNF31
    pin31 = (GPIO_CNF volatile *)((char *)baseCNF + CNF_31);
    pin_CNF.CNF31 = pin31->CNF[0];
    
    // Pointer to PINMUX31
    pinmux31 = (uint32_t volatile *)((char *)basePINMUX + PINMUX_31);
    pin_MUX.PINMUX31 = *pinmux31;
    
    // Pointer to PINCFG31
    pincfg31 = (uint32_t volatile *)((char *)baseCFG + CFG_31);
    pin_CFG.CFG31 = *pincfg31;
    
    // Pointer to CNF32
    pin32 = (GPIO_CNF volatile *)((char *)baseCNF + CNF_32);
    pin_CNF.CNF32 = pin32->CNF[0];
    
    // Pointer to PINMUX32
    pinmux32 = (uint32_t volatile *)((char *)basePINMUX + PINMUX_32);
    pin_MUX.PINMUX32 = *pinmux32;
   
    // Pointer to PINCFG32
    pincfg32 = (uint32_t volatile *)((char *)baseCFG + CFG_32);
    pin_CFG.CFG32 = *pincfg32;
    
    // Pointer to CNF33
    pin33 = (GPIO_CNF volatile *)((char *)baseCNF + CNF_33);
    pin_CNF.CNF33 = pin33->CNF[0];
    
    // Pointer to PINMUX33
    pinmux33 = (uint32_t volatile *)((char *)basePINMUX + PINMUX_33);
    pin_MUX.PINMUX33 = *pinmux33;
    
    // Pointer to PINCFG33
    pincfg33 = (uint32_t volatile *)((char *)baseCFG + CFG_33);
    pin_CFG.CFG33 = *pincfg33;
    
    // Pointer to CNF35
    pin35 = (GPIO_CNF volatile *)((char *)baseCNF + CNF_35);
    pin_CNF.CNF35 = pin35->CNF[0];
    
    // Pointer to PINMUX35
    pinmux35 = (uint32_t volatile *)((char *)basePINMUX + PINMUX_35);
    pin_MUX.PINMUX35 = *pinmux35;
    
    // Pointer to PINCFG35
    pincfg35 = (uint32_t volatile *)((char *)baseCFG + CFG_35);
    pin_CFG.CFG35 = *pincfg35;
    
    // Pointer to CNF36
    pin36 = (GPIO_CNF volatile *)((char *)baseCNF + CNF_36);
    pin_CNF.CNF36 = pin36->CNF[0];
    
    // Pointer to PINMUX36
    pinmux36 = (uint32_t volatile *)((char *)basePINMUX + PINMUX_36);
    pin_MUX.PINMUX36 = *pinmux36;
    
    // Pointer to PINCFG36
    pincfg36 = (uint32_t volatile *)((char *)baseCFG + CFG_36);
    pin_CFG.CFG36 = *pincfg36;
    
    // Pointer to CNF37
    pin37 = (GPIO_CNF volatile *)((char *)baseCNF + CNF_37);
    pin_CNF.CNF37 = pin37->CNF[0];
    
    // Pointer to PINMUX37
    pinmux37 = (uint32_t volatile *)((char *)basePINMUX + PINMUX_37);
    pin_MUX.PINMUX37 = *pinmux37;
    
    // Pointer to PINCFG37
    pincfg37 = (uint32_t volatile *)((char *)baseCFG + CFG_37);
    pin_CFG.CFG37 = *pincfg37;
    
    // Pointer to CNF38
    pin38 = (GPIO_CNF volatile *)((char *)baseCNF + CNF_38);
    pin_CNF.CNF38 = pin38->CNF[0];
    
    // Pointer to PINMUX38
    pinmux38 = (uint32_t volatile *)((char *)basePINMUX + PINMUX_38);
    pin_MUX.PINMUX38 = *pinmux38;
    
    // Pointer to PINCFG38
    pincfg38 = (uint32_t volatile *)((char *)baseCFG + CFG_38);
    pin_CFG.CFG38 = *pincfg38;
   
    // Pointer to CNF40
    pin40 = (GPIO_CNF volatile *)((char *)baseCNF + CNF_40);
    pin_CNF.CNF40 = pin40->CNF[0];
    
    // Pointer to PINMUX40
    pinmux40 = (uint32_t volatile *)((char *)basePINMUX + PINMUX_40);
    pin_MUX.PINMUX40 = *pinmux40;
    
    // Pointer to PINCFG40
    pincfg40 = (uint32_t volatile *)((char *)baseCFG + CFG_40);
    pin_CFG.CFG40 = *pincfg40;

	return status;
}

void gpioTerminate(void)
{
	int pagesize = sysconf(_SC_PAGESIZE);
	/* Restoring registers to their previous state */
	*pinPWM = pinPWM_Init;
	pin3->CNF[0] = pin_CNF.CNF3;
	*pinmux3 = pin_MUX.PINMUX3;
	*pincfg3 = pin_CFG.CFG3;
	pin5->CNF[0] = pin_CNF.CNF5;
	*pinmux5 = pin_MUX.PINMUX5;
	*pincfg5 = pin_CFG.CFG5;
	pin7->CNF[0] = pin_CNF.CNF7;
	*pinmux7 = pin_MUX.PINMUX7;
	*pincfg7 = pin_CFG.CFG7;
	pin8->CNF[0] = pin_CNF.CNF8;
	*pinmux8 = pin_MUX.PINMUX8;
	*pincfg8 = pin_CFG.CFG8;
	pin10->CNF[0] = pin_CNF.CNF10;
	*pinmux10 = pin_MUX.PINMUX10;
	*pincfg10 = pin_CFG.CFG10;
	pin11->CNF[0] = pin_CNF.CNF11;
	*pinmux11 = pin_MUX.PINMUX11;
	*pincfg11 = pin_CFG.CFG11;
	pin12->CNF[0] = pin_CNF.CNF12;
	*pinmux12 = pin_MUX.PINMUX12;
	*pincfg12 = pin_CFG.CFG12;
	pin13->CNF[0] = pin_CNF.CNF13;
	*pinmux13 = pin_MUX.PINMUX13;
	*pincfg13 = pin_CFG.CFG13;
	pin15->CNF[0] = pin_CNF.CNF15;
	*pinmux15 = pin_MUX.PINMUX15;
	*pincfg15 = pin_CFG.CFG15;
	pin16->CNF[0] = pin_CNF.CNF16;
	*pinmux16 = pin_MUX.PINMUX16;
	*pincfg16 = pin_CFG.CFG16;
	pin18->CNF[0] = pin_CNF.CNF18;
	*pinmux18 = pin_MUX.PINMUX18;
	*pincfg18 = pin_CFG.CFG18;
	pin19->CNF[0] = pin_CNF.CNF19;
	*pinmux19 = pin_MUX.PINMUX19;
	*pincfg19 = pin_CFG.CFG19;
	pin21->CNF[0] = pin_CNF.CNF21;
	*pinmux21 = pin_MUX.PINMUX21;
	*pincfg21 = pin_CFG.CFG21;
	pin22->CNF[0] = pin_CNF.CNF22;
	*pinmux22 = pin_MUX.PINMUX22;
	*pincfg22 = pin_CFG.CFG22;
	pin23->CNF[0] = pin_CNF.CNF23;
	*pinmux23 = pin_MUX.PINMUX23;
	*pincfg23 = pin_CFG.CFG23;
	pin24->CNF[0] = pin_CNF.CNF24;
	*pinmux24 = pin_MUX.PINMUX24;
	*pincfg24 = pin_CFG.CFG24;
	pin26->CNF[0] = pin_CNF.CNF26;
	*pinmux26 = pin_MUX.PINMUX26;
	*pincfg26 = pin_CFG.CFG26;
	pin27->CNF[0] = pin_CNF.CNF27;
	*pinmux27 = pin_MUX.PINMUX27;
	*pincfg27 = pin_CFG.CFG27;
	pin28->CNF[0] = pin_CNF.CNF28;
	*pinmux28 = pin_MUX.PINMUX28;
	*pincfg28 = pin_CFG.CFG28;
	pin29->CNF[0] = pin_CNF.CNF29;
	*pinmux29 = pin_MUX.PINMUX29;
	*pincfg29 = pin_CFG.CFG29;
	pin31->CNF[0] = pin_CNF.CNF31;
	*pinmux31 = pin_MUX.PINMUX31;
	*pincfg31 = pin_CFG.CFG31;
	pin32->CNF[0] = pin_CNF.CNF32;
	*pinmux32 = pin_MUX.PINMUX32;
	*pincfg32 = pin_CFG.CFG32;
	pin33->CNF[0] = pin_CNF.CNF33;
	*pinmux33 = pin_MUX.PINMUX33;
	*pincfg33 = pin_CFG.CFG33;
	pin35->CNF[0] = pin_CNF.CNF35;
	*pinmux35 = pin_MUX.PINMUX35;
	*pincfg35 = pin_CFG.CFG35;
	pin36->CNF[0] = pin_CNF.CNF36;
	*pinmux36 = pin_MUX.PINMUX36;
	*pincfg36 = pin_CFG.CFG36;
	pin37->CNF[0] = pin_CNF.CNF37;
	*pinmux37 = pin_MUX.PINMUX37;
	*pincfg37 = pin_CFG.CFG37;
	pin38->CNF[0] = pin_CNF.CNF38;
	*pinmux38 = pin_MUX.PINMUX38;
	*pincfg38 = pin_CFG.CFG38;
	pin40->CNF[0] = pin_CNF.CNF40;
	*pinmux40 = pin_MUX.PINMUX40;
	*pincfg40 = pin_CFG.CFG40;
	
	/* Ummapping CNF registers */
    munmap(baseCNF, pagesize);
    
	/* Ummapping PINMUX registers */
	munmap(basePINMUX, pagesize);
	
	/* Ummapping CFG registers */
	munmap(baseCFG, pagesize);
	
	/* Ummapping PWM registers */
	munmap(basePWM, pagesize);
  
	/* close /dev/mem */
    close(fd_GPIO);
}

int gpioSetMode(unsigned gpio, unsigned mode)
{
	int status = 1;
	if (mode == 0) {
	switch (gpio){
		
		case 3:
			*pinmux3 = PINMUX_IN;
			*pincfg3 = CFG_IN;
			pin3->CNF[0] |= 0x00000008;
			pin3->OE[0] &= ~(0x00000008);
			break;
		case 5:
			*pinmux5 = PINMUX_IN;
			*pincfg5 = CFG_IN;
			pin5->CNF[0] |= 0x00000004;
			pin5->OE[0] &= ~(0x00000004);
			break;
		case 7:
			*pinmux7 = PINMUX_IN;
			*pincfg7 = CFG_IN;
			pin7->CNF[0] |= 0x00000001;
			pin7->OE[0] &= ~(0x00000001);
			break;
		case 8:
			*pinmux8 = PINMUX_IN;
			*pincfg8 = CFG_IN;
			pin8->CNF[0] |= 0x00000001;
			pin8->OE[0] &= ~(0x00000001);
			break;
		case 10:
			*pinmux10 = PINMUX_IN;
			*pincfg10 = CFG_IN;
			pin10->CNF[0] |= 0x00000002;
			pin10->OE[0] &= ~(0x00000002);
			break;
		case 11:
			*pinmux11 = PINMUX_IN;
			*pincfg11 = CFG_IN;
			pin11->CNF[0] |= 0x00000004;
			pin11->OE[0] &= ~(0x00000004);
			break;
	    case 12:
			*pinmux12 = PINMUX_IN;
			*pincfg12 = CFG_IN;
			pin12->CNF[0] |= 0x00000080;
			pin12->OE[0] &= ~(0x00000080);
			break;
		case 13:
			*pinmux13 = PINMUX_IN;
			*pincfg13 = CFG_IN;
			pin13->CNF[0] |= 0x00000040;
			pin13->OE[0] &= ~(0x00000040);
			break;
		case 15:
			*pinmux15 = PINMUX_IN;
			*pincfg15 = CFG_IN;
			pin15->CNF[0] |= 0x00000004;
			pin15->OE[0] &= ~(0x00000004);
			break;
		case 16:
			*pinmux16 = PINMUX_IN;
			*pincfg16 = CFG_IN;
			pin16->CNF[0] |= 0x00000001;
			pin16->OE[0] &= ~(0x00000001);
			break;
		case 18:
			*pinmux18 = PINMUX_IN;
			*pincfg18 = CFG_IN;
			pin18->CNF[0] |= 0x00000080;
			pin18->OE[0] &= ~(0x00000080);
			break;
		case 19:
			*pinmux19 = PINMUX_IN;
			*pincfg19 = CFG_IN;
			pin19->CNF[0] |= 0x00000001;
			pin19->OE[0] &= ~(0x00000001);
			break;
		case 21:
			*pinmux21 = PINMUX_IN;
			*pincfg21 = CFG_IN;
			pin21->CNF[0] |= 0x00000002;
			pin21->OE[0] &= ~(0x00000002);
			break;
		case 22:
			*pinmux22 = PINMUX_IN;
			*pincfg22 = CFG_IN;
			pin22->CNF[0] |= 0x00000020;
			pin22->OE[0] &= ~(0x00000020);
			break;
		case 23:
			*pinmux23 = PINMUX_IN;
			*pincfg23 = CFG_IN;
			pin23->CNF[0] |= 0x00000004;
			pin23->OE[0] &= ~(0x00000004);
			break;
		case 24:
			*pinmux24 = PINMUX_IN;
			*pincfg24 = CFG_IN;
			pin24->CNF[0] |= 0x00000008;
			pin24->OE[0] &= ~(0x00000008);
			break;
		case 26:
			*pinmux26 = PINMUX_IN;
			*pincfg26 = CFG_IN;
			pin26->CNF[0] |= 0x00000010;
			pin26->OE[0] &= ~(0x00000010);
			break;
		case 27:
			*pinmux27 = PINMUX_IN;
			*pincfg27 = CFG_IN;
			pin27->CNF[0] |= 0x00000001;
			pin27->OE[0] &= ~(0x00000001);
			break;
		case 28:
			*pinmux28 = PINMUX_IN;
			*pincfg28 = CFG_IN;
			pin28->CNF[0] |= 0x00000002;
			pin28->OE[0] &= ~(0x00000002);
			break;
		case 29:
			*pinmux29 = PINMUX_IN;
			*pincfg29 = CFG_IN;
			pin29->CNF[0] |= 0x00000020;
			pin29->OE[0] &= ~(0x00000020);
			break;
		case 31:
			*pinmux31 = PINMUX_IN;
			*pincfg31 = CFG_IN;
			pin31->CNF[0] |= 0x00000001;
			pin31->OE[0] &= ~(0x00000001);
			break;
		case 32:
			*pinmux32 = PINMUX_IN;
			*pincfg32 = CFG_IN;
			pin32->CNF[0] |= 0x00000001;
			pin32->OE[0] &= ~(0x00000001);
			break;
		case 33:
			*pinmux33 = PINMUX_IN;
			*pincfg33 = CFG_IN;
			pin33->CNF[0] |= 0x00000040;
			pin33->OE[0] &= ~(0x00000040);
			break;
		case 35:
			*pinmux35 = PINMUX_IN;
			*pincfg35 = CFG_IN;
			pin35->CNF[0] |= 0x00000010;
			pin35->OE[0] &= ~(0x00000000);
			break;
		case 36:
			*pinmux36 = PINMUX_IN;
			*pincfg36 = CFG_IN;
			pin36->CNF[0] |= 0x00000008;
			pin36->OE[0] &= ~(0x00000008);
			break;
		case 37:
			*pinmux37 = PINMUX_IN;
			*pincfg37 = CFG_IN;
			pin37->CNF[0] |= 0x00000010;
			pin37->OE[0] &= ~(0x00000010);
			break;
		case 38:
			*pinmux38 = PINMUX_IN;
			*pincfg38 = CFG_IN;
			pin38->CNF[0] |= 0x00000020;
			pin38->OE[0] &= ~(0x00000020);
			break;
		case 40:
			*pinmux40 = PINMUX_IN;
			*pincfg40 = CFG_IN;
			pin40->CNF[0] |= 0x00000040;
			pin40->OE[0] &= ~(0x00000040);
			break;
		default:
			status = -1;
			printf("Only gpio numbers from 3 to 40 are accepted, this function will read the level on the Jetson Nano header pins,\n");
			printf("numbered as the header pin numbers e.g. AUD_MCLK is pin header number 7\n");
		}
	}
	else if (mode == 1) {
		switch (gpio){
		
		case 3:
			*pinmux3 = PINMUX_OUT;
			*pincfg3 = CFG_OUT;
			pin3->CNF[0] |= 0x00000008;
			pin3->OE[0] |= 0x00000008;
			break;
		case 5:
			*pinmux5 = PINMUX_OUT;
			*pincfg5 = CFG_OUT;
			pin5->CNF[0] |= 0x00000004;
			pin5->OE[0] |= 0x00000004;
			break;
		case 7:
			*pinmux7 = PINMUX_OUT;
			*pincfg7 = CFG_OUT;
			pin7->CNF[0] |= 0x00000001;
			pin7->OE[0] |= 0x00000001;
			break;
		case 8:
			*pinmux8 = PINMUX_OUT;
			*pincfg8 = CFG_OUT;
			pin8->CNF[0] |= 0x00000001;
			pin8->OE[0] |= 0x00000001;
			break;
		case 10:
			*pinmux10 = PINMUX_OUT;
			*pincfg10 = CFG_OUT;
			pin10->CNF[0] |= 0x00000002;
			pin10->OE[0] |= 0x00000002;
			break;
		case 11:
			*pinmux11 = PINMUX_OUT;
			*pincfg11 = CFG_OUT;
			pin11->CNF[0] |= 0x00000004;
			pin11->OE[0] |= 0x00000004;
			break;
	    case 12:
			*pinmux12 = PINMUX_OUT;
			*pincfg12 = CFG_OUT;
			pin12->CNF[0] |= 0x00000080;
			pin12->OE[0] |= 0x00000080;
			break;
		case 13:
			*pinmux13 = PINMUX_OUT;
			*pincfg13 = CFG_OUT;
			pin13->CNF[0] |= 0x00000040;
			pin13->OE[0] |= 0x00000040;
			break;
		case 15:
			*pinmux15 = PINMUX_OUT;
			*pincfg15 = CFG_OUT;
			pin15->CNF[0] |= 0x00000004;
			pin15->OE[0] |= 0x00000004;
			break;
		case 16:
			*pinmux16 = PINMUX_OUT;
			*pincfg16 = CFG_OUT;
			pin16->CNF[0] |= 0x00000001;
			pin16->OE[0] |= 0x00000001;
			break;
		case 18:
			*pinmux18 = PINMUX_OUT;
			*pincfg18 = CFG_OUT;
			pin18->CNF[0] |= 0x00000080;
			pin18->OE[0] |= 0x00000080;
			break;
		case 19:
			*pinmux19 = PINMUX_OUT;
			*pincfg19 = CFG_OUT;
			pin19->CNF[0] |= 0x00000001;
			pin19->OE[0] |= 0x00000001;
			break;
		case 21:
			*pinmux21 = PINMUX_OUT;
			*pincfg21 = CFG_OUT;
			pin21->CNF[0] |= 0x00000002;
			pin21->OE[0] |= 0x00000002;
			break;
		case 22:
			*pinmux22 = PINMUX_OUT;
			*pincfg22 = CFG_OUT;
			pin22->CNF[0] |= 0x00000020;
			pin22->OE[0] |= 0x00000020;
			break;
		case 23:
			*pinmux23 = PINMUX_OUT;
			*pincfg23 = CFG_OUT;
			pin23->CNF[0] |= 0x00000004;
			pin23->OE[0] |= 0x00000004;
			break;
		case 24:
			*pinmux24 = PINMUX_OUT;
			*pincfg24 = CFG_OUT;
			pin24->CNF[0] |= 0x00000008;
			pin24->OE[0] |= 0x00000008;
			break;
		case 26:
			*pinmux26 = PINMUX_OUT;
			*pincfg26 = CFG_OUT;
			pin26->CNF[0] |= 0x00000010;
			pin26->OE[0] |= 0x00000010;
			break;
		case 27:
			*pinmux27 = PINMUX_OUT;
			*pincfg27 = CFG_OUT;
			pin27->CNF[0] |= 0x00000001;
			pin27->OE[0] |= 0x00000001;
			break;
		case 28:
			*pinmux28 = PINMUX_OUT;
			*pincfg28 = CFG_OUT;
			pin28->CNF[0] |= 0x00000002;
			pin28->OE[0] |= 0x00000002;
			break;
		case 29:
			*pinmux29 = PINMUX_OUT;
			*pincfg29 = CFG_OUT;
			pin29->CNF[0] |= 0x00000020;
			pin29->OE[0] |= 0x00000020;
			break;
		case 31:
			*pinmux31 = PINMUX_OUT;
			*pincfg31 = CFG_OUT;
			pin31->CNF[0] |= 0x00000001;
			pin31->OE[0] |= 0x00000001;
			break;
		case 32:
			*pinmux32 = PINMUX_OUT;
			*pincfg32 = CFG_OUT;
			pin32->CNF[0] |= 0x00000001;
			pin32->OE[0] |= 0x00000001;
			break;
		case 33:
			*pinmux33 = PINMUX_OUT;
			*pincfg33 = CFG_OUT;
			pin33->CNF[0] |= 0x00000040;
			pin33->OE[0] |= 0x00000040;
			break;
		case 35:
			*pinmux35 = PINMUX_OUT;
			*pincfg35 = CFG_OUT;
			pin35->CNF[0] |= 0x00000010;
			pin35->OE[0] |= 0x00000010;
			break;
		case 36:
			*pinmux36 = PINMUX_OUT;
			*pincfg36 = CFG_OUT;
			pin36->CNF[0] |= 0x00000008;
			pin36->OE[0] |= 0x00000008;
			break;
		case 37:
			*pinmux37 = PINMUX_OUT;
			*pincfg37 = CFG_OUT;
			pin37->CNF[0] |= 0x00000010;
			pin37->OE[0] |= 0x00000010;
			break;
		case 38:
			*pinmux38 = PINMUX_OUT;
			*pincfg38 = CFG_OUT;
			pin38->CNF[0] |= 0x00000020;
			pin38->OE[0] |= 0x00000020;
			break;
		case 40:
			*pinmux40 = PINMUX_OUT;
			*pincfg40 = CFG_OUT;
			pin40->CNF[0] |= 0x00000040;
			pin40->OE[0] |= 0x00000040;
			break;
		default:
			status = -1;
			printf("Only gpio numbers from 3 to 40 are accepted, this function will only read the level on the Jetson Nano header pins,\n");
			printf("numbered as the header pin numbers e.g. AUD_MCLK is pin header number 7\n");
		}
		
	}
	else {printf("Only modes allowed are JET_INPUT and JET_OUTPUT\n");
		status = -2;
		}
	return status;
	
}

int gpioRead(unsigned gpio)
{
	int level  = 0;
	switch (gpio){
		
		case 3:
			level = (pin3->IN[0])>>3 & 1;
			break;
		case 5:
			level = (pin5->IN[0])>>2 & 1;
			break;
		case 7:
			level = pin7->IN[0] & 1;
			break;
		case 8:
			level = pin8->IN[0] & 1;
			break;
		case 10:
			level = (pin10->IN[0])>>1 & 1;
			break;
		case 11:
			level = (pin11->IN[0])>>2 & 1;
			break;
	    case 12:
			level = (pin12->IN[0])>>7 & 1;
			break;
		case 13:
			level = (pin13->IN[0])>>6 & 1;
			break;
		case 15:
			level = (pin15->IN[0])>>2 & 1;
			break;
		case 16:
			level = pin16->IN[0] & 1;
			break;
		case 18:
			level = (pin18->IN[0])>>7 & 1;
			break;
		case 19:
			level = pin19->IN[0] & 1;
			break;
		case 21:
			level = (pin21->IN[0])>>1 & 1;
			break;
		case 22:
			level = (pin22->IN[0])>>5 & 1;
			break;
		case 23:
			level = (pin23->IN[0])>>2 & 1;
			break;
		case 24:
			level = (pin24->IN[0])>>3 & 1;
			break;
		case 26:
			level = (pin26->IN[0])>>4 & 1;
			break;
		case 27:
			level = pin27->IN[0] & 1;
			break;
		case 28:
			level = (pin28->IN[0])>>1 & 1;
			break;
		case 29:
			level = (pin29->IN[0])>>5 & 1;
			break;
		case 31:
			level = pin31->IN[0] & 1;
			break;
		case 32:
			level = pin32->IN[0] & 1;
			break;
		case 33:
			level = (pin33->IN[0])>>6 & 1;
			break;
		case 35:
			level = (pin35->IN[0])>>4 & 1;
			break;
		case 36:
			level = (pin36->IN[0])>>3 & 1;
			break;
		case 37:
			level = (pin37->IN[0])>>4 & 1;
			break;
		case 38:
			level = (pin38->IN[0])>>5 & 1;
			break;
		case 40:
			level = (pin40->IN[0])>>6 & 1;
			break;
		default:
			level = -1;
			printf("Only gpio numbers from 3 to 40 are accepted, this function will only read the level of the Jetson Nano header pins,\n");
			printf("numbered as the header pin numbers e.g. AUD_MCLK is pin header number 7\n");
	}
	return level;
}

int gpioWrite(unsigned gpio, unsigned level)
{
	int status = 1;
	if (level == 0) {
	switch (gpio){
		
		case 3:
			pin3->OUT[0] &= ~(0x00000008);
			break;
		case 5:
			pin5->OUT[0] &= ~(0x00000004);
			break;
		case 7:
			pin7->OUT[0] &= ~(0x00000001);
			break;
		case 8:
			pin8->OUT[0] &= ~(0x00000001);
			break;
		case 10:
			pin10->OUT[0] &= ~(0x00000002);
			break;
		case 11:
			pin11->OUT[0] &= ~(0x00000004);
			break;
	    case 12:
			pin12->OUT[0] &= ~(0x00000080);
			break;
		case 13:
			pin13->OUT[0] &= ~(0x00000040);
			break;
		case 15:
			pin15->OUT[0] &= ~(0x00000004);
			break;
		case 16:
			pin16->OUT[0] &= ~(0x00000001);
			break;
		case 18:
			pin18->OUT[0] &= ~(0x00000080);
			break;
		case 19:
			pin19->OUT[0] &= ~(0x00000001);
			break;
		case 21:
			pin21->OUT[0] &= ~(0x00000002);
			break;
		case 22:
			pin22->OUT[0] &= ~(0x00000020);
			break;
		case 23:
			pin23->OUT[0] &= ~(0x00000004);
			break;
		case 24:
			pin24->OUT[0] &= ~(0x00000008);
			break;
		case 26:
			pin26->OUT[0] &= ~(0x00000010);
			break;
		case 27:
			pin27->OUT[0] &= ~(0x00000001);
			break;
		case 28:
			pin28->OUT[0] &= ~(0x00000002);
			break;
		case 29:
			pin29->OUT[0] &= ~(0x00000020);
			break;
		case 31:
			pin31->OUT[0] &= ~(0x00000001);
			break;
		case 32:
			pin32->OUT[0] &= ~(0x00000001);
			break;
		case 33:
			pin33->OUT[0] &= ~(0x00000040);
			break;
		case 35:
			pin35->OUT[0] &= ~(0x00000010);
			break;
		case 36:
			pin36->OUT[0] &= ~(0x00000008);
			break;
		case 37:
			pin37->OUT[0] &= ~(0x00000010);
			break;
		case 38:
			pin38->OUT[0] &= ~(0x00000020);
			break;
		case 40:
			pin40->OUT[0] &= ~(0x00000040);
			break;
		default:
			status = -1;
			printf("Only gpio numbers from 3 to 40 are accepted, this function will only read the level of the Jetson Nano header pins,\n");
			printf("numbered as the header pin numbers e.g. AUD_MCLK is pin header number 7\n");
		}
	}
	else if (level == 1) {
	switch (gpio){
		
		case 3:
			pin3->OUT[0] |= level<<3;
			break;
		case 5:
			pin5->OUT[0] |= level<<2;
			break;
		case 7:
			pin7->OUT[0] |= level;
			break;
		case 8:
			pin8->OUT[0] |= level;
			break;
		case 10:
			pin10->OUT[0] |= level<<1;
			break;
		case 11:
			pin11->OUT[0] |= level<<2;
			break;
	    case 12:
			pin12->OUT[0] |= level<<7;
			break;
		case 13:
			pin13->OUT[0] |= level<<6;
			break;
		case 15:
			pin15->OUT[0] |= level<<2;
			break;
		case 16:
			pin16->OUT[0] |= level;
			break;
		case 18:
			pin18->OUT[0] |= level<<7;
			break;
		case 19:
			pin19->OUT[0] |= level;
			break;
		case 21:
			pin21->OUT[0] |= level<<1;
			break;
		case 22:
			pin22->OUT[0] |= level<<5;
			break;
		case 23:
			pin23->OUT[0] |= level<<2;
			break;
		case 24:
			pin24->OUT[0] |= level<<3;
			break;
		case 26:
			pin26->OUT[0] |= level<<4;
			break;
		case 27:
			pin27->OUT[0] |= level;
			break;
		case 28:
			pin28->OUT[0] |= level<<1;
			break;
		case 29:
			pin29->OUT[0] |= level<<5;
			break;
		case 31:
			pin31->OUT[0] |= level;
			break;
		case 32:
			pin32->OUT[0] |= level;
			break;
		case 33:
			pin33->OUT[0] |= level<<6;
			break;
		case 35:
			pin35->OUT[0] |= level<<4;
			break;
		case 36:
			pin36->OUT[0] |= level<<3;
			break;
		case 37:
			pin37->OUT[0] |= level<<4;
			break;
		case 38:
			pin38->OUT[0] |= level<<5;
			break;
		case 40:
			pin40->OUT[0] |= level<<6;
			break;
		default:
			status = -1;
			printf("Only gpio numbers from 3 to 40 are accepted, this function will only read the level of the Jetson Nano header pins,\n");
			printf("numbered as the header pin numbers e.g. AUD_MCLK is pin header number 7\n");
		}
	}
	else {printf("Only levels 0 or 1 are allowed\n");}
	return status;
}

int gpioSetPWMfrequency(unsigned gpio, unsigned frequency)
{
	int status = 1;
	int PFM =0;
	if ((frequency >= 25) && (frequency <=200000)){
		PFM = (204800/frequency)-1;
	switch (gpio){
		case 32:
			pinPWM->PWM_0[0] = 0x0;
			pinPWM->PWM_0[0] = PFM;
			break;
		case 33:
			pinPWM->PWM_2[0] = 0x0;
			pinPWM->PWM_2[0] = PFM;
			break;
		default:
			//status = -1;
			printf("Only gpio numbers f32 and 33 are accepted,\n");
		}
		
	}
	else {printf("Only frequencies from 25 to 200000 Hz are allowed\n");
		status =-1;}
	return status;
}

int gpioPWM(unsigned gpio, unsigned dutycycle)
{
	int status = 1;
	if ((dutycycle >= 0) && (dutycycle <=256)){
	switch (gpio){
		
		case 32:
			*pinmux32 = 0x00000001;
			*pincfg32 = CFG_OUT;
			pin32->CNF[0] &= ~(0x00000001);
			pinPWM->PWM_0[0] &= ~(0xFFFF0000);
			pinPWM->PWM_0[0] |= dutycycle<<16;
			pinPWM->PWM_0[0] |= 0x80000000;
			break;
		case 33:
			*pinmux33 = 0x00000002;
			*pincfg33 = CFG_OUT;
			pin33->CNF[0] &= ~(0x00000040);
			pinPWM->PWM_2[0] &= ~(0xFFFF0000);
			pinPWM->PWM_2[0] |= dutycycle<<16;
			pinPWM->PWM_2[0] |= 0x80000000;
			break;
		default:
			status = -1;
			printf("Only gpio numbers f32 and 33 are accepted,\n");
		}
		
	}
	else {printf("Only a dutycycle from 0 to 256 is allowed\n");
		status =-1;}
	return status;
}

int i2c_smbus_access(int file, char read_write, __u8 command, int size, union i2c_smbus_data *data)
{
	struct i2c_smbus_ioctl_data args;
	args.read_write = read_write;
	args.command = command;
	args.size = size;
	args.data = data;
	return ioctl(file,I2C_SMBUS,&args);
}

int i2cOpen(unsigned i2cBus, unsigned i2cAddr, unsigned i2cFlags)
{
	char dev[20], buf[100];
	int fd, slot, speed;
	uint32_t funcs;
	FILE *fptr;

	if (i2cAddr > 0x7f){
      	printf( "bad I2C address (%d)", i2cAddr);
	return -1;
	}

	if (i2cFlags > 3){
        printf( "Only flags 0 to 2 are supported to set up bus speed");
        return -2;
	}
	
	switch(i2cFlags) {
		
		case 0: 
			speed = 100000;
			break;
		case 1:
			speed = 400000;
			break;
		case 2: 
			speed = 1000000;
			break;
		default:
			i2cFlags = 3;
	}
	
	slot = -3;
	
	if (i2cInfo[i2cBus].state == I2C_CLOSED) {
			slot = i2cBus;
			i2cInfo[slot].state = I2C_RESERVED;
	}
	else { printf("i2c bus already open");
		return -3;
	}
	
	snprintf(buf, sizeof(buf), "/sys/bus/i2c/devices/i2c-%d/bus_clk_rate", i2cBus);
	fptr = fopen(buf, "r");
	
	if (fptr == NULL) {
	printf("not possible to read current bus speed\n");
	}
	
	fscanf(fptr, "%d", &i2c_speed[i2cBus]);
   	
   	snprintf(buf, sizeof(buf), "echo %d > /sys/bus/i2c/devices/i2c-%d/bus_clk_rate", speed, i2cBus);
   	if (system(buf) == -1) { 
		printf( "not possible to change bus speed\n");
	}
	
	snprintf(dev, 19, "/dev/i2c-%d", i2cBus);
	fd = open(dev, O_RDWR);
	if (fd < 0) {
		printf( "bad handle (%d)\n", fd);
		return -4;	
	}
	
	if (ioctl(fd, I2C_SLAVE, i2cAddr) < 0) {
      	close(fd);
      	i2cInfo[slot].state = 0;
      	return -6;;
   	}

   	if (ioctl(fd, I2C_FUNCS, &funcs) < 0){
      		funcs = -1; /* assume all smbus commands allowed */
      		return -7;
   	}

   i2cInfo[slot].fd = fd;
   i2cInfo[slot].addr = i2cAddr;
   i2cInfo[slot].flags = i2cFlags;
   i2cInfo[slot].funcs = funcs;
   i2cInfo[slot].state = I2C_OPENED;

   return slot;
}

int i2cClose(unsigned handle)
{
	char buf[100];
	
	if (handle > 1) {
      printf( "bad handle (%d)", handle);
		return(-1);
	}

	if (i2cInfo[handle].state != I2C_OPENED) {
	   printf( "i2c bus is already closed (%d)", handle);
		return(-1);	
	}
     
	if (i2cInfo[handle].fd >= 0) {close(i2cInfo[handle].fd);};

	i2cInfo[handle].fd = -1;
	i2cInfo[handle].state = I2C_CLOSED;
   
	snprintf(buf, sizeof(buf), "echo %d > /sys/bus/i2c/devices/i2c-%d/bus_clk_rate", i2c_speed[handle], handle);
   	if (system(buf) == -1) { 
		printf( "not possible to return bus speed to original value\n");
	}

   return 0;
}

int i2cWriteByteData(unsigned handle, unsigned reg, unsigned bVal)
{	
	union i2c_smbus_data data;
	int status = 0;
	
	if (handle >= 2) {
		printf( "bad handle (%d)", handle);
		status = -1;
	}

   if (i2cInfo[handle].state != I2C_OPENED){
		printf( "i2c%d is not open)", handle);
		status = -2;
	}

   if ((i2cInfo[handle].funcs & I2C_FUNC_SMBUS_WRITE_BYTE_DATA) == 0){
		printf( "write byte data function not supported by device");
		status = -3;
	}
	
   if (reg > 0x7F){
		printf( "register address on device bigger than 0x7F");
		status = -4;
	}

   if (bVal > 0xFF){
		printf( "value to be written bigger than byte");
		status = -5;
	}
	
	data.byte = bVal;
	if (i2c_smbus_access(i2cInfo[handle].fd,I2C_SMBUS_WRITE,reg, I2C_SMBUS_BYTE_DATA, &data)) {
		status = -6;}
	return status;
}

int i2cReadByteData(unsigned handle, unsigned reg)
{
	int status;
	union i2c_smbus_data data;
	
	if (handle >= 2) {
		printf( "bad handle (%d)", handle);
		status = -1;
	}

   if (i2cInfo[handle].state != I2C_OPENED){
		printf( "i2c%d is not open)", handle);
		status = -2;
	}

   if ((i2cInfo[handle].funcs & I2C_FUNC_SMBUS_READ_BYTE_DATA) == 0){
		printf( "write byte data function not supported by device");
		status = -3;
	}
	
   if (reg > 0x7F){
		printf( "register address on device bigger than 0x7F");
		status = -4;
	}
	
	if (i2c_smbus_access(i2cInfo[handle].fd,I2C_SMBUS_READ,reg, I2C_SMBUS_BYTE_DATA,&data)) {
		status = -5;}
	else
		{status = 0x0FF & data.byte;}
	return status;
}
