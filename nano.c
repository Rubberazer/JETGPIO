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

/* jetgpio version 1.2 */
/* Nano classic extension */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <errno.h>
#include <unistd.h>
#include <math.h>
#include <sys/fcntl.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/file.h>
#include <sys/types.h>
#include <sys/poll.h>
#include <linux/i2c-dev.h>
#include <linux/i2c.h>
#include <linux/spi/spidev.h>
#include <linux/types.h>
#include <linux/gpio.h>
#include <pthread.h>


#include "jetgpio.h"

static int fd_GPIO;

static volatile GPIO_CNF_Init pin_CNF;
static volatile GPIO_CNF_Init pin_OE;
static volatile GPIO_CNF_Init pin_ENB;
static volatile GPIO_CNF_Init pin_LVL;
static volatile GPIO_CNF_Init pin_MUX;
static volatile GPIO_CNF_Init pin_CFG;
PISRFunc ISRFunc_CFG[41];

static volatile GPIO_PWM pinPWM_Init;
static volatile GPIO_PWM *pinPWM;

static i2cInfo_t i2cInfo[2];
static int i2c_speed[2];

static SPIInfo_t SpiInfo[2];

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

static volatile uint32_t *controller_clk_out_enb_l;
static volatile uint32_t *controller_clk_out_enb_h;
static volatile uint32_t *controller_clk_out_enb_l_set;
static volatile uint32_t *controller_rst_devices_h;
static volatile uint32_t *clk_source_spi1;
static volatile uint32_t *clk_source_spi2;

static volatile uint32_t *apbdev_pmc_pwr_det_val;

static void *baseCNF;

static void *basePINMUX;

static void *baseCFG;

static void *basePWM;

static void *baseCAR;

static void *basePMC;

static volatile unsigned global_int;
static pthread_t callThd[28];
static pthread_attr_t attr;
static int pth_err;
static void *status_thread;
static int thread_n = 0;
static unsigned pin_tracker = 0;

int gpioInitialise(void){
    
  int status = 1;
  //  Getting the page size
  int pagesize = sysconf(_SC_PAGESIZE);    //getpagesize();
	
  //  read physical memory (needs root)
  fd_GPIO = open("/dev/mem", O_RDWR | O_SYNC);
  if (fd_GPIO < 0) {
    perror("/dev/mem");
    fprintf(stderr, "Please run this program as root (for example with sudo)\n");
    return -1;
  }
  //  Mapping GPIO_CNF
  baseCNF = mmap(0, pagesize, PROT_READ | PROT_WRITE, MAP_SHARED, fd_GPIO, base_CNF);
  if (baseCNF == MAP_FAILED) {
    return -2;
  }
    
  //  Mapping GPIO_PINMUX
  basePINMUX = mmap(0, pagesize, PROT_READ | PROT_WRITE, MAP_SHARED, fd_GPIO, base_PINMUX);
  if (basePINMUX == MAP_FAILED) {
    return -3;
  }
    
  //  Mapping GPIO_CFG
  baseCFG = mmap(0, pagesize, PROT_READ | PROT_WRITE, MAP_SHARED, fd_GPIO, base_CFG);
  if (baseCFG == MAP_FAILED) {
    return -4;
  }
    
  //  Mapping GPIO_PWM
  basePWM = mmap(0, pagesize, PROT_READ | PROT_WRITE, MAP_SHARED, fd_GPIO, base_PWM);
  if (basePWM == MAP_FAILED) {
    return -5;
  }
    
  //  Mapping CAR
  baseCAR = mmap(0, pagesize, PROT_READ | PROT_WRITE, MAP_SHARED, fd_GPIO, CAR);
  if (baseCAR == MAP_FAILED) {
    return -6;
  }

  //  Mapping PMC
  basePMC = mmap(NULL, pagesize, PROT_READ | PROT_WRITE, MAP_SHARED, fd_GPIO, base_PMC);
  if (basePMC == MAP_FAILED) {
    return -7;
  }  
    
  // Pointer to CNF3
  pin3 = (GPIO_CNF volatile *)((char *)baseCNF + CNF_3);
  pin_CNF.pin3 = pin3->CNF[0];
  pin_OE.pin3 = pin3->OE[0];
  pin_ENB.pin3 = pin3->INT_ENB[0];
  pin_LVL.pin3 = pin3->INT_LVL[0];
    
  // Pointer to PINMUX3
  pinmux3 = (uint32_t volatile *)((char *)basePINMUX + PINMUX_3);
  pin_MUX.pin3 = *pinmux3;
    
  // Pointer to PINCFG3
  pincfg3 = (uint32_t volatile *)((char *)baseCFG + CFG_3);
  pin_CFG.pin3 = *pincfg3;
    
  // Pointer to CNF5
  pin5 = (GPIO_CNF volatile *)((char *)baseCNF + CNF_5);
  pin_CNF.pin5 = pin5->CNF[0];
  pin_OE.pin5 = pin5->OE[0];
  pin_ENB.pin5 = pin5->INT_ENB[0];
  pin_LVL.pin5 = pin5->INT_LVL[0];
    
  // Pointer to PINMUX5
  pinmux5 = (uint32_t volatile *)((char *)basePINMUX + PINMUX_5);
  pin_MUX.pin5 = *pinmux5;
    
  // Pointer to PINCFG5
  pincfg5 = (uint32_t volatile *)((char *)baseCFG + CFG_5);
  pin_CFG.pin5 = *pincfg5;
    
  // Pointer to CNF7
  pin7 = (GPIO_CNF volatile *)((char *)baseCNF + CNF_7);
  pin_CNF.pin7 = pin7->CNF[0];
  pin_OE.pin7 = pin7->OE[0];
  pin_ENB.pin7 =  pin7->INT_ENB[0];
  pin_LVL.pin7 =  pin7->INT_LVL[0];
    
  // Pointer to PINMUX7
  pinmux7 = (uint32_t volatile *)((char *)basePINMUX + PINMUX_7);
  pin_MUX.pin7 = *pinmux7;
    
  // Pointer to PINCFG7
  pincfg7 = (uint32_t volatile *)((char *)baseCFG + CFG_7);
  pin_CFG.pin7 = *pincfg7;
    
  // Pointer to CNF8
  pin8 = (GPIO_CNF volatile *)((char *)baseCNF + CNF_8);
  pin_CNF.pin8 = pin8->CNF[0];
  pin_OE.pin8 = pin8->OE[0];
  pin_ENB.pin8 = pin8->INT_ENB[0];
  pin_LVL.pin8 = pin8->INT_LVL[0];
    
  // Pointer to PINMUX8
  pinmux8 = (uint32_t volatile *)((char *)basePINMUX + PINMUX_8);
  pin_MUX.pin8 = *pinmux8;
    
  // Pointer to PINCFG8
  pincfg8 = (uint32_t volatile *)((char *)baseCFG + CFG_8);
  pin_CFG.pin8 = *pincfg8;
    
  // Pointer to CNF10
  pin10 = (GPIO_CNF volatile *)((char *)baseCNF + CNF_10);
  pin_CNF.pin10 = pin10->CNF[0];
  pin_OE.pin10 = pin10->OE[0];
  pin_ENB.pin10 = pin10->INT_ENB[0];
  pin_LVL.pin10 = pin10->INT_LVL[0];
    
  // Pointer to PINMUX10
  pinmux10 = (uint32_t volatile *)((char *)basePINMUX + PINMUX_10);
  pin_MUX.pin10 = *pinmux10;
    
  // Pointer to PINCFG10
  pincfg10 = (uint32_t volatile *)((char *)baseCFG + CFG_10);
  pin_CFG.pin10 = *pincfg10;
    
  // Pointer to CNF11
  pin11 = (GPIO_CNF volatile *)((char *)baseCNF + CNF_11);
  pin_CNF.pin11 = pin11->CNF[0];
  pin_OE.pin11 = pin11->OE[0];
  pin_ENB.pin11 = pin11->INT_ENB[0];
  pin_LVL.pin11 = pin11->INT_LVL[0];
   
  // Pointer to PINMUX11
  pinmux11 = (uint32_t volatile *)((char *)basePINMUX + PINMUX_11);
  pin_MUX.pin11 = *pinmux11;
    
  // Pointer to PINCFG11
  pincfg11 = (uint32_t volatile *)((char *)baseCFG + CFG_11);
  pin_CFG.pin11 = *pincfg11;
    
  // Pointer to CNF12
  pin12 = (GPIO_CNF volatile *)((char *)baseCNF + CNF_12);
  pin_CNF.pin12 = pin12->CNF[0];
  pin_OE.pin12 = pin12->OE[0];
  pin_ENB.pin12 = pin12->INT_ENB[0];
  pin_LVL.pin12 = pin12->INT_LVL[0];
    
  // Pointer to PINMUX12
  pinmux12 = (uint32_t volatile *)((char *)basePINMUX + PINMUX_12);
  pin_MUX.pin12 = *pinmux12;
    
  // Pointer to PINCFG12
  pincfg12 = (uint32_t volatile *)((char *)baseCFG + CFG_12);
  pin_CFG.pin12 = *pincfg12;
    
  // Pointer to CNF13
  pin13 = (GPIO_CNF volatile *)((char *)baseCNF + CNF_13);
  pin_CNF.pin13 = pin13->CNF[0];
  pin_OE.pin13 = pin13->OE[0];
  pin_ENB.pin13 = pin13->INT_ENB[0];
  pin_LVL.pin13 = pin13->INT_LVL[0];
    
  // Pointer to PINMUX13
  pinmux13 = (uint32_t volatile *)((char *)basePINMUX + PINMUX_13);
  pin_MUX.pin13 = *pinmux13;
    
  // Pointer to PINCFG13
  pincfg13 = (uint32_t volatile *)((char *)baseCFG + CFG_13);
  pin_CFG.pin13 = *pincfg13;
    
  // Pointer to CNF15
  pin15 = (GPIO_CNF volatile *)((char *)baseCNF + CNF_15);
  pin_CNF.pin15 = pin15->CNF[0];
  pin_OE.pin15 = pin15->OE[0];
  pin_ENB.pin15 = pin15->INT_ENB[0];
  pin_LVL.pin15 = pin15->INT_LVL[0];
    
  // Pointer to PINMUX15
  pinmux15 = (uint32_t volatile *)((char *)basePINMUX + PINMUX_15);
  pin_MUX.pin15 = *pinmux15;
    
  // Pointer to PINCFG15
  pincfg15 = (uint32_t volatile *)((char *)baseCFG + CFG_15);
  pin_CFG.pin15 = *pincfg15;
    
  // Pointer to CNF16
  pin16 = (GPIO_CNF volatile *)((char *)baseCNF + CNF_16);
  pin_CNF.pin16 = pin16->CNF[0];
  pin_OE.pin16 = pin16->OE[0];
  pin_ENB.pin16 = pin16->INT_ENB[0];
  pin_LVL.pin16 = pin16->INT_LVL[0];
    
  // Pointer to PINMUX16
  pinmux16 = (uint32_t volatile *)((char *)basePINMUX + PINMUX_16);
  pin_MUX.pin16 = *pinmux16;
   
  // Pointer to PINCFG16
  pincfg16 = (uint32_t volatile *)((char *)baseCFG + CFG_16);
  pin_CFG.pin16 = *pincfg16;
    
  // Pointer to CNF18
  pin18 = (GPIO_CNF volatile *)((char *)baseCNF + CNF_18);
  pin_CNF.pin18 = pin18->CNF[0];
  pin_OE.pin18 = pin18->OE[0];
  pin_ENB.pin18 = pin18->INT_ENB[0];
  pin_LVL.pin18 =  pin18->INT_LVL[0];
    
  // Pointer to PINMUX18
  pinmux18 = (uint32_t volatile *)((char *)basePINMUX + PINMUX_18);
  pin_MUX.pin18 = *pinmux18;
    
  // Pointer to PINCFG18
  pincfg18 = (uint32_t volatile *)((char *)baseCFG + CFG_18);
  pin_CFG.pin18 = *pincfg18;
    
  // Pointer to CNF19
  pin19 = (GPIO_CNF volatile *)((char *)baseCNF + CNF_19);
  pin_CNF.pin19 = pin19->CNF[0];
  pin_OE.pin19 = pin19->OE[0];
  pin_ENB.pin19 = pin19->INT_ENB[0];
  pin_LVL.pin19 = pin19->INT_LVL[0];
    
  // Pointer to PINMUX19
  pinmux19 = (uint32_t volatile *)((char *)basePINMUX + PINMUX_19);
  pin_MUX.pin19 = *pinmux19;
    
  // Pointer to PINCFG19
  pincfg19 = (uint32_t volatile *)((char *)baseCFG + CFG_19);
  pin_CFG.pin19 = *pincfg19;
    
  // Pointer to CNF21
  pin21 = (GPIO_CNF volatile *)((char *)baseCNF + CNF_21);
  pin_CNF.pin21 = pin21->CNF[0];
  pin_OE.pin21 = pin21->OE[0];
  pin_ENB.pin21 = pin21->INT_ENB[0];
  pin_LVL.pin21 = pin21->INT_LVL[0];
    
  // Pointer to PINMUX21
  pinmux21 = (uint32_t volatile *)((char *)basePINMUX + PINMUX_21);
  pin_MUX.pin21 = *pinmux21;
    
  // Pointer to PINCFG21
  pincfg21 = (uint32_t volatile *)((char *)baseCFG + CFG_21);
  pin_CFG.pin21 = *pincfg21;
    
  // Pointer to CNF22
  pin22 = (GPIO_CNF volatile *)((char *)baseCNF + CNF_22);
  pin_CNF.pin22 = pin22->CNF[0];
  pin_OE.pin22 = pin22->OE[0];
  pin_ENB.pin22 = pin22->INT_ENB[0];
  pin_LVL.pin22 = pin22->INT_LVL[0];
    
  // Pointer to PINMUX22
  pinmux22 = (uint32_t volatile *)((char *)basePINMUX + PINMUX_22);
  pin_MUX.pin22 = *pinmux22;
    
  // Pointer to PINCFG22
  pincfg22 = (uint32_t volatile *)((char *)baseCFG + CFG_22);
  pin_CFG.pin22 = *pincfg22;
    
  // Pointer to CNF23
  pin23 = (GPIO_CNF volatile *)((char *)baseCNF + CNF_23);
  pin_CNF.pin23 = pin23->CNF[0];
  pin_OE.pin23 = pin23->OE[0];
  pin_ENB.pin23 = pin23->INT_ENB[0];
  pin_LVL.pin23 = pin23->INT_LVL[0];
    
  // Pointer to PINMUX23
  pinmux23 = (uint32_t volatile *)((char *)basePINMUX + PINMUX_23);
  pin_MUX.pin23 = *pinmux23;
    
  // Pointer to PINCFG23
  pincfg23 = (uint32_t volatile *)((char *)baseCFG + CFG_23);
  pin_CFG.pin23 = *pincfg23;
    
  // Pointer to CNF24
  pin24 = (GPIO_CNF volatile *)((char *)baseCNF + CNF_24);
  pin_CNF.pin24 = pin24->CNF[0];
  pin_OE.pin24 = pin24->OE[0];
  pin_ENB.pin24 = pin24->INT_ENB[0];
  pin_LVL.pin24 = pin24->INT_LVL[0];
    
  // Pointer to PINMUX24
  pinmux24 = (uint32_t volatile *)((char *)basePINMUX + PINMUX_24);
  pin_MUX.pin24 = *pinmux24;
    
  // Pointer to PINCFG24
  pincfg24 = (uint32_t volatile *)((char *)baseCFG + CFG_24);
  pin_CFG.pin24 = *pincfg24;
    
  // Pointer to CNF26
  pin26 = (GPIO_CNF volatile *)((char *)baseCNF + CNF_26);
  pin_CNF.pin26 = pin26->CNF[0];
  pin_OE.pin26 = pin26->OE[0];
  pin_ENB.pin26 = pin26->INT_ENB[0];
  pin_LVL.pin26 = pin26->INT_LVL[0];
    
  // Pointer to PINMUX26
  pinmux26 = (uint32_t volatile *)((char *)basePINMUX + PINMUX_26);
  pin_MUX.pin26 = *pinmux26;
    
  // Pointer to PINCFG26
  pincfg26 = (uint32_t volatile *)((char *)baseCFG + CFG_26);
  pin_CFG.pin26 = *pincfg26;
    
  // Pointer to CNF27
  pin27 = (GPIO_CNF volatile *)((char *)baseCNF + CNF_27);
  pin_CNF.pin27 = pin27->CNF[0];
  pin_OE.pin27 = pin27->OE[0];
  pin_ENB.pin27 = pin27->INT_ENB[0];
  pin_LVL.pin27 = pin27->INT_LVL[0];
    
  // Pointer to PINMUX27
  pinmux27 = (uint32_t volatile *)((char *)basePINMUX + PINMUX_27);
  pin_MUX.pin27 = *pinmux27;
    
  // Pointer to PINCFG27
  pincfg27 = (uint32_t volatile *)((char *)baseCFG + CFG_27);
  pin_CFG.pin27 = *pincfg27;
    
  // Pointer to CNF28
  pin28 = (GPIO_CNF volatile *)((char *)baseCNF + CNF_28);
  pin_CNF.pin28 = pin28->CNF[0];
  pin_OE.pin28 = pin28->OE[0];
  pin_ENB.pin28 = pin28->INT_ENB[0];
  pin_LVL.pin28 = pin28->INT_LVL[0];
    
  // Pointer to PINMUX28
  pinmux28 = (uint32_t volatile *)((char *)basePINMUX + PINMUX_28);
  pin_MUX.pin28 = *pinmux28;
    
  // Pointer to PINCFG28
  pincfg28 = (uint32_t volatile *)((char *)baseCFG + CFG_28);
  pin_CFG.pin28 = *pincfg28;
    
  // Pointer to CNF29
  pin29 = (GPIO_CNF volatile *)((char *)baseCNF + CNF_29);
  pin_CNF.pin29 = pin29->CNF[0];
  pin_OE.pin29 = pin29->OE[0];
  pin_ENB.pin29 = pin29->INT_ENB[0];
  pin_LVL.pin29 = pin29->INT_LVL[0];
    
  // Pointer to PINMUX29
  pinmux29 = (uint32_t volatile *)((char *)basePINMUX + PINMUX_29);
  pin_MUX.pin29 = *pinmux29;
    
  // Pointer to PINCFG29
  pincfg29 = (uint32_t volatile *)((char *)baseCFG + CFG_29);
  pin_CFG.pin29 = *pincfg29;
    
  // Pointer to CNF31
  pin31 = (GPIO_CNF volatile *)((char *)baseCNF + CNF_31);
  pin_CNF.pin31 = pin31->CNF[0];
  pin_OE.pin31 = pin31->OE[0];
  pin_ENB.pin31 = pin31->INT_ENB[0];
  pin_LVL.pin31 = pin31->INT_LVL[0];
    
  // Pointer to PINMUX31
  pinmux31 = (uint32_t volatile *)((char *)basePINMUX + PINMUX_31);
  pin_MUX.pin31 = *pinmux31;
    
  // Pointer to PINCFG31
  pincfg31 = (uint32_t volatile *)((char *)baseCFG + CFG_31);
  pin_CFG.pin31 = *pincfg31;
    
  // Pointer to CNF32
  pin32 = (GPIO_CNF volatile *)((char *)baseCNF + CNF_32);
  pin_CNF.pin32 = pin32->CNF[0];
  pin_OE.pin32 = pin32->OE[0];
  pin_ENB.pin32 = pin32->INT_ENB[0];
  pin_LVL.pin32 = pin32->INT_LVL[0];
    
  // Pointer to PINMUX32
  pinmux32 = (uint32_t volatile *)((char *)basePINMUX + PINMUX_32);
  pin_MUX.pin32 = *pinmux32;
   
  // Pointer to PINCFG32
  pincfg32 = (uint32_t volatile *)((char *)baseCFG + CFG_32);
  pin_CFG.pin32 = *pincfg32;
    
  // Pointer to CNF33
  pin33 = (GPIO_CNF volatile *)((char *)baseCNF + CNF_33);
  pin_CNF.pin33 = pin33->CNF[0];
  pin_OE.pin33 =  pin33->OE[0];
  pin_ENB.pin33 =  pin33->INT_ENB[0];
  pin_LVL.pin33 =  pin33->INT_LVL[0];
    
  // Pointer to PINMUX33
  pinmux33 = (uint32_t volatile *)((char *)basePINMUX + PINMUX_33);
  pin_MUX.pin33 = *pinmux33;
    
  // Pointer to PINCFG33
  pincfg33 = (uint32_t volatile *)((char *)baseCFG + CFG_33);
  pin_CFG.pin33 = *pincfg33;
    
  // Pointer to CNF35
  pin35 = (GPIO_CNF volatile *)((char *)baseCNF + CNF_35);
  pin_CNF.pin35 = pin35->CNF[0];
  pin_OE.pin35 = pin35->OE[0];
  pin_ENB.pin35 = pin35->INT_ENB[0];
  pin_LVL.pin35 = pin35->INT_LVL[0];
    
  // Pointer to PINMUX35
  pinmux35 = (uint32_t volatile *)((char *)basePINMUX + PINMUX_35);
  pin_MUX.pin35 = *pinmux35;
    
  // Pointer to PINCFG35
  pincfg35 = (uint32_t volatile *)((char *)baseCFG + CFG_35);
  pin_CFG.pin35 = *pincfg35;
    
  // Pointer to CNF36
  pin36 = (GPIO_CNF volatile *)((char *)baseCNF + CNF_36);
  pin_CNF.pin36 = pin36->CNF[0];
  pin_OE.pin36 = pin36->OE[0];
  pin_ENB.pin36 = pin36->INT_ENB[0];
  pin_LVL.pin36 = pin36->INT_LVL[0];
    
  // Pointer to PINMUX36
  pinmux36 = (uint32_t volatile *)((char *)basePINMUX + PINMUX_36);
  pin_MUX.pin36 = *pinmux36;
    
  // Pointer to PINCFG36
  pincfg36 = (uint32_t volatile *)((char *)baseCFG + CFG_36);
  pin_CFG.pin36 = *pincfg36;
    
  // Pointer to CNF37
  pin37 = (GPIO_CNF volatile *)((char *)baseCNF + CNF_37);
  pin_CNF.pin37 = pin37->CNF[0];
  pin_OE.pin37 = pin37->OE[0];
  pin_ENB.pin37 = pin37->INT_ENB[0];
  pin_LVL.pin37 = pin37->INT_LVL[0];
    
  // Pointer to PINMUX37
  pinmux37 = (uint32_t volatile *)((char *)basePINMUX + PINMUX_37);
  pin_MUX.pin37 = *pinmux37;
    
  // Pointer to PINCFG37
  pincfg37 = (uint32_t volatile *)((char *)baseCFG + CFG_37);
  pin_CFG.pin37 = *pincfg37;
    
  // Pointer to CNF38
  pin38 = (GPIO_CNF volatile *)((char *)baseCNF + CNF_38);
  pin_CNF.pin38 = pin38->CNF[0];
  pin_OE.pin38 = pin38->OE[0];
  pin_ENB.pin38 = pin38->INT_ENB[0];
  pin_LVL.pin38 = pin38->INT_LVL[0];
    
  // Pointer to PINMUX38
  pinmux38 = (uint32_t volatile *)((char *)basePINMUX + PINMUX_38);
  pin_MUX.pin38 = *pinmux38;
    
  // Pointer to PINCFG38
  pincfg38 = (uint32_t volatile *)((char *)baseCFG + CFG_38);
  pin_CFG.pin38 = *pincfg38;
   
  // Pointer to CNF40
  pin40 = (GPIO_CNF volatile *)((char *)baseCNF + CNF_40);
  pin_CNF.pin40 = pin40->CNF[0];
  pin_OE.pin40 = pin40->OE[0];
  pin_ENB.pin40 = pin40->INT_ENB[0];
  pin_LVL.pin40 = pin40->INT_LVL[0];
    
  // Pointer to PINMUX40
  pinmux40 = (uint32_t volatile *)((char *)basePINMUX + PINMUX_40);
  pin_MUX.pin40 = *pinmux40;
    
  // Pointer to PINCFG40
  pincfg40 = (uint32_t volatile *)((char *)baseCFG + CFG_40);
  pin_CFG.pin40 = *pincfg40;
    
  // Pointer to PM3_PWMx
  pinPWM = (GPIO_PWM volatile *)((char *)basePWM + PM3_PWM0);
  pinPWM_Init = *pinPWM;

  // Pointer to APBDEV_PMC_PWR_DET_VAL_0
  apbdev_pmc_pwr_det_val = (uint32_t volatile *)((char *)basePMC + APBDEV_PMC_PWR_DET_VAL_0 + 0x400);

  // Pointer to controller_clk_out_enb_l
  controller_clk_out_enb_l = (uint32_t volatile *)((char *)baseCAR + CLK_RST_CONTROLLER_CLK_OUT_ENB_L_0);
    
  // Pointer to controller_clk_out_enb_h
  controller_clk_out_enb_h = (uint32_t volatile *)((char *)baseCAR + CLK_RST_CONTROLLER_CLK_OUT_ENB_H_0);

  // Pointer to controller_rst_devices_h
  controller_rst_devices_h = (uint32_t volatile *)((char *)baseCAR + CLK_RST_CONTROLLER_RST_DEVICES_H_0);

  // Pointer to clksourcespi1
  clk_source_spi1 = (uint32_t volatile *)((char *)baseCAR + CLK_RST_CONTROLLER_CLK_SOURCE_SPI1_0);

  // Pointer to clksourcespi2
  clk_source_spi2 = (uint32_t volatile *)((char *)baseCAR + CLK_RST_CONTROLLER_CLK_SOURCE_SPI2_0);

  // Pointer to controller_clk_out_enb_h
  controller_clk_out_enb_l_set = (uint32_t volatile *)((char *)baseCAR + CLK_RST_CONTROLLER_CLK_ENB_L_SET_0);

  // Initialize i2c
  i2cInfo[0].state = I2C_CLOSED;
  i2cInfo[1].state = I2C_CLOSED;
    
  // Initialize spi
  SpiInfo[0].state = SPI_CLOSED;
  SpiInfo[1].state = SPI_CLOSED;

  // Global interrupt variable
  global_int = 1;

  // Power Controller it is enabled on boot
  //*controller_clk_out_enb_l |= 0x00000100;
  //*controller_clk_out_enb_l_set |= 0x00000100;

  // Allocating memory for the struct
  for (int j = 0; j < 41; j++) {
    ISRFunc_CFG[j] = calloc (1, sizeof(ISRFunc));
  }
  return status;
}

void gpioTerminate(void){
    
  // Stopping threads
  global_int = 0;

  // Cancelling threads to avoid blocking on read()
  for(int i = 0;i < thread_n; i++) {
    pthread_cancel(callThd[i]);
    //printf("Thread number: %d cancelled\n",i);
  }
    
  //Joining threads
  for(int j = 0;j < thread_n; j++) {
    pthread_join(callThd[j], &status_thread);
    //printf("Thread number: %d joined\n",j);
  }

  // Free allocated memory 
  for (int k = 0; k < 41; k++) {
    free(ISRFunc_CFG[k]);
  }
    
  int pagesize = sysconf(_SC_PAGESIZE);
  // Restoring registers to their previous state

  if ((pin_tracker >> 28) & 1){
    *pinPWM = pinPWM_Init;
  }
    
  if (pin_tracker & 1){ 
    pin3->CNF[0] = pin_CNF.pin3;
    pin3->OE[0] = pin_OE.pin3;
    pin3->INT_ENB[0] = pin_ENB.pin3;
    pin3->INT_LVL[0] = pin_LVL.pin3;
    *pinmux3 = pin_MUX.pin3;
    *pincfg3 = pin_CFG.pin3;
  }
    
  if ((pin_tracker >> 1) & 1){
    pin5->CNF[0] = pin_CNF.pin5;
    pin5->OE[0] = pin_OE.pin5;
    pin5->INT_ENB[0] = pin_ENB.pin5;
    pin5->INT_LVL[0] = pin_LVL.pin5;
    *pinmux5 = pin_MUX.pin5;
    *pincfg5 = pin_CFG.pin5;
  }
    
  if ((pin_tracker >> 2) & 1){
    pin7->CNF[0] = pin_CNF.pin7;
    pin7->OE[0] = pin_OE.pin7;
    pin7->INT_ENB[0] = pin_ENB.pin7;
    pin7->INT_LVL[0] = pin_LVL.pin7;
    *pinmux7 = pin_MUX.pin7;
    *pincfg7 = pin_CFG.pin7;
  }
    
  if ((pin_tracker >> 3) & 1){
    pin8->CNF[0] = pin_CNF.pin8;
    pin8->OE[0] = pin_OE.pin8;
    pin8->INT_ENB[0] = pin_ENB.pin8;
    pin8->INT_LVL[0] = pin_LVL.pin8;
    *pinmux8 = pin_MUX.pin8;
    *pincfg8 = pin_CFG.pin8;
  }
    
  if ((pin_tracker >> 4) & 1){
    pin10->CNF[0] = pin_CNF.pin10;
    pin10->OE[0] = pin_OE.pin10;
    pin10->INT_ENB[0] = pin_ENB.pin10;
    pin10->INT_LVL[0] = pin_LVL.pin10;
    *pinmux10 = pin_MUX.pin10;
    *pincfg10 = pin_CFG.pin10;
  }
    
  if ((pin_tracker >> 5) & 1){
    pin11->CNF[0] = pin_CNF.pin11;
    pin11->OE[0] = pin_OE.pin11;
    pin11->INT_ENB[0] = pin_ENB.pin11;
    pin11->INT_LVL[0] = pin_LVL.pin11;
    *pinmux11 = pin_MUX.pin11;
    *pincfg11 = pin_CFG.pin11;
  }
    
  if ((pin_tracker >> 6) & 1){
    pin12->CNF[0] = pin_CNF.pin12;
    pin12->OE[0] = pin_OE.pin12;
    pin12->INT_ENB[0] = pin_ENB.pin12;
    pin12->INT_LVL[0] = pin_LVL.pin12;
    *pinmux12 = pin_MUX.pin12;
    *pincfg12 = pin_CFG.pin12;
  }

  if (((pin_tracker >> 7) & 1) || ((pin_tracker >> 30) & 1)){
    pin13->CNF[0] = pin_CNF.pin13;
    pin13->OE[0] = pin_OE.pin13;
    pin13->INT_ENB[0] = pin_ENB.pin13;
    pin13->INT_LVL[0] = pin_LVL.pin13;
    *pinmux13 = pin_MUX.pin13;
    *pincfg13 = pin_CFG.pin13;
  }

  if ((pin_tracker >> 8) & 1){
    pin15->CNF[0] = pin_CNF.pin15;
    pin15->OE[0] = pin_OE.pin15;
    pin15->INT_ENB[0] = pin_ENB.pin15;
    pin15->INT_LVL[0] = pin_LVL.pin15;
    *pinmux15 = pin_MUX.pin15;
    *pincfg15 = pin_CFG.pin15;
  }

  if ((pin_tracker >> 9) & 1){
    pin16->CNF[0] = pin_CNF.pin16;
    pin16->OE[0] = pin_OE.pin16;
    pin16->INT_ENB[0] = pin_ENB.pin16;
    pin16->INT_LVL[0] = pin_LVL.pin16;
    *pinmux16 = pin_MUX.pin16;
    *pincfg16 = pin_CFG.pin16;
  }

  if (((pin_tracker >> 10) & 1) || ((pin_tracker >> 30) & 1)){
    pin18->CNF[0] = pin_CNF.pin18;
    pin18->OE[0] = pin_OE.pin18;
    pin18->INT_ENB[0] = pin_ENB.pin18;
    pin18->INT_LVL[0] = pin_LVL.pin18;
    *pinmux18 = pin_MUX.pin18;
    *pincfg18 = pin_CFG.pin18;
  }

  if (((pin_tracker >> 11) & 1) || ((pin_tracker >> 29) & 1)){
    pin19->CNF[0] = pin_CNF.pin19;
    pin19->OE[0] = pin_OE.pin19;
    pin19->INT_ENB[0] = pin_ENB.pin19;
    pin19->INT_LVL[0] = pin_LVL.pin19;
    *pinmux19 = pin_MUX.pin19;
    *pincfg19 = pin_CFG.pin19;
  }

  if (((pin_tracker >> 12) & 1) || ((pin_tracker >> 29) & 1)){
    pin21->CNF[0] = pin_CNF.pin21;
    pin21->OE[0] = pin_OE.pin21;
    pin21->INT_ENB[0] = pin_ENB.pin21;
    pin21->INT_LVL[0] = pin_LVL.pin21;
    *pinmux21 = pin_MUX.pin21;
    *pincfg21 = pin_CFG.pin21;
  }

  if (((pin_tracker >> 13) & 1) || ((pin_tracker >> 30) & 1)){
    pin22->CNF[0] = pin_CNF.pin22;
    pin22->OE[0] = pin_OE.pin22;
    pin22->INT_ENB[0] = pin_ENB.pin22;
    pin22->INT_LVL[0] = pin_LVL.pin22;
    *pinmux22 = pin_MUX.pin22;
    *pincfg22 = pin_CFG.pin22;
  }

  if (((pin_tracker >> 14) & 1) || ((pin_tracker >> 29) & 1)){
    pin23->CNF[0] = pin_CNF.pin23;
    pin23->OE[0] = pin_OE.pin23;
    pin23->INT_ENB[0] = pin_ENB.pin23;
    pin23->INT_LVL[0] = pin_LVL.pin23;
    *pinmux23 = pin_MUX.pin23;
    *pincfg23 = pin_CFG.pin23;
  }

  if (((pin_tracker >> 15) & 1) || ((pin_tracker >> 29) & 1)){
    pin24->CNF[0] = pin_CNF.pin24;
    pin24->OE[0] = pin_OE.pin24;
    pin24->INT_ENB[0] = pin_ENB.pin24;
    pin24->INT_LVL[0] = pin_LVL.pin24;
    *pinmux24 = pin_MUX.pin24;
    *pincfg24 = pin_CFG.pin24;
  }

  if ((pin_tracker >> 16) & 1){
    pin26->CNF[0] = pin_CNF.pin26;
    pin26->OE[0] = pin_OE.pin26;
    pin26->INT_ENB[0] = pin_ENB.pin26;
    pin26->INT_LVL[0] = pin_LVL.pin26;
    *pinmux26 = pin_MUX.pin26;
    *pincfg26 = pin_CFG.pin26;
  }

  if ((pin_tracker >> 17) & 1){
    pin27->CNF[0] = pin_CNF.pin27;
    pin27->OE[0] = pin_OE.pin27;
    pin27->INT_ENB[0] = pin_ENB.pin27;
    pin27->INT_LVL[0] = pin_LVL.pin27;
    *pinmux27 = pin_MUX.pin27;
    *pincfg27 = pin_CFG.pin27;
  }

  if ((pin_tracker >> 18) & 1){
    pin28->CNF[0] = pin_CNF.pin28;
    pin28->OE[0] = pin_OE.pin28;
    pin28->INT_ENB[0] = pin_ENB.pin28;
    pin28->INT_LVL[0] = pin_LVL.pin28;
    *pinmux28 = pin_MUX.pin28;
    *pincfg28 = pin_CFG.pin28;
  }

  if ((pin_tracker >> 19) & 1){
    pin29->CNF[0] = pin_CNF.pin29;
    pin29->OE[0] = pin_OE.pin29;
    pin29->INT_ENB[0] = pin_ENB.pin29;
    pin29->INT_LVL[0] = pin_LVL.pin29;
    *pinmux29 = pin_MUX.pin29;
    *pincfg29 = pin_CFG.pin29;
  }

  if ((pin_tracker >> 20) & 1){
    pin31->CNF[0] = pin_CNF.pin31;
    pin31->OE[0] = pin_OE.pin31;
    pin31->INT_ENB[0] = pin_ENB.pin31;
    pin31->INT_LVL[0] = pin_LVL.pin31;
    *pinmux31 = pin_MUX.pin31;
    *pincfg31 = pin_CFG.pin31;
  }

  if (((pin_tracker >> 21) & 1) || ((pin_tracker >> 28) & 1)){
    pin32->CNF[0] = pin_CNF.pin32;
    pin32->OE[0] = pin_OE.pin32;
    pin32->INT_ENB[0] = pin_ENB.pin32;
    pin32->INT_LVL[0] = pin_LVL.pin32;
    *pinmux32 = pin_MUX.pin32;
    *pincfg32 = pin_CFG.pin32;
  }

  if (((pin_tracker >> 22) & 1) || ((pin_tracker >> 28) & 1)){
    pin33->CNF[0] = pin_CNF.pin33;
    pin33->OE[0] = pin_OE.pin33;
    pin33->INT_ENB[0] = pin_ENB.pin33;
    pin33->INT_LVL[0] = pin_LVL.pin33;
    *pinmux33 = pin_MUX.pin33;
    *pincfg33 = pin_CFG.pin33;
  }

  if ((pin_tracker >> 23) & 1){
    pin35->CNF[0] = pin_CNF.pin35;
    pin35->OE[0] = pin_OE.pin35;
    pin35->INT_ENB[0] = pin_ENB.pin35;
    pin35->INT_LVL[0] = pin_LVL.pin35;
    *pinmux35 = pin_MUX.pin35;
    *pincfg35 = pin_CFG.pin35;
  }

  if ((pin_tracker >> 24) & 1){
    pin36->CNF[0] = pin_CNF.pin36;
    pin36->OE[0] = pin_OE.pin36;
    pin36->INT_ENB[0] = pin_ENB.pin36;
    pin36->INT_LVL[0] = pin_LVL.pin36;
    *pinmux36 = pin_MUX.pin36;
    *pincfg36 = pin_CFG.pin36;
  }

  if (((pin_tracker >> 25) & 1) || ((pin_tracker >> 30) & 1)){
    pin37->CNF[0] = pin_CNF.pin37;
    pin37->OE[0] = pin_OE.pin37;
    pin37->INT_ENB[0] = pin_ENB.pin37;
    pin37->INT_LVL[0] = pin_LVL.pin37;
    *pinmux37 = pin_MUX.pin37;
    *pincfg37 = pin_CFG.pin37;
  }

  if ((pin_tracker >> 26) & 1){
    pin38->CNF[0] = pin_CNF.pin38;
    pin38->OE[0] = pin_OE.pin38;
    pin38->INT_ENB[0] = pin_ENB.pin38;
    pin38->INT_LVL[0] = pin_LVL.pin38;
    *pinmux38 = pin_MUX.pin38;
    *pincfg38 = pin_CFG.pin38;
  }

  if ((pin_tracker >> 27) & 1){
    pin40->CNF[0] = pin_CNF.pin40;
    pin40->OE[0] = pin_OE.pin40;
    pin40->INT_ENB[0] = pin_ENB.pin40;
    pin40->INT_LVL[0] = pin_LVL.pin40;
    *pinmux40 = pin_MUX.pin40;
    *pincfg40 = pin_CFG.pin40;
  }
	
  // Ummapping CNF registers
  munmap(baseCNF, pagesize);
    
  // Ummapping PINMUX registers
  munmap(basePINMUX, pagesize);
	
  // Ummapping CFG registers 
  munmap(baseCFG, pagesize);
	
  // Ummapping PWM registers 
  munmap(basePWM, pagesize);
	
  // Ummapping CAR registers 
  munmap(baseCAR, pagesize);

  // Ummapping PMC registers 
  munmap(basePMC, pagesize);
  
  // close /dev/mem 
  close(fd_GPIO);
}

int gpioSetMode(unsigned gpio, unsigned mode){
    
  int status = 1;
  if (mode == 0) {
    switch (gpio){
		
    case 3:
      *pinmux3 = PINMUX_IN;
      *pincfg3 = CFG_IN;
      pin3->CNF[0] |= 0x00000008;
      pin3->OE[0] &= ~(0x00000008);
      pin_tracker |= 1;
      break;
    case 5:
      *pinmux5 = PINMUX_IN;
      *pincfg5 = CFG_IN;
      pin5->CNF[0] |= 0x00000004;
      pin5->OE[0] &= ~(0x00000004);
      pin_tracker |= (1 << 1);
      break;
    case 7:
      *pinmux7 = PINMUX_IN;
      *pincfg7 = CFG_IN;
      pin7->CNF[0] |= 0x00000001;
      pin7->OE[0] &= ~(0x00000001);
      pin_tracker |= (1 << 2);
      break;
    case 8:
      *pinmux8 = PINMUX_IN;
      *pincfg8 = CFG_IN;
      pin8->CNF[0] |= 0x00000001;
      pin8->OE[0] &= ~(0x00000001);
      pin_tracker |= (1 << 3);
      break;
    case 10:
      *pinmux10 = PINMUX_IN;
      *pincfg10 = CFG_IN;
      pin10->CNF[0] |= 0x00000002;
      pin10->OE[0] &= ~(0x00000002);
      pin_tracker |= (1 << 4);
      break;
    case 11:
      *pinmux11 = PINMUX_IN;
      *pincfg11 = CFG_IN;
      pin11->CNF[0] |= 0x00000004;
      pin11->OE[0] &= ~(0x00000004);
      pin_tracker |= (1 << 5);
      break;
    case 12:
      *pinmux12 = PINMUX_IN;
      *pincfg12 = CFG_IN;
      pin12->CNF[0] |= 0x00000080;
      pin12->OE[0] &= ~(0x00000080);
      pin_tracker |= (1 << 6);
      break;
    case 13:
      *pinmux13 = PINMUX_IN;
      *pincfg13 = CFG_IN;
      pin13->CNF[0] |= 0x00000040;
      pin13->OE[0] &= ~(0x00000040);
      pin_tracker |= (1 << 7);
      break;
    case 15:
      *pinmux15 = PINMUX_IN;
      *pincfg15 = CFG_IN;
      pin15->CNF[0] |= 0x00000004;
      pin15->OE[0] &= ~(0x00000004);
      pin_tracker |= (1 << 8);
      break;
    case 16:
      *pinmux16 = PINMUX_IN;
      *pincfg16 = CFG_IN;
      pin16->CNF[0] |= 0x00000001;
      pin16->OE[0] &= ~(0x00000001);
      pin_tracker |= (1 << 9);
      break;
    case 18:
      *pinmux18 = PINMUX_IN;
      *pincfg18 = CFG_IN;
      pin18->CNF[0] |= 0x00000080;
      pin18->OE[0] &= ~(0x00000080);
      pin_tracker |= (1 << 10);
      break;
    case 19:
      *pinmux19 = PINMUX_IN;
      *pincfg19 = CFG_IN;
      pin19->CNF[0] |= 0x00000001;
      pin19->OE[0] &= ~(0x00000001);
      pin_tracker |= (1 << 11);
      break;
    case 21:
      *pinmux21 = PINMUX_IN;
      *pincfg21 = CFG_IN;
      pin21->CNF[0] |= 0x00000002;
      pin21->OE[0] &= ~(0x00000002);
      pin_tracker |= (1 << 12);
      break;
    case 22:
      *pinmux22 = PINMUX_IN;
      *pincfg22 = CFG_IN;
      pin22->CNF[0] |= 0x00000020;
      pin22->OE[0] &= ~(0x00000020);
      pin_tracker |= (1 << 13);
      break;
    case 23:
      *pinmux23 = PINMUX_IN;
      *pincfg23 = CFG_IN;
      pin23->CNF[0] |= 0x00000004;
      pin23->OE[0] &= ~(0x00000004);
      pin_tracker |= (1 << 14);
      break;
    case 24:
      *pinmux24 = PINMUX_IN;
      *pincfg24 = CFG_IN;
      pin24->CNF[0] |= 0x00000008;
      pin24->OE[0] &= ~(0x00000008);
      pin_tracker |= (1 << 15);
      break;
    case 26:
      *pinmux26 = PINMUX_IN;
      *pincfg26 = CFG_IN;
      pin26->CNF[0] |= 0x00000010;
      pin26->OE[0] &= ~(0x00000010);
      pin_tracker |= (1 << 16);
      break;
    case 27:
      *pinmux27 = PINMUX_IN;
      *pincfg27 = CFG_IN;
      pin27->CNF[0] |= 0x00000001;
      pin27->OE[0] &= ~(0x00000001);
      pin_tracker |= (1 << 17);
      break;
    case 28:
      *pinmux28 = PINMUX_IN;
      *pincfg28 = CFG_IN;
      pin28->CNF[0] |= 0x00000002;
      pin28->OE[0] &= ~(0x00000002);
      pin_tracker |= (1 << 18);
      break;
    case 29:
      *pinmux29 = PINMUX_IN;
      *pincfg29 = CFG_IN;
      pin29->CNF[0] |= 0x00000020;
      pin29->OE[0] &= ~(0x00000020);
      pin_tracker |= (1 << 19);
      break;
    case 31:
      *pinmux31 = PINMUX_IN;
      *pincfg31 = CFG_IN;
      pin31->CNF[0] |= 0x00000001;
      pin31->OE[0] &= ~(0x00000001);
      pin_tracker |= (1 << 20);
      break;
    case 32:
      *pinmux32 = PINMUX_IN;
      *pincfg32 = CFG_IN;
      pin32->CNF[0] |= 0x00000001;
      pin32->OE[0] &= ~(0x00000001);
      pin_tracker |= (1 << 21);
      break;
    case 33:
      *pinmux33 = PINMUX_IN;
      *pincfg33 = CFG_IN;
      pin33->CNF[0] |= 0x00000040;
      pin33->OE[0] &= ~(0x00000040);
      pin_tracker |= (1 << 22);
      break;
    case 35:
      *pinmux35 = PINMUX_IN;
      *pincfg35 = CFG_IN;
      pin35->CNF[0] |= 0x00000010;
      pin35->OE[0] &= ~(0x00000010);
      pin_tracker |= (1 << 23);
      break;
    case 36:
      *pinmux36 = PINMUX_IN;
      *pincfg36 = CFG_IN;
      pin36->CNF[0] |= 0x00000008;
      pin36->OE[0] &= ~(0x00000008);
      pin_tracker |= (1 << 24);
      break;
    case 37:
      *pinmux37 = PINMUX_IN;
      *pincfg37 = CFG_IN;
      pin37->CNF[0] |= 0x00000010;
      pin37->OE[0] &= ~(0x00000010);
      pin_tracker |= (1 << 25);
      break;
    case 38:
      *pinmux38 = PINMUX_IN;
      *pincfg38 = CFG_IN;
      pin38->CNF[0] |= 0x00000020;
      pin38->OE[0] &= ~(0x00000020);
      pin_tracker |= (1 << 26);
      break;
    case 40:
      *pinmux40 = PINMUX_IN;
      *pincfg40 = CFG_IN;
      pin40->CNF[0] |= 0x00000040;
      pin40->OE[0] &= ~(0x00000040);
      pin_tracker |= (1 << 27);
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
      pin_tracker |= 1;
      break;
    case 5:
      *pinmux5 = PINMUX_OUT;
      *pincfg5 = CFG_OUT;
      pin5->CNF[0] |= 0x00000004;
      pin5->OE[0] |= 0x00000004;
      pin_tracker |= (1 << 1);
      break;
    case 7:
      *pinmux7 = PINMUX_OUT;
      *pincfg7 = CFG_OUT;
      pin7->CNF[0] |= 0x00000001;
      pin7->OE[0] |= 0x00000001;
      pin_tracker |= (1 << 2);
      break;
    case 8:
      *pinmux8 = PINMUX_OUT;
      *pincfg8 = CFG_OUT;
      pin8->CNF[0] |= 0x00000001;
      pin8->OE[0] |= 0x00000001;
      pin_tracker |= (1 << 3);
      break;
    case 10:
      *pinmux10 = PINMUX_OUT;
      *pincfg10 = CFG_OUT;
      pin10->CNF[0] |= 0x00000002;
      pin10->OE[0] |= 0x00000002;
      pin_tracker |= (1 << 4);
      break;
    case 11:
      *pinmux11 = PINMUX_OUT;
      *pincfg11 = CFG_OUT;
      pin11->CNF[0] |= 0x00000004;
      pin11->OE[0] |= 0x00000004;
      pin_tracker |= (1 << 5);
      break;
    case 12:
      *pinmux12 = PINMUX_OUT;
      *pincfg12 = CFG_OUT;
      pin12->CNF[0] |= 0x00000080;
      pin12->OE[0] |= 0x00000080;
      pin_tracker |= (1 << 6);
      break;
    case 13:
      *pinmux13 = PINMUX_OUT1;
      *pincfg13 = CFG_OUT1;
      pin13->CNF[0] |= 0x00000040;
      pin13->OE[0] |= 0x00000040;
      pin_tracker |= (1 << 7);
      break;
    case 15:
      *pinmux15 = PINMUX_OUT;
      *pincfg15 = CFG_OUT;
      pin15->CNF[0] |= 0x00000004;
      pin15->OE[0] |= 0x00000004;
      pin_tracker |= (1 << 8);
      break;
    case 16:
      *pinmux16 = PINMUX_OUT1;
      *pincfg16 = CFG_OUT1;
      pin16->CNF[0] |= 0x00000001;
      pin16->OE[0] |= 0x00000001;
      pin_tracker |= (1 << 9);
      break;
    case 18:
      *pinmux18 = PINMUX_OUT1;
      *pincfg18 = CFG_OUT1;
      pin18->CNF[0] |= 0x00000080;
      pin18->OE[0] |= 0x00000080;
      pin_tracker |= (1 << 10);
      break;
    case 19:
      *pinmux19 = PINMUX_OUT1;
      *pincfg19 = CFG_OUT1;
      pin19->CNF[0] |= 0x00000001;
      pin19->OE[0] |= 0x00000001;
      pin_tracker |= (1 << 11);
      break;
    case 21:
      *pinmux21 = PINMUX_OUT1;
      *pincfg21 = CFG_OUT1;
      pin21->CNF[0] |= 0x00000002;
      pin21->OE[0] |= 0x00000002;
      pin_tracker |= (1 << 12);
      break;
    case 22:
      *pinmux22 = PINMUX_OUT1;
      *pincfg22 = CFG_OUT1;
      pin22->CNF[0] |= 0x00000020;
      pin22->OE[0] |= 0x00000020;
      pin_tracker |= (1 << 13);
      break;
    case 23:
      *pinmux23 = PINMUX_OUT1;
      *pincfg23 = CFG_OUT1;
      pin23->CNF[0] |= 0x00000004;
      pin23->OE[0] |= 0x00000004;
      pin_tracker |= (1 << 14);
      break;
    case 24:
      *pinmux24 = PINMUX_OUT1;
      *pincfg24 = CFG_OUT1;
      pin24->CNF[0] |= 0x00000008;
      pin24->OE[0] |= 0x00000008;
      pin_tracker |= (1 << 15);
      break;
    case 26:
      *pinmux26 = PINMUX_OUT1;
      *pincfg26 = CFG_OUT1;
      pin26->CNF[0] |= 0x00000010;
      pin26->OE[0] |= 0x00000010;
      pin_tracker |= (1 << 16);
      break;
    case 27:
      *pinmux27 = PINMUX_OUT;
      *pincfg27 = CFG_OUT;
      pin27->CNF[0] |= 0x00000001;
      pin27->OE[0] |= 0x00000001;
      pin_tracker |= (1 << 17);
      break;
    case 28:
      *pinmux28 = PINMUX_OUT;
      *pincfg28 = CFG_OUT;
      pin28->CNF[0] |= 0x00000002;
      pin28->OE[0] |= 0x00000002;
      pin_tracker |= (1 << 18);
      break;
    case 29:
      *pinmux29 = PINMUX_OUT;
      *pincfg29 = CFG_OUT;
      pin29->CNF[0] |= 0x00000020;
      pin29->OE[0] |= 0x00000020;
      pin_tracker |= (1 << 19);
      break;
    case 31:
      *pinmux31 = PINMUX_OUT;
      *pincfg31 = CFG_OUT;
      pin31->CNF[0] |= 0x00000001;
      pin31->OE[0] |= 0x00000001;
      pin_tracker |= (1 << 20);
      break;
    case 32:
      *pinmux32 = PINMUX_OUT;
      *pincfg32 = CFG_OUT;
      pin32->CNF[0] |= 0x00000001;
      pin32->OE[0] |= 0x00000001;
      pin_tracker |= (1 << 21);
      break;
    case 33:
      *pinmux33 = PINMUX_OUT;
      *pincfg33 = CFG_OUT;
      pin33->CNF[0] |= 0x00000040;
      pin33->OE[0] |= 0x00000040;
      pin_tracker |= (1 << 22);
      break;
    case 35:
      *pinmux35 = PINMUX_OUT;
      *pincfg35 = CFG_OUT;
      pin35->CNF[0] |= 0x00000010;
      pin35->OE[0] |= 0x00000010;
      pin_tracker |= (1 << 23);
      break;
    case 36:
      *pinmux36 = PINMUX_OUT;
      *pincfg36 = CFG_OUT;
      pin36->CNF[0] |= 0x00000008;
      pin36->OE[0] |= 0x00000008;
      pin_tracker |= (1 << 24);
      break;
    case 37:
      *pinmux37 = PINMUX_OUT1;
      *pincfg37 = CFG_OUT1;
      pin37->CNF[0] |= 0x00000010;
      pin37->OE[0] |= 0x00000010;
      pin_tracker |= (1 << 25);
      break;
    case 38:
      *pinmux38 = PINMUX_OUT;
      *pincfg38 = CFG_OUT;
      pin38->CNF[0] |= 0x00000020;
      pin38->OE[0] |= 0x00000020;
      pin_tracker |= (1 << 26);
      break;
    case 40:
      *pinmux40 = PINMUX_OUT;
      *pincfg40 = CFG_OUT;
      pin40->CNF[0] |= 0x00000040;
      pin40->OE[0] |= 0x00000040;
      pin_tracker |= (1 << 27);
      break;
    default:
      status = -2;
      printf("Only gpio numbers from 3 to 40 are accepted, this function will only write the level on the Jetson Nano header pins,\n");
      printf("numbered as the header pin numbers e.g. AUD_MCLK is pin header number 7\n");
    }
		
  }
  else {printf("Only modes allowed are JET_INPUT and JET_OUTPUT\n");
    status = -3;
  }
  return status;	
}

int gpioRead(unsigned gpio){
    
  int level = 0;
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

int gpioWrite(unsigned gpio, unsigned level){
    
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
      status = -2;
      printf("Only gpio numbers from 3 to 40 are accepted, this function will only read the level of the Jetson Nano header pins,\n");
      printf("numbered as the header pin numbers e.g. AUD_MCLK is pin header number 7\n");
    }
  }
  else {printf("Only levels 0 or 1 are allowed\n");
    status = -3;
  }
  return status;
}

void *callback(void *arg){
    
  ISRFunc *int_struct = (ISRFunc *) arg;
  int edge = int_struct->edge;
  unsigned gpio_offset = int_struct->gpio_offset;
  uint64_t *timestamp =  int_struct->timestamp;
  *timestamp = 0;
  unsigned debounce = int_struct->debounce;
  int fd;
  int ret;
  struct gpioevent_request req;
  struct gpioevent_data event;
    
  fd = open("/dev/gpiochip0", 0);
  if (fd < 0) {
    printf( "Bad handle (%d)\n", fd);
    pthread_exit(NULL);	
  }

  req.lineoffset = gpio_offset;
  req.handleflags = GPIOHANDLE_REQUEST_INPUT;
  req.eventflags = edge;
  //strncpy(req.consumer_label, "gpio_event", sizeof(req.consumer_label) - 1);
    
  ret = ioctl(fd, GPIO_GET_LINEEVENT_IOCTL, &req);
  if (ret == -1) {
    ret = -errno;
    printf("Failed to issue GET EVENT ""IOCTL (%d)\n",ret);
    close(fd);
    pthread_exit(NULL);
  }
  close(fd);
  while (global_int){
    ret = read(req.fd, &event, sizeof(event));
    if ((ret == -1) || (ret != sizeof(event))) {
      ret = -errno;
      printf("Failed to read event (%d)\n", ret);
      break;
    }
        
    if ((event.timestamp - *timestamp) > (debounce*100)){
      *timestamp = event.timestamp;
    }
    else{
      event.id = 0x00;
    }
        
    switch (event.id) {
    case GPIOEVENT_EVENT_RISING_EDGE:
      int_struct->f();
      break;
    case GPIOEVENT_EVENT_FALLING_EDGE:
      int_struct->f();
      break;
    default:
      //Do nothing
      break;
    }
  }
  pthread_exit(NULL);
}
    
int gpioSetISRFunc(unsigned gpio, unsigned edge, unsigned debounce, unsigned long *timestamp, void (*f)()){
    
  int status = 1;
  unsigned x = 0;
  unsigned gpio_offset = 0;

  if (debounce < 0 || debounce > 1000){
    printf( "Debounce setting should be a number between 0 and 1000 useconds\n");
    status = -1;
  }

  if (edge == RISING_EDGE || edge == FALLING_EDGE || edge == EITHER_EDGE){

    switch (gpio){
		
    case 3:
      x = 0x00000008;
      pin3->INT_LVL[0] = (edge == 1 ? (pin3->INT_LVL[0] | (x+x*0x100)):(edge == 2 ? ((pin3->INT_LVL[0] | x*0x100) & ~x):pin3->INT_LVL[0] | (x*0x100+x*0x10000)));
      pin3->INT_ENB[0] |= x;
      pin3->INT_CLR[0] |= x;
      gpio_offset = 75;
      break;
    case 5:
      x = 0x00000004;
      pin5->INT_LVL[0] = (edge == 1 ? (pin5->INT_LVL[0] | (x+x*0x100)):(edge == 0 ? ((pin5->INT_LVL[0] | x*0x100) & ~x):pin5->INT_LVL[0] | (x*0x100+x*0x10000)));
      pin5->INT_ENB[0] |= x;
      pin5->INT_CLR[0] |= x;
      gpio_offset = 74;
      break;
    case 7:
      x =  0x00000001;
      pin7->INT_LVL[0] = (edge == 1 ? (pin7->INT_LVL[0] | (x+x*0x100)):(edge == 0 ? ((pin7->INT_LVL[0] | x*0x100) & ~x):pin7->INT_LVL[0] | (x*0x100+x*0x10000)));
      pin7->INT_ENB[0] |= x;
      pin7->INT_CLR[0] |= x;
      gpio_offset = 216;
      break;
    case 8:
      x =  0x00000001;
      pin8->INT_LVL[0] = (edge == 1 ? (pin8->INT_LVL[0] | (x+x*0x100)):(edge == 0 ? ((pin8->INT_LVL[0] | x*0x100) & ~x):pin8->INT_LVL[0] | (x*0x100+x*0x10000)));
      pin8->INT_ENB[0] |= x;
      pin8->INT_CLR[0] |= x;
      gpio_offset = 48;
      break;
    case 10:
      x = 0x00000002;
      pin10->INT_LVL[0] = (edge == 1 ? (pin10->INT_LVL[0] | (x+x*0x100)):(edge == 0 ? ((pin10->INT_LVL[0] | x*0x100) & ~x):pin10->INT_LVL[0] | (x*0x100+x*0x10000)));
      pin10->INT_ENB[0] |= x;
      pin10->INT_CLR[0] |= x;
      gpio_offset = 49;
      break;
    case 11:
      x = 0x00000004;
      pin11->INT_LVL[0] = (edge == 1 ? (pin11->INT_LVL[0] | (x+x*0x100)):(edge == 0 ? ((pin11->INT_LVL[0] | x*0x100) & ~x):pin11->INT_LVL[0] | (x*0x100+x*0x10000)));
      pin11->INT_ENB[0] |= x;
      pin11->INT_CLR[0] |= x;
      gpio_offset = 50;
      break;
    case 12:
      x = 0x00000080;
      pin12->INT_LVL[0] = (edge == 1 ? (pin12->INT_LVL[0] | (x+x*0x100)):(edge == 0 ? ((pin12->INT_LVL[0] | x*0x100) & ~x):pin12->INT_LVL[0] | (x*0x100+x*0x10000)));
      pin12->INT_ENB[0] |= x;
      pin12->INT_CLR[0] |= x;
      gpio_offset = 79;
      break;
    case 13:
      x = 0x00000040;
      pin13->INT_LVL[0] = (edge == 1 ? (pin13->INT_LVL[0] | (x+x*0x100)):(edge == 0 ? ((pin13->INT_LVL[0] | x*0x100) & ~x):pin13->INT_LVL[0] | (x*0x100+x*0x10000)));
      pin13->INT_ENB[0] |= x;
      pin13->INT_CLR[0] |= x;
      gpio_offset = 14;
      break;
    case 15:
      x = 0x00000004;
      pin15->INT_LVL[0] = (edge == 1 ? (pin15->INT_LVL[0] | (x+x*0x100)):(edge == 0 ? ((pin15->INT_LVL[0] | x*0x100) & ~x):pin15->INT_LVL[0] | (x*0x100+x*0x10000)));
      pin15->INT_ENB[0] |= x;
      pin15->INT_CLR[0] |= x;
      gpio_offset = 194;
      break;
    case 16:
      x = 0x00000001;
      pin16->INT_LVL[0] = (edge == 1 ? (pin16->INT_LVL[0] | (x+x*0x100)):(edge == 0 ? ((pin16->INT_LVL[0] | x*0x100) & ~x):pin16->INT_LVL[0] | (x*0x100+x*0x10000)));
      pin16->INT_ENB[0] |= x;
      pin16->INT_CLR[0] |= x;
      gpio_offset = 232;
      break;
    case 18:
      x = 0x00000080;
      pin18->INT_LVL[0] = (edge == 1 ? (pin18->INT_LVL[0] | (x+x*0x100)):(edge == 0 ? ((pin18->INT_LVL[0] | x*0x100) & ~x):pin18->INT_LVL[0] | (x*0x100+x*0x10000)));
      pin18->INT_ENB[0] |= x;
      pin18->INT_CLR[0] |= x;
      gpio_offset = 15;
      break;
    case 19:
      x = 0x00000001;
      pin19->INT_LVL[0] = (edge == 1 ? (pin19->INT_LVL[0] | (x+x*0x100)):(edge == 0 ? ((pin19->INT_LVL[0] | x*0x100) & ~x):pin19->INT_LVL[0] | (x*0x100+x*0x10000)));
      pin19->INT_ENB[0] |= x;
      pin19->INT_CLR[0] |= x;
      gpio_offset = 16;
      break;
    case 21:
      x = 0x00000002;
      pin21->INT_LVL[0] = (edge == 1 ? (pin21->INT_LVL[0] | (x+x*0x100)):(edge == 0 ? ((pin21->INT_LVL[0] | x*0x100) & ~x):pin21->INT_LVL[0] | (x*0x100+x*0x10000)));
      pin21->INT_ENB[0] |= x;
      pin21->INT_CLR[0] |= x;
      gpio_offset = 17;
      break;
    case 22:
      x = 0x00000020;
      pin22->INT_LVL[0] = (edge == 1 ? (pin22->INT_LVL[0] | (x+x*0x100)):(edge == 0 ? ((pin22->INT_LVL[0] | x*0x100) & ~x):pin22->INT_LVL[0] | (x*0x100+x*0x10000)));
      pin22->INT_ENB[0] |= x;
      pin22->INT_CLR[0] |= x;
      gpio_offset = 13;
      break;
    case 23:
      x = 0x00000004;
      pin23->INT_LVL[0] = (edge == 1 ? (pin23->INT_LVL[0] | (x+x*0x100)):(edge == 0 ? ((pin23->INT_LVL[0] | x*0x100) & ~x):pin23->INT_LVL[0] | (x*0x100+x*0x10000)));
      pin23->INT_ENB[0] |= x;
      pin23->INT_CLR[0] |= x;
      gpio_offset = 18;
      break;
    case 24:
      x = 0x00000008;
      pin24->INT_LVL[0] = (edge == 1 ? (pin24->INT_LVL[0] | (x+x*0x100)):(edge == 0 ? ((pin24->INT_LVL[0] | x*0x100) & ~x):pin24->INT_LVL[0] | (x*0x100+x*0x10000)));
      pin24->INT_ENB[0] |= x;
      pin24->INT_CLR[0] |= x;
      gpio_offset = 19;
      break;
    case 26:
      x = 0x00000010;
      pin26->INT_LVL[0] = (edge == 1 ? (pin26->INT_LVL[0] | (x+x*0x100)):(edge == 0 ? ((pin26->INT_LVL[0] | x*0x100) & ~x):pin26->INT_LVL[0] | (x*0x100+x*0x10000)));
      pin26->INT_ENB[0] |= x;
      pin26->INT_CLR[0] |= x;
      gpio_offset = 20;
      break;
    case 27:
      x = 0x00000001;
      pin27->INT_LVL[0] = (edge == 1 ? (pin27->INT_LVL[0] | (x+x*0x100)):(edge == 0 ? ((pin27->INT_LVL[0] | x*0x100) & ~x):pin27->INT_LVL[0] | (x*0x100+x*0x10000)));
      pin27->INT_ENB[0] |= x;
      pin27->INT_CLR[0] |= x;
      gpio_offset = 72;
      break;
    case 28:
      x = 0x00000002;
      pin28->INT_LVL[0] = (edge == 1 ? (pin28->INT_LVL[0] | (x+x*0x100)):(edge == 0 ? ((pin28->INT_LVL[0] | x*0x100) & ~x):pin28->INT_LVL[0] | (x*0x100+x*0x10000)));
      pin28->INT_ENB[0] |= x;
      pin28->INT_CLR[0] |= x;
      gpio_offset = 73;
      break;
    case 29:
      x =  0x00000020;
      pin29->INT_LVL[0] = (edge == 1 ? (pin29->INT_LVL[0] | (x+x*0x100)):(edge == 0 ? ((pin29->INT_LVL[0] | x*0x100) & ~x):pin29->INT_LVL[0] | (x*0x100+x*0x10000)));
      pin29->INT_ENB[0] |= x;
      pin29->INT_CLR[0] |= x;
      gpio_offset = 149;
      break;
    case 31:
      x =  0x00000001;
      pin31->INT_LVL[0] = (edge == 1 ? (pin31->INT_LVL[0] | (x+x*0x100)):(edge == 0 ? ((pin31->INT_LVL[0] | x*0x100) & ~x):pin31->INT_LVL[0] | (x*0x100+x*0x10000)));
      pin31->INT_ENB[0] |= x;
      pin31->INT_CLR[0] |= x;
      gpio_offset = 200;
      break;
    case 32:
      x =  0x00000001;
      pin32->INT_LVL[0] = (edge == 1 ? (pin32->INT_LVL[0] | (x+x*0x100)):(edge == 0 ? ((pin32->INT_LVL[0] | x*0x100) & ~x):pin32->INT_LVL[0] | (x*0x100+x*0x10000)));
      pin32->INT_ENB[0] |= x;
      pin32->INT_CLR[0] |= x;
      gpio_offset = 168;
      break;
    case 33:
      x = 0x00000040;
      pin33->INT_LVL[0] = (edge == 1 ? (pin33->INT_LVL[0] | (x+x*0x100)):(edge == 0 ? ((pin33->INT_LVL[0] | x*0x100) & ~x):pin33->INT_LVL[0] | (x*0x100+x*0x10000)));
      pin33->INT_ENB[0] |= x;
      pin33->INT_CLR[0] |= x;
      gpio_offset = 38;
      break;
    case 35:
      x = 0x00000010;
      pin35->INT_LVL[0] = (edge == 1 ? (pin35->INT_LVL[0] | (x+x*0x100)):(edge == 0 ? ((pin35->INT_LVL[0] | x*0x100) & ~x):pin35->INT_LVL[0] | (x*0x100+x*0x10000)));
      pin35->INT_ENB[0] |= x;
      pin35->INT_CLR[0] |= x;
      gpio_offset = 76;
      break;
    case 36:
      x = 0x00000008;
      pin36->INT_LVL[0] = (edge == 1 ? (pin36->INT_LVL[0] | (x+x*0x100)):(edge == 0 ? ((pin36->INT_LVL[0] | x*0x100) & ~x):pin36->INT_LVL[0] | (x*0x100+x*0x10000)));
      pin36->INT_ENB[0] |= x;
      pin36->INT_CLR[0] |= x;
      gpio_offset = 51;
      break;
    case 37:
      x = 0x00000010;
      pin37->INT_LVL[0] = (edge == 1 ? (pin37->INT_LVL[0] | (x+x*0x100)):(edge == 0 ? ((pin37->INT_LVL[0] | x*0x100) & ~x):pin37->INT_LVL[0] | (x*0x100+x*0x10000)));
      pin37->INT_ENB[0] |= x;
      pin37->INT_CLR[0] |= x;
      gpio_offset = 12;
      break;
    case 38:
      x =  0x00000020;
      pin38->INT_LVL[0] = (edge == 1 ? (pin38->INT_LVL[0] | (x+x*0x100)):(edge == 0 ? ((pin38->INT_LVL[0] | x*0x100) & ~x):pin38->INT_LVL[0] | (x*0x100+x*0x10000)));
      pin38->INT_ENB[0] |= x;
      pin38->INT_CLR[0] |= x;
      gpio_offset = 77;
      break;
    case 40:
      x = 0x00000040;
      pin40->INT_LVL[0] = (edge == 1 ? (pin40->INT_LVL[0] | (x+x*0x100)):(edge == 0 ? ((pin40->INT_LVL[0] | x*0x100) & ~x):pin40->INT_LVL[0] | (x*0x100+x*0x10000)));
      pin40->INT_ENB[0] |= x;
      pin40->INT_CLR[0] |= x;
      gpio_offset = 78;
      break;
    default:
      status = -2;
      printf("Only gpio numbers from 3 to 40 are accepted\n");
    }
  }
  
  else {printf("Edge should be: RISING_EDGE,FALLING_EDGE or EITHER_EDGE\n");
    status = -3;
  }
  if (ISRFunc_CFG[gpio]->gpio != 0){
    printf("Input pin %d is already being monitored for interruptions\n", gpio);
    status = -4;
  }
  else{
    ISRFunc_CFG[gpio]->gpio = gpio;
    ISRFunc_CFG[gpio]->f = f;
    ISRFunc_CFG[gpio]->edge = edge;
    ISRFunc_CFG[gpio]->gpio_offset = gpio_offset;
    ISRFunc_CFG[gpio]->stat_reg = x;
    ISRFunc_CFG[gpio]->timestamp = timestamp;
    ISRFunc_CFG[gpio]->debounce = debounce;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
    pth_err = pthread_create(&callThd[thread_n], &attr, callback, (void *)ISRFunc_CFG[gpio]); 
    if (pth_err !=0){
      printf("Thread not created, exiting the function  with error: %d\n", pth_err);
      return(-5);
    }
    thread_n++;
  }
  return status;
}

int gpioSetPWMfrequency(unsigned gpio, unsigned frequency){
    
  int status = 1;
  int PFM = 0;
  if ((frequency >= 25) && (frequency <=187000)){
    PFM = round(187500.0/(double)frequency)-1;
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
      status = -1;
      printf("Only gpio numbers 32 and 33 are accepted\n");
    }
    pin_tracker |= (1 << 28);		
  }
  else {printf("Only frequencies from 25 to 187000 Hz are allowed\n");
    status =-2;}
  return status;
}

int gpioPWM(unsigned gpio, unsigned dutycycle){
    
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
      printf("Only gpio numbers 32 and 33 are accepted,\n");
    }
		
  }
  else {printf("Only a dutycycle from 0 to 256 is allowed\n");
    status =-2;}
  return status;
}

int i2c_smbus_access(int file, char read_write, __u8 command, int size, union i2c_smbus_data *data){
    
  struct i2c_smbus_ioctl_data args;
  args.read_write = read_write;
  args.command = command;
  args.size = size;
  args.data = data;
  return ioctl(file,I2C_SMBUS,&args);
}

int i2cOpen(unsigned i2cBus, unsigned i2cFlags){
    
  char dev[20], buf[100];
  int fd, slot, speed;
  uint32_t funcs;
  FILE *fptr;

  if (!(i2cBus == 0 || i2cBus == 1)){
    printf( "Bad i2c device (%d) only 0 or 1 are accepted\n", i2cBus);
    return -1;
  }

  if (!(i2cFlags == 0 || i2cFlags == 1 || i2cFlags == 2)){
    printf( "Only flags 0 to 2 are supported to set up bus speed\n");
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
	
  slot = -5;
	
  if (i2cInfo[i2cBus].state == I2C_CLOSED) {
    slot = i2cBus;
    i2cInfo[slot].state = I2C_RESERVED;
  }
  else { printf("i2c bus already open\n");
    return -3;
  }
	
  snprintf(buf, sizeof(buf), "/sys/bus/i2c/devices/i2c-%d/bus_clk_rate", i2cBus);
  fptr = fopen(buf, "r");
	
  if (fptr == NULL) {
    printf("Not possible to read current bus speed\n");
  }
	
  fscanf(fptr, "%d", &i2c_speed[i2cBus]);
   	
  snprintf(buf, sizeof(buf), "echo %d > /sys/bus/i2c/devices/i2c-%d/bus_clk_rate", speed, i2cBus);
  if (system(buf) == -1) { 
    printf( "Not possible to change bus speed\n");
  }

  strcpy(buf, "modprobe i2c_dev");
    
  if (system(buf) == -1) { /* Ignore errors */
  }

  snprintf(dev, 19, "/dev/i2c-%d", i2cBus);
  fd = open(dev, O_RDWR);
  if (fd < 0) {
    printf( "Bad handle (%d)\n", fd);
    return -4;	
  }

  if (ioctl(fd, I2C_FUNCS, &funcs) < 0){
    funcs = -1; /* assume all smbus commands allowed */
    return -6;
  }

  i2cInfo[slot].fd = fd;
  i2cInfo[slot].flags = i2cFlags;
  i2cInfo[slot].funcs = funcs;
  i2cInfo[slot].state = I2C_OPENED;

  return slot;
}

int i2cClose(unsigned handle){
    
  char buf[100];
	
  if (handle > 1) {
    printf( "bad handle (%d)", handle);
    return -1;
  }

  if (i2cInfo[handle].state != I2C_OPENED) {
    printf( "i2c bus is already closed (%d)", handle);
    return -2;	
  }
     
  if (i2cInfo[handle].fd >= 0) {close(i2cInfo[handle].fd);}

  i2cInfo[handle].fd = -1;
  i2cInfo[handle].state = I2C_CLOSED;
   
  snprintf(buf, sizeof(buf), "echo %d > /sys/bus/i2c/devices/i2c-%d/bus_clk_rate", i2c_speed[handle], handle);
  if (system(buf) == -1) { 
    printf( "Not possible to return bus speed to original value\n");
  }

  return 0;
}

int i2cWriteByteData(unsigned handle, unsigned i2cAddr, unsigned reg, unsigned bVal){
    
  union i2c_smbus_data data;
  int status = 0;
	
  if (handle >= 2) {
    printf( "Bad handle (%d)\n", handle);
    status = -1;
  }

  if (i2cInfo[handle].state != I2C_OPENED){
    printf( "i2c%d is not open\n", handle);
    status = -2;
  }

  if (i2cAddr > 0x7f){
    printf( "Bad I2C address (%d)\n", i2cAddr);
    status = -3;
  }
	
  if (reg > 0xFF){
    printf( "Register address on device bigger than 0xFF\n");
    status = -4;
  }

  if (bVal > 0xFF){
    printf( "Value to be written bigger than byte\n");
    status = -5;
  }

  i2cInfo[handle].addr = i2cAddr;

  if (ioctl(i2cInfo[handle].fd, I2C_SLAVE, i2cAddr) < 0) {
    printf( "I2C slave address not found on bus\n");
    status = -6;
  }

  if ((i2cInfo[handle].funcs & I2C_FUNC_SMBUS_WRITE_BYTE_DATA) == 0){
    printf( "Write byte data function not supported by device\n");
    status = -7;
  }
	
  data.byte = bVal;
    
  if (i2c_smbus_access(i2cInfo[handle].fd,I2C_SMBUS_WRITE, reg, I2C_SMBUS_BYTE_DATA, &data)<0) {
    printf( "Not possible to write register\n");
    status = -8;}
  return status;
}

int i2cReadByteData(unsigned handle, unsigned i2cAddr, unsigned reg){
    
  int status = 0;
  union i2c_smbus_data data;
	
  if (handle >= 2) {
    printf( "Bad handle (%d)\n", handle);
    status = -1;
  }

  if (i2cInfo[handle].state != I2C_OPENED){
    printf( "i2c%d is not open\n", handle);
    status = -2;
  }
    
  if (i2cAddr > 0x7f){
    printf( "Bad I2C address (%d)\n", i2cAddr);
    status = -3;
  }

  if (reg > 0xFF){
    printf( "Register address on device bigger than 0xFF\n");
    status = -4;
  }

  i2cInfo[handle].addr = i2cAddr;

  if (ioctl(i2cInfo[handle].fd, I2C_SLAVE, i2cAddr) < 0) {
    printf( "I2C slave address not found on bus\n");
    status = -5;
  }
    
  if ((i2cInfo[handle].funcs & I2C_FUNC_SMBUS_READ_BYTE_DATA) == 0){
    printf( "Read byte data function not supported by device\n");
    status = -6;
  }
	
  if (i2c_smbus_access(i2cInfo[handle].fd,I2C_SMBUS_READ, reg, I2C_SMBUS_BYTE_DATA, &data)<0) {
    printf( "Not possible to read register\n");
    status = -7;}
  else
    {status = 0x0FF & data.byte;}
  return status;
}

int i2cWriteWordData(unsigned handle, unsigned i2cAddr, unsigned reg, unsigned wVal){
    
  union i2c_smbus_data data;
  int status = 0;
	
  if (handle >= 2) {
    printf( "Bad handle (%d)\n", handle);
    status = -1;
  }

  if (i2cInfo[handle].state != I2C_OPENED){
    printf( "i2c%d is not open\n", handle);
    status = -2;
  }

  if (i2cAddr > 0x7f){
    printf( "Bad I2C address (%d)\n", i2cAddr);
    status = -3;
  }
	
  if (reg > 0xFF){
    printf( "Register address on device bigger than 0xFF\n");
    status = -4;
  }

  if (wVal > 0xFFFF){
    printf( "Value to be written bigger than word\n");
    status = -5;
  }

  i2cInfo[handle].addr = i2cAddr;

  if (ioctl(i2cInfo[handle].fd, I2C_SLAVE, i2cAddr) < 0) {
    printf( "I2C slave address not found on bus\n");
    status = -6;
  }

  if ((i2cInfo[handle].funcs & I2C_FUNC_SMBUS_WRITE_WORD_DATA) == 0){
    printf( "Write word data function not supported by device\n");
    status = -7;
  }
	
  data.word = wVal;
    
  if (i2c_smbus_access(i2cInfo[handle].fd,I2C_SMBUS_WRITE, reg, I2C_SMBUS_WORD_DATA, &data)<0) {
    printf( "Not possible to write register\n");
    status = -8;}
  return status;
}

int i2cReadWordData(unsigned handle, unsigned i2cAddr, unsigned reg){
    
  int status = 0;
  union i2c_smbus_data data;
	
  if (handle >= 2) {
    printf( "Bad handle (%d)\n", handle);
    status = -1;
  }

  if (i2cInfo[handle].state != I2C_OPENED){
    printf( "i2c%d is not open\n", handle);
    status = -2;
  }
    
  if (i2cAddr > 0x7f){
    printf( "Bad I2C address (%d)\n", i2cAddr);
    status = -3;
  }

  if (reg > 0xFF){
    printf( "Register address on device bigger than 0xFF\n");
    status = -4;
  }

  i2cInfo[handle].addr = i2cAddr;

  if (ioctl(i2cInfo[handle].fd, I2C_SLAVE, i2cAddr) < 0) {
    printf( "I2C slave address not found on bus\n");
    status = -5;
  }
    
  if ((i2cInfo[handle].funcs & I2C_FUNC_SMBUS_READ_WORD_DATA) == 0){
    printf( "Read word data function not supported by device\n");
    status = -6;
  }
	
  if (i2c_smbus_access(i2cInfo[handle].fd,I2C_SMBUS_READ, reg, I2C_SMBUS_WORD_DATA, &data)<0) {
    printf( "Not possible to read register\n");
    status = -7;}
  else
    {status = 0x0FFFF & data.word;}
  return status;
}

int spiOpen(unsigned spiChan, unsigned speed, unsigned mode, unsigned cs_delay, unsigned bits_word, unsigned lsb_first, unsigned cs_change){
    
  char dev[20], buf[100];
  int fd, slot;
  int ret = 0;

  if (!(spiChan == 0 || spiChan == 1)){
    printf( "Bad spi device (%d) only 0 or 1 are accepted\n", spiChan);
    return -1;
  }

  if (speed < 0 || speed > 50000000){
    printf( "Speed in bits/second (%d) shouldn't be bigger that 50 Mbit/s\n", speed);
    return -2;
  }

  if (!(mode == 0 || mode == 1 || mode == 2 || mode == 3)){
    printf( "Mode (%d) should be a number between 0 and 3\n", mode);
    return -3;
  }

  if (cs_delay < 0 || cs_delay > 1000){
    printf( "cs_delay in us (%d) shouldn't be bigger that 1000 us\n", cs_delay);
    return -5;
  }

  if (bits_word < 0 || bits_word > 32){
    printf( "Bits per word (%d) should be a number between 0 and 32\n", bits_word);
    return -6;
  }

  if (!(lsb_first == 0 || lsb_first == 1)){
    printf( "Least significant bit first option (%d) should be 0 or 1\n", lsb_first);
    return -7;
  }

  if (!(cs_change == 0 || cs_change == 1)){
    printf( "cs_change option (%d) should be 0 or 1\n", cs_change);
    return -10;
  }
    
  slot = -22;    
    
  if (SpiInfo[spiChan].state == SPI_CLOSED) {
    slot = spiChan;
    SpiInfo[slot].state = SPI_RESERVED;
  }
  else { printf("Spi bus already open\n");
    return -11;
  }

  //*apbdev_pmc_pwr_det_val = 0x007cbc2d;
    
  if (spiChan == 0) {
    pin19->CNF[0] &= ~(0x00000001);
    *pinmux19 = 0xe200;
    *pincfg19 = 0xf0000000;
    pin21->CNF[0] &= ~(0x00000002);
    *pinmux21 = 0x0240;
    *pincfg21 = 0x00000000;
    pin23->CNF[0] &= ~(0x00000004);
    *pinmux23 = 0xe200; 
    *pincfg23 = 0xf0000000;
    pin24->CNF[0] &= ~(0x00000008);
    *pinmux24 = 0xe200;
    *pincfg24 = 0xf0000000;
    pin_tracker |= (1 << 29);
  }

  if (spiChan == 1) {
    pin37->CNF[0] &= ~(0x00000010);
    *pinmux37 = 0xe200;
    *pincfg37 = 0xf0000000;
    pin22->CNF[0] &= ~(0x00000020);
    *pinmux22 = 0x0240;
    *pincfg22 = 0x00000000;
    pin13->CNF[0] &= ~(0x00000040);
    *pinmux13 = 0xe200;
    *pincfg13 = 0xf0000000;
    pin18->CNF[0] &= ~(0x00000080); 
    *pinmux18 = 0xe200;
    *pincfg18 = 0xf0000000;
    pin_tracker |= (1 << 30);
  }

  strcpy(buf, "modprobe spidev bufsiz=65535");
    
  if (system(buf) == -1) { 
    printf( "Not possible to load the linux spidev module (driver) \n");
    return -12;
  }
 
  snprintf(dev, 19, "/dev/spidev%d.0", spiChan);
  fd = open(dev, O_RDWR);
  if (fd < 0) {
    printf( "Bad handle (%d)\n", fd);
    return -13;	
  }
    
  ret = ioctl(fd, SPI_IOC_WR_MODE, &mode);
  if (ret < 0){
    printf("Can't set spi mode\n");
    return -14;
  }
    
  ret = ioctl(fd, SPI_IOC_RD_MODE, &mode);
  if (ret < 0){
    printf("Can't get spi mode\n");
    return -15; 
  }
    
  ret = ioctl(fd, SPI_IOC_WR_BITS_PER_WORD, &bits_word);
  if (ret < 0){
    printf("Can't set bits per word\n");
    return -16;
  }
    
  ret = ioctl(fd, SPI_IOC_RD_BITS_PER_WORD, &bits_word);
  if (ret < 0){
    printf("Can't get bits per word\n");
    return -17;
  }
  
  ret = ioctl(fd, SPI_IOC_WR_MAX_SPEED_HZ, &speed);
  if (ret < 0){
    printf("Can't set max speed hz");
    return -18;
  }

  ret = ioctl(fd, SPI_IOC_RD_MAX_SPEED_HZ, &speed);
  if (ret < 0){
    printf("Can't get max speed\n");
    return -19;
  }

  ret = ioctl(fd, SPI_IOC_WR_LSB_FIRST, &lsb_first);
  if (ret < 0){
    printf("Can't set lsb first\n");
    return -20;
  }

  ret = ioctl(fd, SPI_IOC_RD_LSB_FIRST, &lsb_first);
  if (ret < 0){
    printf("Can't get lsb first\n");
    return -21;
  }

  SpiInfo[slot].fd = fd;
  SpiInfo[slot].mode = mode;
  SpiInfo[slot].speed = speed;
  SpiInfo[slot].cs_delay = cs_delay;
  SpiInfo[slot].cs_change = cs_change;
  SpiInfo[slot].bits_word = bits_word;
  SpiInfo[slot].state = SPI_OPENED;
    
  return slot;
}

int spiClose(unsigned handle){
    
  if (handle > 1) {
    printf( "Bad handle (%d)", handle);
    return -1;
  }
    
  if (SpiInfo[handle].state != SPI_OPENED) {
    printf( "Spi bus is already closed (%d)", handle);
    return -2;	
  }
     
  if (SpiInfo[handle].fd >= 0) {close(SpiInfo[handle].fd);}
    
  //*apbdev_pmc_pwr_det_val = 0x00fcbc2d;
    
  SpiInfo[handle].fd = -1;
  SpiInfo[handle].state = SPI_CLOSED;

  return 0;
}

int spiXfer(unsigned handle, char *txBuf, char *rxBuf, unsigned len){
    
  int ret;
  struct spi_ioc_transfer tr;
    
  if (handle > 1) {
    printf( "Bad handle (%d)\n", handle);
    return -1;
  }

  tr.tx_buf = (unsigned long)txBuf;
  tr.rx_buf = (unsigned long)rxBuf;
  tr.len = len;
  tr.delay_usecs = SpiInfo[handle].cs_delay;
  tr.speed_hz = SpiInfo[handle].speed;
  tr.bits_per_word = SpiInfo[handle].bits_word;
  tr.cs_change = SpiInfo[handle].cs_change;
  tr.tx_nbits = 1;
  tr.rx_nbits = 1;
    
  ret = ioctl(SpiInfo[handle].fd, SPI_IOC_MESSAGE(1), &tr);
  if (ret < 1){
    printf("Can't send spi message\n");
    return -2;
  }
  return ret;
}
