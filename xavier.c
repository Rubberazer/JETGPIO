/*MIT License
 *
 *Copyright (c) 2025 Rubberazer
 *
 *Permission is hereby granted, free of charge, to any person obtaining a copy
 *of this software and associated documentation files (the "Software"), to deal
 *in the Software without restriction, including without limitation the rights
 *to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 *copies of the Software, and to permit persons to whom the Software is
 *furnished to do so, subject to the following conditions:
 *
 *The above copyright notice and this permission notice shall be included in all
 *copies or substantial portions of the Software.
 *
 *THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 *OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 *SOFTWARE.
 */

/* jetgpio version 2.1 */
/* Xavier NX extension */

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
#include <linux/version.h>

#include "jetgpio.h"

#define BILLION 1000000000L

static int fd_GPIO;

static volatile GPIO_CNF_Init pin_CNF;
static volatile GPIO_CNF_Init pin_DEB;
static volatile GPIO_CNF_Init pin_OUT;
static volatile GPIO_CNF_Init pin_OUT_VLE;
static volatile GPIO_CNF_Init pin_INT_CLR;
static volatile GPIO_CNF_Init pin_MUX;
static volatile GPIO_CNF_Init pin_CFG;

PISRFunc ISRFunc_CFG[41];

static volatile uint32_t *PWM1;
static volatile uint32_t *PWM8;
static volatile uint32_t  PWM1_Init;
static volatile uint32_t  PWM8_Init;

static i2cInfo_t i2cInfo[9];
static int i2c_speed[9];
static SPIInfo_t SpiInfo[3];

static volatile GPIO_CNFO *pin7;
static volatile GPIO_CNFO *pin8;
static volatile GPIO_CNFO *pin10;
static volatile GPIO_CNFO *pin11;
static volatile GPIO_CNFO *pin12;
static volatile GPIO_CNFO *pin13;
static volatile GPIO_CNFO *pin15;
static volatile GPIO_CNFO *pin16;
static volatile GPIO_CNFO *pin18;
static volatile GPIO_CNFO *pin19;
static volatile GPIO_CNFO *pin21;
static volatile GPIO_CNFO *pin22;
static volatile GPIO_CNFO *pin23;
static volatile GPIO_CNFO *pin24;
static volatile GPIO_CNFO *pin26;
static volatile GPIO_CNFO *pin27;
static volatile GPIO_CNFO *pin28;
static volatile GPIO_CNFO *pin29;
static volatile GPIO_CNFO *pin31;
static volatile GPIO_CNFO *pin32;
static volatile GPIO_CNFO *pin33;
static volatile GPIO_CNFO *pin35;
static volatile GPIO_CNFO *pin36;
static volatile GPIO_CNFO *pin37;
static volatile GPIO_CNFO *pin38;
static volatile GPIO_CNFO *pin40;

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

static void *baseCNF_AON;
static void *baseCNF_NAON;

static void *basePINMUX_AON;
static void *basePINMUX_Audio;
static void *basePINMUX_CAM;
static void *basePINMUX_UART;
static void *basePINMUX_EDP;

static void *basePWM1;
static void *basePWM8;
static unsigned clk_rate_PWM1 = 408000000;
static unsigned clk_rate_PWM8 = 408000000;

static volatile unsigned global_int;
static pthread_t callThd[26];
static pthread_attr_t attr;
static int pth_err;
static void *status_thread;
static int thread_n = 0;
static unsigned long long pin_tracker = 0;

int gpioInitialise(void)
{
  int status = 1;
  //  Getting the page size
  int pagesize = sysconf(_SC_PAGESIZE);
  //  read physical memory (needs root)
  fd_GPIO = open("/dev/mem", O_RDWR | O_SYNC);
  if (fd_GPIO < 0) {
    perror("/dev/mem");
    fprintf(stderr, "Please run this program as root (for example with sudo)\n");
    return -1;
  }
  //  Mapping base_CNF_xavier_AON
  baseCNF_AON = mmap(0, pagesize, PROT_READ | PROT_WRITE, MAP_SHARED, fd_GPIO, base_CNF_xavier_AON);
  if (baseCNF_AON == MAP_FAILED) {
    return -2;
  }

  //  Mapping base_CNF_xavier_NAON
  baseCNF_NAON = mmap(0, 5 * pagesize, PROT_READ | PROT_WRITE, MAP_SHARED, fd_GPIO, base_CNF_xavier_NAON);
  if (baseCNF_NAON == MAP_FAILED) {
    return -3;
  }
    
  //  Mapping Pinmux_xavier_AON
  basePINMUX_AON = mmap(0, pagesize, PROT_READ | PROT_WRITE, MAP_SHARED, fd_GPIO, Pinmux_xavier_AON);
  if (basePINMUX_AON == MAP_FAILED) {
    return -4;
  }

  //  Mapping Pinmux_xavier_Audio
  basePINMUX_Audio = mmap(0, pagesize, PROT_READ | PROT_WRITE, MAP_SHARED, fd_GPIO, Pinmux_xavier_Audio);
  if (basePINMUX_Audio == MAP_FAILED) {
    return -5;
  }
    
  //  Mapping Pinmux_xavier_CAM
  basePINMUX_CAM = mmap(0, pagesize, PROT_READ | PROT_WRITE, MAP_SHARED, fd_GPIO, Pinmux_xavier_CAM);
  if (basePINMUX_CAM == MAP_FAILED) {
    return -6;
  }

  //  Mapping Pinmux_xavier_UART
  basePINMUX_UART = mmap(0, pagesize, PROT_READ | PROT_WRITE, MAP_SHARED, fd_GPIO, Pinmux_xavier_UART);
  if (basePINMUX_UART == MAP_FAILED) {
    return -7;
  }

  //  Mapping Pinmux_xavier_EDP
  basePINMUX_EDP = mmap(0, pagesize, PROT_READ | PROT_WRITE, MAP_SHARED, fd_GPIO, Pinmux_xavier_EDP);
  if (basePINMUX_EDP == MAP_FAILED) {
    return -8;
  }
  
  //  Mapping PWM1
  basePWM1 = mmap(0, pagesize, PROT_READ | PROT_WRITE, MAP_SHARED, fd_GPIO, base_xavier_PWM1);
  if (basePWM1 == MAP_FAILED) {
    return -9;
  }

  //  Mapping PWM8
  basePWM8 = mmap(0, pagesize, PROT_READ | PROT_WRITE, MAP_SHARED, fd_GPIO, base_xavier_PWM8);
  if (basePWM8 == MAP_FAILED) {
    return -10;
  }

  // Pointer to CNF7
  pin7 = (GPIO_CNFO volatile *)((char *)baseCNF_NAON + CNFX_7);
  pin_CNF.pin7 = pin7->CNF[0];
  pin_DEB.pin7 = pin7->DEB[0];
  pin_OUT.pin7 = pin7->OUT[0];
  pin_OUT_VLE.pin7 = pin7->OUT_VLE[0];
    
  // Pointer to PINMUX7
  pinmux7 = (uint32_t volatile *)((char *)basePINMUX_Audio + PINMUXX_7);
  pin_MUX.pin7 = *pinmux7;
    
  // Pointer to PINCFG7
  pincfg7 = (uint32_t volatile *)((char *)basePINMUX_Audio + CFGX_7);
  pin_CFG.pin7 = *pincfg7;
  
  // Pointer to CNF8
  pin8 = (GPIO_CNFO volatile *)((char *)baseCNF_NAON + CNFX_8);
  pin_CNF.pin8 = pin8->CNF[0];
  pin_DEB.pin8 = pin8->DEB[0];
  pin_OUT.pin8 = pin8->OUT[0];
  pin_OUT_VLE.pin8 = pin8->OUT_VLE[0];
  
  // Pointer to PINMUX8
  pinmux8 = (uint32_t volatile *)((char *)basePINMUX_CAM + PINMUXX_8);
  pin_MUX.pin8 = *pinmux8;
    
  // Pointer to PINCFG8
  pincfg8 = (uint32_t volatile *)((char *)basePINMUX_CAM + CFGX_8);
  pin_CFG.pin8 = *pincfg8;
    
  // Pointer to CNF10
  pin10 = (GPIO_CNFO volatile *)((char *)baseCNF_NAON + CNFX_10);
  pin_CNF.pin10 = pin10->CNF[0];
  pin_DEB.pin10 = pin10->DEB[0];
  pin_OUT.pin10 = pin10->OUT[0];
  pin_OUT_VLE.pin10 = pin10->OUT_VLE[0];
    
  // Pointer to PINMUX10
  pinmux10 = (uint32_t volatile *)((char *)basePINMUX_CAM + PINMUXX_10);
  pin_MUX.pin10 = *pinmux10;
    
  // Pointer to PINCFG10
  pincfg10 = (uint32_t volatile *)((char *)basePINMUX_CAM + CFGX_10);
  pin_CFG.pin10 = *pincfg10;
    
  // Pointer to CNF11
  pin11 = (GPIO_CNFO volatile *)((char *)baseCNF_NAON + CNFX_11);
  pin_CNF.pin11 = pin11->CNF[0];
  pin_DEB.pin11 = pin11->DEB[0];
  pin_OUT.pin11 = pin11->OUT[0];
  pin_OUT_VLE.pin11 = pin11->OUT_VLE[0];
  
  // Pointer to PINMUX11
  pinmux11 = (uint32_t volatile *)((char *)basePINMUX_CAM + PINMUXX_11);
  pin_MUX.pin11 = *pinmux11;
    
  // Pointer to PINCFG11
  pincfg11 = (uint32_t volatile *)((char *)basePINMUX_CAM + CFGX_11);
  pin_CFG.pin11 = *pincfg11;
    
  // Pointer to CNF12
  pin12 = (GPIO_CNFO volatile *)((char *)baseCNF_NAON + CNFX_12);
  pin_CNF.pin12 = pin12->CNF[0];
  pin_DEB.pin12 = pin12->DEB[0];
  pin_OUT.pin12 = pin12->OUT[0];
  pin_OUT_VLE.pin12 = pin12->OUT_VLE[0];
    
  // Pointer to PINMUX12
  pinmux12 = (uint32_t volatile *)((char *)basePINMUX_Audio + PINMUXX_12);
  pin_MUX.pin12 = *pinmux12;
    
  // Pointer to PINCFG12
  pincfg12 = (uint32_t volatile *)((char *)basePINMUX_Audio + CFGX_12);
  pin_CFG.pin12 = *pincfg12;
    
  // Pointer to CNF13
  pin13 = (GPIO_CNFO volatile *)((char *)baseCNF_NAON + CNFX_13);
  pin_CNF.pin13 = pin13->CNF[0];
  pin_DEB.pin13 = pin13->DEB[0];
  pin_OUT.pin13 = pin13->OUT[0];
  pin_OUT_VLE.pin13 = pin13->OUT_VLE[0];
      
  // Pointer to PINMUX13
  pinmux13 = (uint32_t volatile *)((char *)basePINMUX_UART + PINMUXX_13);
  pin_MUX.pin13 = *pinmux13;
    
  // Pointer to PINCFG13
  pincfg13 = (uint32_t volatile *)((char *)basePINMUX_UART + CFGX_13);
  pin_CFG.pin13 = *pincfg13;
    
  // Pointer to CNF15
  pin15 = (GPIO_CNFO volatile *)((char *)baseCNF_AON + CNFX_15);
  pin_CNF.pin15 = pin15->CNF[0];
  pin_DEB.pin15 = pin15->DEB[0];
  pin_OUT.pin15 = pin15->OUT[0];
  pin_OUT_VLE.pin15 = pin15->OUT_VLE[0];
    
  // Pointer to PINMUX15
  pinmux15 = (uint32_t volatile *)((char *)basePINMUX_AON + PINMUXX_15);
  pin_MUX.pin15 = *pinmux15;
    
  // Pointer to PINCFG15
  pincfg15 = (uint32_t volatile *)((char *)basePINMUX_AON + CFGX_15);
  pin_CFG.pin15 = *pincfg15;
    
  // Pointer to CNF16
  pin16 = (GPIO_CNFO volatile *)((char *)baseCNF_NAON + CNFX_16);
  pin_CNF.pin16 = pin16->CNF[0];
  pin_DEB.pin16 = pin16->DEB[0];
  pin_OUT.pin16 = pin16->OUT[0];
  pin_OUT_VLE.pin16 = pin16->OUT_VLE[0];
      
  // Pointer to PINMUX16
  pinmux16 = (uint32_t volatile *)((char *)basePINMUX_UART + PINMUXX_16);
  pin_MUX.pin16 = *pinmux16;
   
  // Pointer to PINCFG16
  pincfg16 = (uint32_t volatile *)((char *)basePINMUX_UART + CFGX_16);
  pin_CFG.pin16 = *pincfg16;
    
  // Pointer to CNF18
  pin18 = (GPIO_CNFO volatile *)((char *)baseCNF_NAON + CNFX_18);
  pin_CNF.pin18 = pin18->CNF[0];
  pin_DEB.pin18 = pin18->DEB[0];
  pin_OUT.pin18 = pin18->OUT[0];
  pin_OUT_VLE.pin18 = pin18->OUT_VLE[0];
      
  // Pointer to PINMUX18
  pinmux18 = (uint32_t volatile *)((char *)basePINMUX_UART + PINMUXX_18);
  pin_MUX.pin18 = *pinmux18;
    
  // Pointer to PINCFG18
  pincfg18 = (uint32_t volatile *)((char *)basePINMUX_UART + CFGX_18);
  pin_CFG.pin18 = *pincfg18;
    
  // Pointer to CNF19
  pin19 = (GPIO_CNFO volatile *)((char *)baseCNF_NAON + CNFX_19);
  pin_CNF.pin19 = pin19->CNF[0];
  pin_DEB.pin19 = pin19->DEB[0];
  pin_OUT.pin19 = pin19->OUT[0];
  pin_OUT_VLE.pin19 = pin19->OUT_VLE[0];
      
  // Pointer to PINMUX19
  pinmux19 = (uint32_t volatile *)((char *)basePINMUX_UART + PINMUXX_19);
  pin_MUX.pin19 = *pinmux19;
    
  // Pointer to PINCFG19
  pincfg19 = (uint32_t volatile *)((char *)basePINMUX_UART + CFGX_19);
  pin_CFG.pin19 = *pincfg19;
    
  // Pointer to CNF21
  pin21 = (GPIO_CNFO volatile *)((char *)baseCNF_NAON + CNFX_21);
  pin_CNF.pin21 = pin21->CNF[0];
  pin_DEB.pin21 = pin21->DEB[0];
  pin_OUT.pin21 = pin21->OUT[0];
  pin_OUT_VLE.pin21 = pin21->OUT_VLE[0];
      
  // Pointer to PINMUX21
  pinmux21 = (uint32_t volatile *)((char *)basePINMUX_UART + PINMUXX_21);
  pin_MUX.pin21 = *pinmux21;
    
  // Pointer to PINCFG21
  pincfg21 = (uint32_t volatile *)((char *)basePINMUX_UART + CFGX_21);
  pin_CFG.pin21 = *pincfg21;
    
  // Pointer to CNF22
  pin22 = (GPIO_CNFO volatile *)((char *)baseCNF_NAON + CNFX_22);
  pin_CNF.pin22 = pin22->CNF[0];
  pin_DEB.pin22 = pin22->DEB[0];
  pin_OUT.pin22 = pin22->OUT[0];
  pin_OUT_VLE.pin22 = pin22->OUT_VLE[0];
      
  // Pointer to PINMUX22
  pinmux22 = (uint32_t volatile *)((char *)basePINMUX_UART + PINMUXX_22);
  pin_MUX.pin22 = *pinmux22;
    
  // Pointer to PINCFG22
  pincfg22 = (uint32_t volatile *)((char *)basePINMUX_UART + CFGX_22);
  pin_CFG.pin22 = *pincfg22;
    
  // Pointer to CNF23
  pin23 = (GPIO_CNFO volatile *)((char *)baseCNF_NAON + CNFX_23);
  pin_CNF.pin23 = pin23->CNF[0];
  pin_DEB.pin23 = pin23->DEB[0];
  pin_OUT.pin23 = pin23->OUT[0];
  pin_OUT_VLE.pin23 = pin23->OUT_VLE[0];
      
  // Pointer to PINMUX23
  pinmux23 = (uint32_t volatile *)((char *)basePINMUX_UART + PINMUXX_23);
  pin_MUX.pin23 = *pinmux23;
    
  // Pointer to PINCFG23
  pincfg23 = (uint32_t volatile *)((char *)basePINMUX_UART + CFGX_23);
  pin_CFG.pin23 = *pincfg23;
    
  // Pointer to CNF24
  pin24 = (GPIO_CNFO volatile *)((char *)baseCNF_NAON + CNFX_24);
  pin_CNF.pin24 = pin24->CNF[0];
  pin_DEB.pin24 = pin24->DEB[0];
  pin_OUT.pin24 = pin24->OUT[0];
  pin_OUT_VLE.pin24 = pin24->OUT_VLE[0];
      
  // Pointer to PINMUX24
  pinmux24 = (uint32_t volatile *)((char *)basePINMUX_UART + PINMUXX_24);
  pin_MUX.pin24 = *pinmux24;
    
  // Pointer to PINCFG24
  pincfg24 = (uint32_t volatile *)((char *)basePINMUX_UART + CFGX_24);
  pin_CFG.pin24 = *pincfg24;
    
  // Pointer to CNF26
  pin26 = (GPIO_CNFO volatile *)((char *)baseCNF_NAON + CNFX_26);
  pin_CNF.pin26 = pin26->CNF[0];
  pin_DEB.pin26 = pin26->DEB[0];
  pin_OUT.pin26 = pin26->OUT[0];
  pin_OUT_VLE.pin26 = pin26->OUT_VLE[0];
      
  // Pointer to PINMUX26
  pinmux26 = (uint32_t volatile *)((char *)basePINMUX_UART + PINMUXX_26);
  pin_MUX.pin26 = *pinmux26;
    
  // Pointer to PINCFG26
  pincfg26 = (uint32_t volatile *)((char *)basePINMUX_UART + CFGX_26);
  pin_CFG.pin26 = *pincfg26;
    
  // Pointer to CNF27
  pin27 = (GPIO_CNFO volatile *)((char *)baseCNF_AON + CNFX_27);
  pin_CNF.pin27 = pin27->CNF[0];
  pin_DEB.pin27 = pin27->DEB[0];
  pin_OUT.pin27 = pin27->OUT[0];
  pin_OUT_VLE.pin27 = pin27->OUT_VLE[0];
      
  // Pointer to PINMUX27
  pinmux27 = (uint32_t volatile *)((char *)basePINMUX_AON + PINMUXX_27);
  pin_MUX.pin27 = *pinmux27;
    
  // Pointer to PINCFG27
  pincfg27 = (uint32_t volatile *)((char *)basePINMUX_AON + CFGX_27);
  pin_CFG.pin27 = *pincfg27;
  
  // Pointer to CNF28
  pin28 = (GPIO_CNFO volatile *)((char *)baseCNF_AON + CNFX_28);
  pin_CNF.pin28 = pin28->CNF[0];
  pin_DEB.pin28 = pin28->DEB[0];
  pin_OUT.pin28 = pin28->OUT[0];
  pin_OUT_VLE.pin28 = pin28->OUT_VLE[0];
    
  // Pointer to PINMUX28
  pinmux28 = (uint32_t volatile *)((char *)basePINMUX_AON + PINMUXX_28);
  pin_MUX.pin28 = *pinmux28;
    
  // Pointer to PINCFG28
  pincfg28 = (uint32_t volatile *)((char *)basePINMUX_AON + CFGX_28);
  pin_CFG.pin28 = *pincfg28;
    
  // Pointer to CNF29
  pin29 = (GPIO_CNFO volatile *)((char *)baseCNF_NAON + CNFX_29);
  pin_CNF.pin29 = pin29->CNF[0];
  pin_DEB.pin29 = pin29->DEB[0];
  pin_OUT.pin29 = pin29->OUT[0];
  pin_OUT_VLE.pin29 = pin29->OUT_VLE[0];
    
  // Pointer to PINMUX29
  pinmux29 = (uint32_t volatile *)((char *)basePINMUX_CAM + PINMUXX_29);
  pin_MUX.pin29 = *pinmux29;
    
  // Pointer to PINCFG29
  pincfg29 = (uint32_t volatile *)((char *)basePINMUX_CAM + CFGX_29);
  pin_CFG.pin29 = *pincfg29;
    
  // Pointer to CNF31
  pin31 = (GPIO_CNFO volatile *)((char *)baseCNF_NAON + CNFX_31);
  pin_CNF.pin31 = pin31->CNF[0];
  pin_DEB.pin31 = pin31->DEB[0];
  pin_OUT.pin31 = pin31->OUT[0];
  pin_OUT_VLE.pin31 = pin31->OUT_VLE[0];
    
  // Pointer to PINMUX31
  pinmux31 = (uint32_t volatile *)((char *)basePINMUX_CAM + PINMUXX_31);
  pin_MUX.pin31 = *pinmux31;
    
  // Pointer to PINCFG31
  pincfg31 = (uint32_t volatile *)((char *)basePINMUX_CAM + CFGX_31);
  pin_CFG.pin31 = *pincfg31;
    
  // Pointer to CNF32
  pin32 = (GPIO_CNFO volatile *)((char *)baseCNF_NAON + CNFX_32);
  pin_CNF.pin32 = pin32->CNF[0];
  pin_DEB.pin32 = pin32->DEB[0];
  pin_OUT.pin32 = pin32->OUT[0];
  pin_OUT_VLE.pin32 = pin32->OUT_VLE[0];
    
  // Pointer to PINMUX32
  pinmux32 = (uint32_t volatile *)((char *)basePINMUX_CAM + PINMUXX_32);
  pin_MUX.pin32 = *pinmux32;
   
  // Pointer to PINCFG32
  pincfg32 = (uint32_t volatile *)((char *)basePINMUX_CAM + CFGX_32);
  pin_CFG.pin32 = *pincfg32;
    
  // Pointer to CNF33
  pin33 = (GPIO_CNFO volatile *)((char *)baseCNF_NAON + CNFX_33); 
  pin_CNF.pin33 = pin33->CNF[0];
  pin_DEB.pin33 = pin33->DEB[0];
  pin_OUT.pin33 = pin33->OUT[0];
  pin_OUT_VLE.pin33 = pin33->OUT_VLE[0];
    
  // Pointer to PINMUX33
  pinmux33 = (uint32_t volatile *)((char *)basePINMUX_EDP + PINMUXX_33);
  pin_MUX.pin33 = *pinmux33;
    
  // Pointer to PINCFG33
  pincfg33 = (uint32_t volatile *)((char *)basePINMUX_EDP + CFGX_33);
  pin_CFG.pin33 = *pincfg33;
    
  // Pointer to CNF35
  pin35 = (GPIO_CNFO volatile *)((char *)baseCNF_NAON + CNFX_35);
  pin_CNF.pin35 = pin35->CNF[0];
  pin_DEB.pin35 = pin35->DEB[0];
  pin_OUT.pin35 = pin35->OUT[0];
  pin_OUT_VLE.pin35 = pin35->OUT_VLE[0];
    
  // Pointer to PINMUX35
  pinmux35 = (uint32_t volatile *)((char *)basePINMUX_Audio + PINMUXX_35);
  pin_MUX.pin35 = *pinmux35;
    
  // Pointer to PINCFG35
  pincfg35 = (uint32_t volatile *)((char *)basePINMUX_Audio + CFGX_35);
  pin_CFG.pin35 = *pincfg35;
    
  // Pointer to CNF36
  pin36 = (GPIO_CNFO volatile *)((char *)baseCNF_NAON + CNFX_36);
  pin_CNF.pin36 = pin36->CNF[0];
  pin_DEB.pin36 = pin36->DEB[0];
  pin_OUT.pin36 = pin36->OUT[0];
  pin_OUT_VLE.pin36 = pin36->OUT_VLE[0];
    
  // Pointer to PINMUX36
  pinmux36 = (uint32_t volatile *)((char *)basePINMUX_CAM + PINMUXX_36);
  pin_MUX.pin36 = *pinmux36;
    
  // Pointer to PINCFG36
  pincfg36 = (uint32_t volatile *)((char *)basePINMUX_CAM + CFGX_36);
  pin_CFG.pin36 = *pincfg36;
    
  // Pointer to CNF37
  pin37 = (GPIO_CNFO volatile *)((char *)baseCNF_NAON + CNFX_37);
  pin_CNF.pin37 = pin37->CNF[0];
  pin_DEB.pin37 = pin37->DEB[0];
  pin_OUT.pin37 = pin37->OUT[0];
  pin_OUT_VLE.pin37 = pin37->OUT_VLE[0];
    
  // Pointer to PINMUX37
  pinmux37 = (uint32_t volatile *)((char *)basePINMUX_UART + PINMUXX_37);
  pin_MUX.pin37 = *pinmux37;
    
  // Pointer to PINCFG37
  pincfg37 = (uint32_t volatile *)((char *)basePINMUX_UART + CFGX_37);
  pin_CFG.pin37 = *pincfg37;
    
  // Pointer to CNF38
  pin38 = (GPIO_CNFO volatile *)((char *)baseCNF_NAON + CNFX_38);
  pin_CNF.pin38 = pin38->CNF[0];
  pin_DEB.pin38 = pin38->DEB[0];
  pin_OUT.pin38 = pin38->OUT[0];
  pin_OUT_VLE.pin38 = pin38->OUT_VLE[0];
    
  // Pointer to PINMUX38
  pinmux38 = (uint32_t volatile *)((char *)basePINMUX_Audio + PINMUXX_38);
  pin_MUX.pin38 = *pinmux38;
    
  // Pointer to PINCFG38
  pincfg38 = (uint32_t volatile *)((char *)basePINMUX_Audio + CFGX_38);
  pin_CFG.pin38 = *pincfg38;
   
  // Pointer to CNF40
  pin40 = (GPIO_CNFO volatile *)((char *)baseCNF_NAON + CNFX_40);
  pin_CNF.pin40 = pin40->CNF[0];
  pin_DEB.pin40 = pin40->DEB[0];
  pin_OUT.pin40 = pin40->OUT[0];
  pin_OUT_VLE.pin40 = pin40->OUT_VLE[0];
    
  // Pointer to PINMUX40
  pinmux40 = (uint32_t volatile *)((char *)basePINMUX_Audio + PINMUXX_40);
  pin_MUX.pin40 = *pinmux40;
    
  // Pointer to PINCFG40
  pincfg40 = (uint32_t volatile *)((char *)basePINMUX_Audio + CFGX_40);
  pin_CFG.pin40 = *pincfg40;
    
  // Pointer to PWM1
  PWM1 = (uint32_t volatile *)((char *)basePWM1);
  PWM1_Init = *PWM1;

  // Pointer to PWM8
  PWM8 = (uint32_t volatile *)((char *)basePWM8);
  PWM8_Init = *PWM8;

  // Initialize i2c
  i2cInfo[1].state = I2C_CLOSED;
  i2cInfo[8].state = I2C_CLOSED;
    
  // Initialize spi
  SpiInfo[0].state = SPI_CLOSED;
  SpiInfo[2].state = SPI_CLOSED;

  // Global interrupt variable
  global_int = 1;

  // Allocating memory for the struct
  for (int j = 0; j < 41; j++) {
    ISRFunc_CFG[j] = calloc (1, sizeof(ISRFunc));
  }
  return status;
}

void gpioTerminate(void) {
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

  if ((pin_tracker >> 28) & 1) {
    *PWM1 = PWM1_Init;
    char buf[100];
    
    snprintf(buf, sizeof(buf), "echo %u > /sys/kernel/debug/bpmp/debug/clk/pwm1/rate", clk_rate_PWM1);
    if (system(buf) == -1) { 
      printf( "Not possible to change clock rate on pwm1\n");
    }
  }

  if ((pin_tracker >> 30) & 1) {
    *PWM8 = PWM8_Init;
    char buf[100];
    
    snprintf(buf, sizeof(buf), "echo %u > /sys/kernel/debug/bpmp/debug/clk/pwm8/rate", clk_rate_PWM8);
    if (system(buf) == -1) { 
      printf( "Not possible to change clock rate on pwm7\n");
    }
  }
    
  if ((pin_tracker >> 2) & 1) {
    pin7->CNF[0] = pin_CNF.pin7;
    pin7->DEB[0] = pin_DEB.pin7;
    pin7->OUT[0] = pin_OUT.pin7;
    pin7->OUT_VLE[0] = pin_OUT_VLE.pin7;
    *pinmux7 = pin_MUX.pin7;
    *pincfg7 = pin_CFG.pin7;
  }
  if ((pin_tracker >> 3) & 1) {
    pin8->CNF[0] = pin_CNF.pin8;
    pin8->DEB[0] = pin_DEB.pin8;
    pin8->OUT[0] = pin_OUT.pin8;
    pin8->OUT_VLE[0] = pin_OUT_VLE.pin8;
    *pinmux8 = pin_MUX.pin8;
    *pincfg8 = pin_CFG.pin8;
  } 
  if ((pin_tracker >> 4) & 1) {
    pin10->CNF[0] = pin_CNF.pin10;
    pin10->DEB[0] = pin_DEB.pin10;
    pin10->OUT[0] = pin_OUT.pin10;
    pin10->OUT_VLE[0] = pin_OUT_VLE.pin10;
    *pinmux10 = pin_MUX.pin10;
    *pincfg10 = pin_CFG.pin10;
  }
  if ((pin_tracker >> 5) & 1) {
    pin11->CNF[0] = pin_CNF.pin11;
    pin11->DEB[0] = pin_DEB.pin11;
    pin11->OUT[0] = pin_OUT.pin11;
    pin11->OUT_VLE[0] = pin_OUT_VLE.pin11;
    *pinmux11 = pin_MUX.pin11;
    *pincfg11 = pin_CFG.pin11;
  } 
  if ((pin_tracker >> 6) & 1) {
    pin12->CNF[0] = pin_CNF.pin12;
    pin12->DEB[0] = pin_DEB.pin12;
    pin12->OUT[0] = pin_OUT.pin12;
    pin12->OUT_VLE[0] = pin_OUT_VLE.pin12;
    *pinmux12 = pin_MUX.pin12;
    *pincfg12 = pin_CFG.pin12;
  }
  if (((pin_tracker >> 7) & 1) || ((pin_tracker >> 32) & 1)) {
    pin13->CNF[0] = pin_CNF.pin13;
    pin13->DEB[0] = pin_DEB.pin13;
    pin13->OUT[0] = pin_OUT.pin13;
    pin13->OUT_VLE[0] = pin_OUT_VLE.pin13;
    *pinmux13 = pin_MUX.pin13;
    *pincfg13 = pin_CFG.pin13;
  }
  if ((pin_tracker >> 8) & 1) {
    pin15->CNF[0] = pin_CNF.pin15;
    pin15->DEB[0] = pin_DEB.pin15;
    pin15->OUT[0] = pin_OUT.pin15;
    pin15->OUT_VLE[0] = pin_OUT_VLE.pin15;
    *pinmux15 = pin_MUX.pin15;
    *pincfg15 = pin_CFG.pin15;
  }
  if ((pin_tracker >> 9) & 1) {
    pin16->CNF[0] = pin_CNF.pin16;
    pin16->DEB[0] = pin_DEB.pin16;
    pin16->OUT[0] = pin_OUT.pin16;
    pin16->OUT_VLE[0] = pin_OUT_VLE.pin16;
    *pinmux16 = pin_MUX.pin16;
    *pincfg16 = pin_CFG.pin16;
  }
  if (((pin_tracker >> 10) & 1) || ((pin_tracker >> 32) & 1)) {
    pin18->CNF[0] = pin_CNF.pin18;
    pin18->DEB[0] = pin_DEB.pin18;
    pin18->OUT[0] = pin_OUT.pin18;
    pin18->OUT_VLE[0] = pin_OUT_VLE.pin18;
    *pinmux18 = pin_MUX.pin18;
    *pincfg18 = pin_CFG.pin18;
  }
  if (((pin_tracker >> 11) & 1) || ((pin_tracker >> 31) & 1)) {
    pin19->CNF[0] = pin_CNF.pin19;
    pin19->DEB[0] = pin_DEB.pin19;
    pin19->OUT[0] = pin_OUT.pin19;
    pin19->OUT_VLE[0] = pin_OUT_VLE.pin19;
    *pinmux19 = pin_MUX.pin19;
    *pincfg19 = pin_CFG.pin19;
  }
  if (((pin_tracker >> 12) & 1) || ((pin_tracker >> 31) & 1)) {
    pin21->CNF[0] = pin_CNF.pin21;
    pin21->DEB[0] = pin_DEB.pin21;
    pin21->OUT[0] = pin_OUT.pin21;
    pin21->OUT_VLE[0] = pin_OUT_VLE.pin21;
    *pinmux21 = pin_MUX.pin21;
    *pincfg21 = pin_CFG.pin21;
  }
  if (((pin_tracker >> 13) & 1) || ((pin_tracker >> 32) & 1)) {
    pin22->CNF[0] = pin_CNF.pin22;
    pin22->DEB[0] = pin_DEB.pin22;
    pin22->OUT[0] = pin_OUT.pin22;
    pin22->OUT_VLE[0] = pin_OUT_VLE.pin22;
    *pinmux22 = pin_MUX.pin22;
    *pincfg22 = pin_CFG.pin22;
  }
  if (((pin_tracker >> 14) & 1) || ((pin_tracker >> 31) & 1)) {
    pin23->CNF[0] = pin_CNF.pin23;
    pin23->DEB[0] = pin_DEB.pin23;
    pin23->OUT[0] = pin_OUT.pin23;
    pin23->OUT_VLE[0] = pin_OUT_VLE.pin23;
    *pinmux23 = pin_MUX.pin23;
    *pincfg23 = pin_CFG.pin23;
  }
  if (((pin_tracker >> 15) & 1) || ((pin_tracker >> 31) & 1)) {
    pin24->CNF[0] = pin_CNF.pin24;
    pin24->DEB[0] = pin_DEB.pin24;
    pin24->OUT[0] = pin_OUT.pin24;
    pin24->OUT_VLE[0] = pin_OUT_VLE.pin24;
    *pinmux24 = pin_MUX.pin24;
    *pincfg24 = pin_CFG.pin24;
  }
  if ((pin_tracker >> 16) & 1) {
    pin26->CNF[0] = pin_CNF.pin26;
    pin26->DEB[0] = pin_DEB.pin26;
    pin26->OUT[0] = pin_OUT.pin26;
    pin26->OUT_VLE[0] = pin_OUT_VLE.pin26;
    *pinmux26 = pin_MUX.pin26;
    *pincfg26 = pin_CFG.pin26;
  }
  if ((pin_tracker >> 17) & 1) {
    pin27->CNF[0] = pin_CNF.pin27;
    pin27->DEB[0] = pin_DEB.pin27;
    pin27->OUT[0] = pin_OUT.pin27;
    pin27->OUT_VLE[0] = pin_OUT_VLE.pin27;
    *pinmux27 = pin_MUX.pin27;
    *pincfg27 = pin_CFG.pin27;
  }
  if ((pin_tracker >> 18) & 1) {
    pin28->CNF[0] = pin_CNF.pin28;
    pin28->DEB[0] = pin_DEB.pin28;
    pin28->OUT[0] = pin_OUT.pin28;
    pin28->OUT_VLE[0] = pin_OUT_VLE.pin28;
    *pinmux28 = pin_MUX.pin28;
    *pincfg28 = pin_CFG.pin28;
  }
  if ((pin_tracker >> 19) & 1) {
    pin29->CNF[0] = pin_CNF.pin29;
    pin29->DEB[0] = pin_DEB.pin29;
    pin29->OUT[0] = pin_OUT.pin29;
    pin29->OUT_VLE[0] = pin_OUT_VLE.pin29;
    *pinmux29 = pin_MUX.pin29;
    *pincfg29 = pin_CFG.pin29;
  }
  if ((pin_tracker >> 20) & 1) {
    pin31->CNF[0] = pin_CNF.pin31;
    pin31->DEB[0] = pin_DEB.pin31;
    pin31->OUT[0] = pin_OUT.pin31;
    pin31->OUT_VLE[0] = pin_OUT_VLE.pin31;
    *pinmux31 = pin_MUX.pin31;
    *pincfg31 = pin_CFG.pin31;
  }
  if (((pin_tracker >> 21) & 1) || ((pin_tracker >> 30) & 1)) {
    pin32->CNF[0] = pin_CNF.pin32;
    pin32->DEB[0] = pin_DEB.pin32;
    pin32->OUT[0] = pin_OUT.pin32;
    pin32->OUT_VLE[0] = pin_OUT_VLE.pin32;
    *pinmux32 = pin_MUX.pin32;
    *pincfg32 = pin_CFG.pin32;
  }
  if (((pin_tracker >> 22) & 1) || ((pin_tracker >> 29) & 1)) {
    pin33->CNF[0] = pin_CNF.pin33;
    pin33->DEB[0] = pin_DEB.pin33;
    pin33->OUT[0] = pin_OUT.pin33;
    pin33->OUT_VLE[0] = pin_OUT_VLE.pin3;
    *pinmux33 = pin_MUX.pin33;
    *pincfg33 = pin_CFG.pin33;
  }
  if ((pin_tracker >> 23) & 1) {
    pin35->CNF[0] = pin_CNF.pin35;
    pin35->DEB[0] = pin_DEB.pin35;
    pin35->OUT[0] = pin_OUT.pin35;
    pin35->OUT_VLE[0] = pin_OUT_VLE.pin35;
    *pinmux35 = pin_MUX.pin35;
    *pincfg35 = pin_CFG.pin35;
  }
  if ((pin_tracker >> 24) & 1) {
    pin36->CNF[0] = pin_CNF.pin36;
    pin36->DEB[0] = pin_DEB.pin36;
    pin36->OUT[0] = pin_OUT.pin36;
    pin36->OUT_VLE[0] = pin_OUT_VLE.pin36;
    *pinmux36 = pin_MUX.pin36;
    *pincfg36 = pin_CFG.pin36;
  }
  if (((pin_tracker >> 25) & 1) || ((pin_tracker >> 32) & 1)) {
    pin37->CNF[0] = pin_CNF.pin37;
    pin37->DEB[0] = pin_DEB.pin37;
    pin37->OUT[0] = pin_OUT.pin37;
    pin37->OUT_VLE[0] = pin_OUT_VLE.pin37;
    *pinmux37 = pin_MUX.pin37;
    *pincfg37 = pin_CFG.pin37;
  }
  if ((pin_tracker >> 26) & 1) {
    pin38->CNF[0] = pin_CNF.pin38;
    pin38->DEB[0] = pin_DEB.pin38;
    pin38->OUT[0] = pin_OUT.pin38;
    pin38->OUT_VLE[0] = pin_OUT_VLE.pin38;
    *pinmux38 = pin_MUX.pin38;
    *pincfg38 = pin_CFG.pin38;
  }
  if ((pin_tracker >> 27) & 1) {
    pin40->CNF[0] = pin_CNF.pin40;
    pin40->DEB[0] = pin_DEB.pin40;
    pin40->OUT[0] = pin_OUT.pin40;
    pin40->OUT_VLE[0] = pin_OUT_VLE.pin40;
    *pinmux40 = pin_MUX.pin40;
    *pincfg40 = pin_CFG.pin40;
  }

  // Ummapping CNF AON registers
  munmap(baseCNF_AON, pagesize);
  // Ummapping CNF Non AON registers
  munmap(baseCNF_NAON, 5 * pagesize);
  // Ummapping PINMUX AON registers
  munmap(basePINMUX_AON, pagesize);
  // Ummapping PINMUX Audio registers
  munmap(basePINMUX_Audio, pagesize);
  // Ummapping PINMUX CAM registers
  munmap(basePINMUX_CAM, pagesize);
  // Ummapping PINMUX UART registers
  munmap(basePINMUX_UART, pagesize);
  // Ummapping PINMUX EDP registers
  munmap(basePINMUX_EDP, pagesize);
  // Ummapping PWM1 registers 
  munmap(basePWM1, pagesize);
  // Ummapping PWM8 registers 
  munmap(basePWM8, pagesize);
  // close /dev/mem 
  close(fd_GPIO);
}

int gpioSetMode(unsigned gpio, unsigned mode) {
  int status = 1;
  if (mode == JET_INPUT) {
    switch (gpio){
		
    case 7:
      *pinmux7 = PINMUXO_IN;
      *pincfg7 = CFGO_IN;
      pin7->CNF[0] = CNFO_IN;
      pin7->OUT[0] |= 0x00000001;
      pin_tracker |= (1 << 2);
      break;
    case 8:
      *pinmux8 = PINMUXO_IN;
      *pincfg8 = CFGO_IN;
      pin8->CNF[0] = CNFO_IN;
      pin8->OUT[0] |= 0x00000001;
      pin_tracker |= (1 << 3);
      break;
    case 10:
      *pinmux10 = PINMUXO_IN;
      *pincfg10 = CFGO_IN;
      pin10->CNF[0] = CNFO_IN;
      pin10->OUT[0] |= 0x00000001;
      pin_tracker |= (1 << 4);
      break;
    case 11:
      *pinmux11 = PINMUXO_IN;
      *pincfg11 = CFGO_IN;
      pin11->CNF[0] = CNFO_IN;
      pin11->OUT[0] |= 0x00000001;
      pin_tracker |= (1 << 5);
      break;
    case 12:
      *pinmux12 = PINMUXO_IN;
      *pincfg12 = CFGO_IN;
      pin12->CNF[0] = CNFO_IN;
      pin12->OUT[0] |= 0x00000001;
      pin_tracker |= (1 << 6);
      break;
    case 13:
      *pinmux13 = PINMUXO_IN;
      *pincfg13 = CFGO_IN;
      pin13->CNF[0] = CNFO_IN;
      pin13->OUT[0] |= 0x00000001;
      pin_tracker |= (1 << 7);
      break;
    case 15:
      *pinmux15 = PINMUXO_IN;
      *pincfg15 = CFGO_IN;
      pin15->CNF[0] = CNFO_IN;
      pin15->OUT[0] |= 0x00000001;
      pin_tracker |= (1 << 8);
      break;
    case 16:
      *pinmux16 = PINMUXO_IN;
      *pincfg16 = CFGO_IN;
      pin16->CNF[0] = CNFO_IN;
      pin16->OUT[0] |= 0x00000001;
      pin_tracker |= (1 << 9);
      break;
    case 18:
      *pinmux18 = PINMUXO_IN;
      *pincfg18 = CFGO_IN;
      pin18->CNF[0] = CNFO_IN;
      pin18->OUT[0] |= 0x00000001;
      pin_tracker |= (1 << 10);
      break;
    case 19:
      *pinmux19 = PINMUXO_IN;
      *pincfg19 = CFGO_IN;
      pin19->CNF[0] = CNFO_IN;
      pin19->OUT[0] |= 0x00000001;
      pin_tracker |= (1 << 11);
      break;
    case 21:
      *pinmux21 = PINMUXO_IN;
      *pincfg21 = CFGO_IN;
      pin21->CNF[0] = CNFO_IN;
      pin21->OUT[0] |= 0x00000001;
      pin_tracker |= (1 << 12);
      break;
    case 22:
      *pinmux22 = PINMUXO_IN;
      *pincfg22 = CFGO_IN;
      pin22->CNF[0] = CNFO_IN;
      pin22->OUT[0] |= 0x00000001;
      pin_tracker |= (1 << 13);
      break;
    case 23:
      *pinmux23 = PINMUXO_IN;
      *pincfg23 = CFGO_IN;
      pin23->CNF[0] = CNFO_IN;
      pin23->OUT[0] |= 0x00000001;
      pin_tracker |= (1 << 14);
      break;
    case 24:
      *pinmux24 = PINMUXO_IN;
      *pincfg24 = CFGO_IN;
      pin24->CNF[0] = CNFO_IN;
      pin24->OUT[0] |= 0x00000001;
      pin_tracker |= (1 << 15);
      break;
    case 26:
      *pinmux26 = PINMUXO_IN;
      *pincfg26 = CFGO_IN;
      pin26->CNF[0] = CNFO_IN;
      pin26->OUT[0] |= 0x00000001;
      pin_tracker |= (1 << 16);
      break;
    case 27:
      *pinmux27 = PINMUXO_IN;
      *pincfg27 = CFGO_IN;
      pin27->CNF[0] = CNFO_IN;
      pin27->OUT[0] |= 0x00000001;
      pin_tracker |= (1 << 17);
      break;
    case 28:
      *pinmux28 = PINMUXO_IN;
      *pincfg28 = CFGO_IN;
      pin28->CNF[0] = CNFO_IN;
      pin28->OUT[0] |= 0x00000001;
      pin_tracker |= (1 << 18);
      break;
    case 29:
      *pinmux29 = PINMUXO_IN;
      *pincfg29 = CFGO_IN;
      pin29->CNF[0] = CNFO_IN;
      pin29->OUT[0] |= 0x00000001;
      pin_tracker |= (1 << 19);
      break;
    case 31:
      *pinmux31 = PINMUXO_IN;
      *pincfg31 = CFGO_IN;
      pin31->CNF[0] = CNFO_IN;
      pin31->OUT[0] |= 0x00000001;
      pin_tracker |= (1 << 20);
      break;
    case 32:
      *pinmux32 = PINMUXO_IN;
      *pincfg32 = CFGO_IN;
      pin32->CNF[0] = CNFO_IN;
      pin32->OUT[0] |= 0x00000001;
      pin_tracker |= (1 << 21);
      break;
    case 33:
      *pinmux33 = PINMUXO_IN;
      *pincfg33 = CFGO_IN;
      pin33->CNF[0] = CNFO_IN;
      pin33->OUT[0] |= 0x00000001;
      pin_tracker |= (1 << 22);
      break;
    case 35:
      *pinmux35 = PINMUXO_IN;
      *pincfg35 = CFGO_IN;
      pin35->CNF[0] = CNFO_IN;
      pin35->OUT[0] |= 0x00000001;
      pin_tracker |= (1 << 23);
      break;
    case 36:
      *pinmux36 = PINMUXO_IN;
      *pincfg36 = CFGO_IN;
      pin36->CNF[0] = CNFO_IN;
      pin36->OUT[0] |= 0x00000001;
      pin_tracker |= (1 << 24);
      break;
    case 37:
      *pinmux37 = PINMUXO_IN;
      *pincfg37 = CFGO_IN;
      pin37->CNF[0] = CNFO_IN;
      pin37->OUT[0] |= 0x00000001;
      pin_tracker |= (1 << 25);
      break;
    case 38:
      *pinmux38 = PINMUXO_IN;
      *pincfg38 = CFGO_IN;
      pin38->CNF[0] = CNFO_IN;
      pin38->OUT[0] |= 0x00000001;
      pin_tracker |= (1 << 26);
      break;
    case 40:
      *pinmux40 = PINMUXO_IN;
      *pincfg40 = CFGO_IN;
      pin40->CNF[0] = CNFO_IN;
      pin40->OUT[0] |= 0x00000001;
      pin_tracker |= (1 << 27);
      break;
    default:
      status = -1;
      printf("Only gpio numbers from 7 to 40 are accepted, this function will read the level on the Jetson header pins,\n");
      printf("numbered as the header pin numbers e.g. AUD_MCLK is pin header number 7\n");
    }
  }
  else if (mode == JET_OUTPUT) {
    switch (gpio) {
		
    case 7:
      *pinmux7 = PINMUXO_OUT;
      *pincfg7 = CFGO_OUT;
      pin7->CNF[0] = CNFO_OUT;
      pin7->OUT[0] &= ~(0x00000001);
      pin_tracker |= (1 << 2);
      break;
    case 8:
      *pinmux8 = PINMUXO_OUT;
      *pincfg8 = CFGO_OUT;
      pin8->CNF[0] = CNFO_OUT;
      pin8->OUT[0] &= ~(0x00000001);
      pin_tracker |= (1 << 3);
      break;
    case 10:
      *pinmux10 = PINMUXO_OUT;
      *pincfg10 = CFGO_OUT;
      pin10->CNF[0] = CNFO_OUT;
      pin10->OUT[0] &= ~(0x00000001);
      pin_tracker |= (1 << 4);
      break;
    case 11:
      *pinmux11 = PINMUXO_OUT;
      *pincfg11 = CFGO_OUT;
      pin11->CNF[0] = CNFO_OUT;
      pin11->OUT[0] &= ~(0x00000001);
      pin_tracker |= (1 << 5);
      break;
    case 12:
      *pinmux12 = PINMUXO_OUT;
      *pincfg12 = CFGO_OUT;
      pin12->CNF[0] = CNFO_OUT;
      pin12->OUT[0] &= ~(0x00000001);
      pin_tracker |= (1 << 6);
      break;
    case 13:
      *pinmux13 = PINMUXO_OUT;
      *pincfg13 = CFGO_OUT;
      pin13->CNF[0] = CNFO_OUT;
      pin13->OUT[0] &= ~(0x00000001);
      pin_tracker |= (1 << 7);
      break;
    case 15:
      *pinmux15 = PINMUXO_OUT;
      *pincfg15 = CFGO_OUT;
      pin15->CNF[0] = CNFO_OUT;
      pin15->OUT[0] &= ~(0x00000001);
      pin_tracker |= (1 << 8);
      break;
    case 16:
      *pinmux16 = PINMUXO_OUT;
      *pincfg16 = CFGO_OUT;
      pin16->CNF[0] = CNFO_OUT;
      pin16->OUT[0] &= ~(0x00000001);
      pin_tracker |= (1 << 9);
      break;
    case 18:
      *pinmux18 = PINMUXO_OUT;
      *pincfg18 = CFGO_OUT;
      pin18->CNF[0] = CNFO_OUT;
      pin18->OUT[0] &= ~(0x00000001);
      pin_tracker |= (1 << 10);
      break;
    case 19:
      *pinmux19 = PINMUXO_OUT;
      *pincfg19 = CFGO_OUT;
      pin19->CNF[0] = CNFO_OUT;
      pin19->OUT[0] &= ~(0x00000001);
      pin_tracker |= (1 << 11);
      break;
    case 21:
      *pinmux21 = PINMUXO_OUT;
      *pincfg21 = CFGO_OUT;
      pin21->CNF[0] = CNFO_OUT;
      pin21->OUT[0] &= ~(0x00000001);
      pin_tracker |= (1 << 12);
      break;
    case 22:
      *pinmux22 = PINMUXO_OUT;
      *pincfg22 = CFGO_OUT;
      pin22->CNF[0] = CNFO_OUT;
      pin22->OUT[0] &= ~(0x00000001);
      pin_tracker |= (1 << 13);
      break;
    case 23:
      *pinmux23 = PINMUXO_OUT;
      *pincfg23 = CFGO_OUT;
      pin23->CNF[0] = CNFO_OUT;
      pin23->OUT[0] &= ~(0x00000001);
      pin_tracker |= (1 << 14);
      break;
    case 24:
      *pinmux24 = PINMUXO_OUT;
      *pincfg24 = CFGO_OUT;
      pin24->CNF[0] = CNFO_OUT;
      pin24->OUT[0] &= ~(0x00000001);
      pin_tracker |= (1 << 15);
      break;
    case 26:
      *pinmux26 = PINMUXO_OUT;
      *pincfg26 = CFGO_OUT;
      pin26->CNF[0] = CNFO_OUT;
      pin26->OUT[0] &= ~(0x00000001);
      pin_tracker |= (1 << 16);
      break;
    case 27:
      *pinmux27 = PINMUXO_OUT1;
      *pincfg27 = CFGO_OUT;
      pin27->CNF[0] = CNFO_OUT;
      pin27->OUT[0] &= ~(0x00000001);
      pin_tracker |= (1 << 17);
      break;
    case 28:
      *pinmux28 = PINMUXO_OUT1;
      *pincfg28 = CFGO_OUT;
      pin28->CNF[0] = CNFO_OUT;
      pin28->OUT[0] &= ~(0x00000001);
      pin_tracker |= (1 << 18);
      break;
    case 29:
      *pinmux29 = PINMUXO_OUT;
      *pincfg29 = CFGO_OUT;
      pin29->CNF[0] = CNFO_OUT;
      pin29->OUT[0] &= ~(0x00000001);
      pin_tracker |= (1 << 19);
      break;
    case 31:
      *pinmux31 = PINMUXO_OUT;
      *pincfg31 = CFGO_OUT;
      pin31->CNF[0] = CNFO_OUT;
      pin31->OUT[0] &= ~(0x00000001);
      pin_tracker |= (1 << 20);
      break;
    case 32:
      *pinmux32 = PINMUXO_OUT;
      *pincfg32 = CFGO_OUT;
      pin32->CNF[0] = CNFO_OUT;
      pin32->OUT[0] &= ~(0x00000001);
      pin_tracker |= (1 << 21);
      break;
    case 33:
      *pinmux33 = PINMUXO_OUT;
      *pincfg33 = CFGO_OUT;
      pin33->CNF[0] = CNFO_OUT;
      pin33->OUT[0] &= ~(0x00000001);
      pin_tracker |= (1 << 22);
      break;
    case 35:
      *pinmux35 = PINMUXO_OUT;
      *pincfg35 = CFGO_OUT;
      pin35->CNF[0] = CNFO_OUT;
      pin35->OUT[0] &= ~(0x00000001);
      pin_tracker |= (1 << 23);
      break;
    case 36:
      *pinmux36 = PINMUXO_OUT;
      *pincfg36 = CFGO_OUT;
      pin36->CNF[0] = CNFO_OUT;
      pin36->OUT[0] &= ~(0x00000001);
      pin_tracker |= (1 << 24);
      break;
    case 37:
      *pinmux37 = PINMUXO_OUT;
      *pincfg37 = CFGO_OUT;
      pin37->CNF[0] = CNFO_OUT;
      pin37->OUT[0] &= ~(0x00000001);
      pin_tracker |= (1 << 25);
      break;
    case 38:
      *pinmux38 = PINMUXO_OUT;
      *pincfg38 = CFGO_OUT;
      pin38->CNF[0] = CNFO_OUT;
      pin38->OUT[0] &= ~(0x00000001);
      pin_tracker |= (1 << 26);
      break;
    case 40:
      *pinmux40 = PINMUXO_OUT;
      *pincfg40 = CFGO_OUT;
      pin40->CNF[0] = CNFO_OUT;
      pin40->OUT[0] &= ~(0x00000001);
      pin_tracker |= (1 << 27);
      break;
    default:
      status = -2;
      printf("Only gpio numbers from 7 to 40 are accepted, this function will only write the level on the Jetson header pins,\n");
      printf("numbered as the header pin numbers e.g. AUD_MCLK is pin header number 7\n");
    }
  }
  else { printf("Only modes allowed are JET_INPUT and JET_OUTPUT\n");
    status = -3;
  }
  return status;	
}

int gpioRead(unsigned gpio) {
  int level = 0;
  switch (gpio){
      
  case 7:
    level = pin7->IN[0] & 1;
    break;
  case 8:
    level = pin8->IN[0] & 1;
    break;
  case 10:
    level = pin10->IN[0] & 1;
    break;
  case 11:
    level = pin11->IN[0] & 1;
    break;
  case 12:
    level = pin12->IN[0] & 1;
    break;
  case 13:
    level = pin13->IN[0] & 1;
    break;
  case 15:
    level = pin15->IN[0] & 1;
    break;
  case 16:
    level = pin16->IN[0] & 1;
    break;
  case 18:
    level = pin18->IN[0] & 1;
    break;
  case 19:
    level = pin19->IN[0] & 1;
    break;
  case 21:
    level = pin21->IN[0] & 1;
    break;
  case 22:
    level = pin22->IN[0] & 1;
    break;
  case 23:
    level = pin23->IN[0] & 1;
    break;
  case 24:
    level = pin24->IN[0] & 1;
    break;
  case 26:
    level = pin26->IN[0] & 1;
    break;
  case 27:
    level = pin27->IN[0] & 1;
    break;
  case 28:
    level = pin28->IN[0] & 1;
    break;
  case 29:
    level = pin29->IN[0] & 1;
    break;
  case 31:
    level = pin31->IN[0] & 1;
    break;
  case 32:
    level = pin32->IN[0] & 1;
    break;
  case 33:
    level = pin33->IN[0] & 1;
    break;
  case 35:
    level = pin35->IN[0] & 1;
    break;
  case 36:
    level = pin36->IN[0] & 1;
    break;
  case 37:
    level = pin37->IN[0] & 1;
    break;
  case 38:
    level = pin38->IN[0] & 1;
    break;
  case 40:
    level = pin40->IN[0] & 1;
    break;
  default:
    level = -1;
    printf("Only gpio numbers from 7 to 40 are accepted, this function will only read the level of the Jetson header pins,\n");
    printf("numbered as the header pin numbers e.g. AUD_MCLK is pin header number 7\n");
  }
  return level;
}

int gpioWrite(unsigned gpio, unsigned level) {
  int status = 1;
  if (level == 0) {
    switch (gpio){

    case 7:
      pin7->OUT_VLE[0] &= level;
      break;
    case 8:
      pin8->OUT_VLE[0] &= level;
      break;
    case 10:
      pin10->OUT_VLE[0] &= level;
      break;
    case 11:
      pin11->OUT_VLE[0] &= level;
      break;
    case 12:
      pin12->OUT_VLE[0] &= level;
      break;
    case 13:
      pin13->OUT_VLE[0] &= level;
      break;
    case 15:
      pin15->OUT_VLE[0] &= level;
      break;
    case 16:
      pin16->OUT_VLE[0] &= level;
      break;
    case 18:
      pin18->OUT_VLE[0] &= level;
      break;
    case 19:
      pin19->OUT_VLE[0] &= level;
      break;
    case 21:
      pin21->OUT_VLE[0] &= level;
      break;
    case 22:
      pin22->OUT_VLE[0] &= level;
      break;
    case 23:
      pin23->OUT_VLE[0] &= level;
      break;
    case 24:
      pin24->OUT_VLE[0] &= level;
      break;
    case 26:
      pin26->OUT_VLE[0] &= level;
      break;
    case 27:
      pin27->OUT_VLE[0] &= level;
      break;
    case 28:
      pin28->OUT_VLE[0] &= level;
      break;
    case 29:
      pin29->OUT_VLE[0] &= level;
      break;
    case 31:
      pin31->OUT_VLE[0] &= level;
      break;
    case 32:
      pin32->OUT_VLE[0] &= level;
      break;
    case 33:
      pin33->OUT_VLE[0] &= level;
      break;
    case 35:
      pin35->OUT_VLE[0] &= level;
      break;
    case 36:
      pin36->OUT_VLE[0] &= level;
      break;
    case 37:
      pin37->OUT_VLE[0] &= level;
      break;
    case 38:
      pin38->OUT_VLE[0] &= level;
      break;
    case 40:
      pin40->OUT_VLE[0] &= level;
      break;
    default:
      status = -1;
      printf("Only gpio numbers from 7 to 40 are accepted, this function will only read the level of the Jetson header pins,\n");
      printf("numbered as the header pin numbers e.g. AUD_MCLK is pin header number 7\n");
    }
  }
  else if (level == 1) {
    switch (gpio){
		
    case 7:
      pin7->OUT_VLE[0] |= level;
      break;
    case 8:
      pin8->OUT_VLE[0] |= level;
      break;
    case 10:
      pin10->OUT_VLE[0] |= level;
      break;
    case 11:
      pin11->OUT_VLE[0] |= level;
      break;
    case 12:
      pin12->OUT_VLE[0] |= level;
      break;
    case 13:
      pin13->OUT_VLE[0] |= level;
      break;
    case 15:
      pin15->OUT_VLE[0] |= level;
      break;
    case 16:
      pin16->OUT_VLE[0] |= level;
      break;
    case 18:
      pin18->OUT_VLE[0] |= level;
      break;
    case 19:
      pin19->OUT_VLE[0] |= level;
      break;
    case 21:
      pin21->OUT_VLE[0] |= level;
      break;
    case 22:
      pin22->OUT_VLE[0] |= level;
      break;
    case 23:
      pin23->OUT_VLE[0] |= level;
      break;
    case 24:
      pin24->OUT_VLE[0] |= level;
      break;
    case 26:
      pin26->OUT_VLE[0] |= level;
      break;
    case 27:
      pin27->OUT_VLE[0] |= level;
      break;
    case 28:
      pin28->OUT_VLE[0] |= level;
      break;
    case 29:
      pin29->OUT_VLE[0] |= level;
      break;
    case 31:
      pin31->OUT_VLE[0] |= level;
      break;
    case 32:
      pin32->OUT_VLE[0] |= level;
      break;
    case 33:
      pin33->OUT_VLE[0] |= level;
      break;
    case 35:
      pin35->OUT_VLE[0] |= level;
      break;
    case 36:
      pin36->OUT_VLE[0] |= level;
      break;
    case 37:
      pin37->OUT_VLE[0] |= level;
      break;
    case 38:
      pin38->OUT_VLE[0] |= level;
      break;
    case 40:
      pin40->OUT_VLE[0] |= level;
      break;
    default:
      status = -2;
      printf("Only gpio numbers from 7 to 40 are accepted, this function will only read the level of the Jetson header pins,\n");
      printf("numbered as the header pin numbers e.g. AUD_MCLK is pin header number 7\n");
    }
  }
  else {printf("Only levels 0 or 1 are allowed\n");
    status = -3;
  }
  return status;
}

void *callback(void *arg) {
  ISRFunc *int_struct = (ISRFunc *) arg;
  unsigned gpio = int_struct->gpio;
  int edge = int_struct->edge;
  unsigned gpio_offset = int_struct->gpio_offset;
  uint64_t *timestamp = int_struct->timestamp;
  *timestamp = 0;
  uint64_t timestamp_new = 0;
  unsigned debounce = int_struct->debounce;
  char dev[20];
  int fd;
  int ret;
  struct timespec start;

  struct gpioevent_request req = {0};
  struct gpioevent_data event = {0};
  
  
  if (gpio == 15 || gpio == 27 || gpio == 28) {
#if LINUX_VERSION_CODE < KERNEL_VERSION(5,1,0)
    strcpy(dev, "/dev/gpiochip1");
#else
    strcpy(dev, "/dev/gpiochip2");
#endif
  }
  else {
#if LINUX_VERSION_CODE < KERNEL_VERSION(5,1,0)
    strcpy(dev, "/dev/gpiochip0");
#else
    strcpy(dev, "/dev/gpiochip1");
#endif
  }
  
  fd = open(dev, O_RDONLY);
  if (fd < 0) {
    printf("Bad handle (%d)\n", fd);
    pthread_exit(NULL);	
  }

  req.lineoffset = gpio_offset;
  req.handleflags = GPIOHANDLE_REQUEST_INPUT;
  req.eventflags = edge;
  strncpy(req.consumer_label, "gpio_event", sizeof(req.consumer_label) - 1);
    
  ret = ioctl(fd, GPIO_GET_LINEEVENT_IOCTL, &req);
  if (ret == -1) {
    ret = -errno;
    printf("Failed to issue GET EVENT ""IOCTL (%d)\n",ret);
    close(fd);
    pthread_exit(NULL);
  }
  close(fd);
  
  while (global_int) {
    ret = read(req.fd, &event, sizeof(event));
    if ((ret == -1) || (ret != sizeof(event))) {
      ret = -errno;
      printf("Failed to read event (%d)\n", ret);
      break;
    }
    /* TO COMPLETE COMMENT HERE
     */
    clock_gettime(CLOCK_REALTIME, &start);			       
    timestamp_new = BILLION * (start.tv_sec) + start.tv_nsec;

    if ((timestamp_new - *timestamp) > (debounce * 100)) {
      *timestamp = timestamp_new;
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
    
int gpioSetISRFunc(unsigned gpio, unsigned edge, unsigned debounce, unsigned long *timestamp, void (*f)()) {
  int status = 1;
  unsigned gpio_offset = 0;

  if (debounce < 0 || debounce > 1000) {
    printf( "Debounce setting should be a number between 0 and 1000 useconds\n");
    status = -1;
  }
  
  if (edge == RISING_EDGE || edge == FALLING_EDGE || edge == EITHER_EDGE) {

    switch (gpio) {
		
    case 7:
      pin7->CNF[0] = (edge == RISING_EDGE ? 0xd9 : (edge == FALLING_EDGE ? 0xc9 : 0xcd));
      pin7->INT_CLR[0] |= 1;
#if LINUX_VERSION_CODE < KERNEL_VERSION(5,1,0)
      gpio_offset = 148;
#else
      gpio_offset = 118;
#endif
      break;
    case 8:
      pin8->CNF[0] = (edge == RISING_EDGE ? 0xd9 : (edge == FALLING_EDGE ? 0xc9 : 0xcd));
      pin8->INT_CLR[0] |= 1;
#if LINUX_VERSION_CODE < KERNEL_VERSION(5,1,0)
      gpio_offset = 138;
#else
      gpio_offset = 110;
#endif
      break;
    case 10:
      pin10->CNF[0] = (edge == RISING_EDGE ? 0xd9 : (edge == FALLING_EDGE ? 0xc9 : 0xcd));
      pin10->INT_CLR[0] |= 1;
#if LINUX_VERSION_CODE < KERNEL_VERSION(5,1,0)
      gpio_offset = 139;
#else
      gpio_offset = 111;
#endif
      break;
    case 11:
      pin11->CNF[0] = (edge == RISING_EDGE ? 0xd9 : (edge == FALLING_EDGE ? 0xc9 : 0xcd));
      pin11->INT_CLR[0] |= 1;
#if LINUX_VERSION_CODE < KERNEL_VERSION(5,1,0)
      gpio_offset = 140;
#else
      gpio_offset = 112;
#endif
      break;
    case 12:
      pin12->CNF[0] = (edge == RISING_EDGE ? 0xd9 : (edge == FALLING_EDGE ? 0xc9 : 0xcd));
      pin12->INT_CLR[0] |= 1;
#if LINUX_VERSION_CODE < KERNEL_VERSION(5,1,0)
      gpio_offset = 157;
#else
      gpio_offset = 127;
#endif
      break;
    case 13:
      pin13->CNF[0] = (edge == RISING_EDGE ? 0xd9 : (edge == FALLING_EDGE ? 0xc9 : 0xcd));
      pin13->INT_CLR[0] |= 1;
#if LINUX_VERSION_CODE < KERNEL_VERSION(5,1,0)
      gpio_offset = 192;
#else
      gpio_offset = 149;
#endif
      break;
    case 15:
      pin15->CNF[0] = (edge == RISING_EDGE ? 0xd9 : (edge == FALLING_EDGE ? 0xc9 : 0xcd));
      pin15->INT_CLR[0] |= 1;
#if LINUX_VERSION_CODE < KERNEL_VERSION(5,1,0)
      gpio_offset = 20;
#else
      gpio_offset = 16;
#endif
      break;
    case 16:
      pin16->CNF[0] = (edge == RISING_EDGE ? 0xd9 : (edge == FALLING_EDGE ? 0xc9 : 0xcd));
      pin16->INT_CLR[0] |= 1;
#if LINUX_VERSION_CODE < KERNEL_VERSION(5,1,0)
      gpio_offset = 196;
#else
      gpio_offset = 153;
#endif
      break;
    case 18:
      pin18->CNF[0] = (edge == RISING_EDGE ? 0xd9 : (edge == FALLING_EDGE ? 0xc9 : 0xcd));
      pin18->INT_CLR[0] |= 1;
#if LINUX_VERSION_CODE < KERNEL_VERSION(5,1,0)
      gpio_offset = 195;
#else
      gpio_offset = 152;
#endif
      break;
    case 19:
      pin19->CNF[0] = (edge == RISING_EDGE ? 0xd9 : (edge == FALLING_EDGE ? 0xc9 : 0xcd));
      pin19->INT_CLR[0] |= 1;
#if LINUX_VERSION_CODE < KERNEL_VERSION(5,1,0)
      gpio_offset = 205;
#else
      gpio_offset = 162;
#endif
      break;
    case 21:
      pin21->CNF[0] = (edge == RISING_EDGE ? 0xd9 : (edge == FALLING_EDGE ? 0xc9 : 0xcd));
      pin21->INT_CLR[0] |= 1;
#if LINUX_VERSION_CODE < KERNEL_VERSION(5,1,0)
      gpio_offset = 204;
#else
      gpio_offset = 161;
#endif
      break;
    case 22:
      pin22->CNF[0] = (edge == RISING_EDGE ? 0xd9 : (edge == FALLING_EDGE ? 0xc9 : 0xcd));
      pin22->INT_CLR[0] |= 1;
#if LINUX_VERSION_CODE < KERNEL_VERSION(5,1,0)
      gpio_offset = 193;
#else
      gpio_offset = 150;
#endif
      break;
    case 23:
      pin23->CNF[0] = (edge == RISING_EDGE ? 0xd9 : (edge == FALLING_EDGE ? 0xc9 : 0xcd));
      pin23->INT_CLR[0] |= 1;
#if LINUX_VERSION_CODE < KERNEL_VERSION(5,1,0)
      gpio_offset = 203;
#else
      gpio_offset = 160;
#endif
      break;
    case 24:
      pin24->CNF[0] = (edge == RISING_EDGE ? 0xd9 : (edge == FALLING_EDGE ? 0xc9 : 0xcd));
      pin24->INT_CLR[0] |= 1;
#if LINUX_VERSION_CODE < KERNEL_VERSION(5,1,0)
      gpio_offset = 206;
#else
      gpio_offset = 163;
#endif
      break;
    case 26:
      pin26->CNF[0] = (edge == RISING_EDGE ? 0xd9 : (edge == FALLING_EDGE ? 0xc9 : 0xcd));
      pin26->INT_CLR[0] |= 1;
#if LINUX_VERSION_CODE < KERNEL_VERSION(5,1,0)
      gpio_offset = 207;
#else
      gpio_offset = 164;
#endif
      break;
    case 27:
      pin27->CNF[0] = (edge == RISING_EDGE ? 0xd9 : (edge == FALLING_EDGE ? 0xc9 : 0xcd));
      pin27->INT_CLR[0] |= 1;
#if LINUX_VERSION_CODE < KERNEL_VERSION(5,1,0)
      gpio_offset = 24;
#else
      gpio_offset = 20;
#endif
      break;
    case 28:
      pin28->CNF[0] = (edge == RISING_EDGE ? 0xd9 : (edge == FALLING_EDGE ? 0xc9 : 0xcd));
      pin28->INT_CLR[0] |= 1;
#if LINUX_VERSION_CODE < KERNEL_VERSION(5,1,0)
      gpio_offset = 23;
#else
      gpio_offset = 19;
#endif
      break;
    case 29:
      pin29->CNF[0] = (edge == RISING_EDGE ? 0xd9 : (edge == FALLING_EDGE ? 0xc9 : 0xcd));
      pin29->INT_CLR[0] |= 1;
#if LINUX_VERSION_CODE < KERNEL_VERSION(5,1,0)
      gpio_offset = 133;
#else
      gpio_offset = 105;
#endif
      break;
    case 31:
      pin31->CNF[0] = (edge == RISING_EDGE ? 0xd9 : (edge == FALLING_EDGE ? 0xc9 : 0xcd));
      pin31->INT_CLR[0] |= 1;
#if LINUX_VERSION_CODE < KERNEL_VERSION(5,1,0)
      gpio_offset = 134;
#else
      gpio_offset = 106;
#endif
      break;
    case 32:
      pin32->CNF[0] = (edge == RISING_EDGE ? 0xd9 : (edge == FALLING_EDGE ? 0xc9 : 0xcd));
      pin32->INT_CLR[0] |= 1;
#if LINUX_VERSION_CODE < KERNEL_VERSION(5,1,0)
      gpio_offset = 136;
#else
      gpio_offset = 108;
#endif
      break;
    case 33:
      pin33->CNF[0] = (edge == RISING_EDGE ? 0xd9 : (edge == FALLING_EDGE ? 0xc9 : 0xcd));
      pin33->INT_CLR[0] |= 1;
#if LINUX_VERSION_CODE < KERNEL_VERSION(5,1,0)
      gpio_offset = 105;
#else
      gpio_offset = 84;
#endif
      break;
    case 35:
      pin35->CNF[0] = (edge == RISING_EDGE ? 0xd9 : (edge == FALLING_EDGE ? 0xc9 : 0xcd));
      pin35->INT_CLR[0] |= 1;
#if LINUX_VERSION_CODE < KERNEL_VERSION(5,1,0)
      gpio_offset = 160;
#else
      gpio_offset = 130;
#endif
      break;
    case 36:
      pin36->CNF[0] = (edge == RISING_EDGE ? 0xd9 : (edge == FALLING_EDGE ? 0xc9 : 0xcd));
      pin36->INT_CLR[0] |= 1;
#if LINUX_VERSION_CODE < KERNEL_VERSION(5,1,0)
      gpio_offset = 141;
#else
      gpio_offset = 113;
#endif
      break;
    case 37:
      pin37->CNF[0] = (edge == RISING_EDGE ? 0xd9 : (edge == FALLING_EDGE ? 0xc9 : 0xcd));
      pin37->INT_CLR[0] |= 1;
#if LINUX_VERSION_CODE < KERNEL_VERSION(5,1,0)
      gpio_offset = 194;
#else
      gpio_offset = 151;
#endif
      break;
    case 38:
      pin38->CNF[0] = (edge == RISING_EDGE ? 0xd9 : (edge == FALLING_EDGE ? 0xc9 : 0xcd));
      pin38->INT_CLR[0] |= 1;
#if LINUX_VERSION_CODE < KERNEL_VERSION(5,1,0)
      gpio_offset = 159;
#else
      gpio_offset = 129;
#endif
      break;
    case 40:
      pin40->CNF[0] = (edge == RISING_EDGE ? 0xd9 : (edge == FALLING_EDGE ? 0xc9 : 0xcd));
      pin40->INT_CLR[0] |= 1;
#if LINUX_VERSION_CODE < KERNEL_VERSION(5,1,0)
      gpio_offset = 158;
#else
      gpio_offset = 128;
#endif
      break;
    default:
      status = -2;
      printf("Only gpio numbers from 7 to 40 are accepted\n");
    }
  }
  
  else {printf("Edge should be: RISING_EDGE,FALLING_EDGE or EITHER_EDGE\n");
    status = -3;
  }
  if (ISRFunc_CFG[gpio]->gpio != 0) {
    printf("Input pin %d is already being monitored for interruptions\n", gpio);
    status = -4;
  }
  else {
    ISRFunc_CFG[gpio]->gpio = gpio;
    ISRFunc_CFG[gpio]->f = f;
    ISRFunc_CFG[gpio]->edge = edge;
    ISRFunc_CFG[gpio]->gpio_offset = gpio_offset;
    ISRFunc_CFG[gpio]->stat_reg = 0;
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

int gpioSetPWMfrequency(unsigned gpio, unsigned frequency) {
  int status = 1;
  int PFM = 0;
  unsigned clk_rate_PWM;
  char buf[100];
  FILE *fptr;

  if ((frequency >= 50) && (frequency <=1593000)) {
    if (frequency > 796875) {
      clk_rate_PWM = 408000000; 
      PFM = round(1593750.0/(double)frequency)-1;
    }
    
    if ((frequency >= 98) && (frequency <= 796875)) {
      clk_rate_PWM = 204000000;
      PFM = round(796875.0/(double)frequency)-1;
    }
    
    if (frequency < 98) {
      clk_rate_PWM = 102000000;
      PFM = round(398437.5/(double)frequency)-1;
    }
    switch(gpio) {
   
    case 32:
      snprintf(buf, sizeof(buf), "/sys/kernel/debug/bpmp/debug/clk/pwm8/rate");
      fptr = fopen(buf, "r");
      
      if (fptr == NULL) {
        printf("Not possible to read current clock rate on pwm8\n");
      }
	
      fscanf(fptr, "%u", &clk_rate_PWM8);
      
      snprintf(buf, sizeof(buf), "echo %u > /sys/kernel/debug/bpmp/debug/clk/pwm8/rate", clk_rate_PWM);
      if (system(buf) == -1) { 
        printf( "Not possible to change clock rate on pwm8\n");
      }
      fclose(fptr);
      *pinmux32 = 0x00000401;
      *pincfg32 = CFGO_OUT;
      pin32->CNF[0] = 0x00000001;
      *PWM8 = 0x0;
      *PWM8 = PFM;
      pin_tracker |= (1 << 30);
      break;
    case 33:
      snprintf(buf, sizeof(buf), "/sys/kernel/debug/bpmp/debug/clk/pwm1/rate");
      fptr = fopen(buf, "r");
      
      if (fptr == NULL) {
        printf("Not possible to read current clock rate on pwm1\n");
      }
	
      fscanf(fptr, "%u", &clk_rate_PWM1);
      
      snprintf(buf, sizeof(buf), "echo %u > /sys/kernel/debug/bpmp/debug/clk/pwm1/rate", clk_rate_PWM);
      if (system(buf) == -1) { 
        printf( "Not possible to change clock rate on pwm1\n");
      }
      fclose(fptr);
      *pinmux33 = 0x00000402;
      *pincfg33 = CFGO_OUT;
      pin33->CNF[0] = 0x00000001;
      *PWM1 = 0x0;
      *PWM1 = PFM;
      pin_tracker |= (1 << 29);
      break;
    default:
      status = -1;
      printf("Only gpio numbers 32 and 33 are accepted\n");
    }  		
  }
  else {printf("Only frequencies from 50 to 1595000 Hz are allowed\n");
    status =-2;}
  return status;
}

int gpioPWM(unsigned gpio, unsigned dutycycle) {
  int status = 1;
  
  if ((dutycycle >= 0) && (dutycycle <=256)) {
    switch (gpio) {
 
    case 32:
      *PWM8 &= ~(0xFFFF0000);
      *PWM8 |= dutycycle<<16;
      *PWM8 |= 0x80000000;
      break;
    case 33:
      *PWM1 &= ~(0xFFFF0000);
      *PWM1 |= dutycycle<<16;
      *PWM1 |= 0x80000000;
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

int i2c_smbus_access(int file, char read_write, __u8 command, int size, union i2c_smbus_data *data) {
  struct i2c_smbus_ioctl_data args;
  args.read_write = read_write;
  args.command = command;
  args.size = size;
  args.data = data;
  return ioctl(file,I2C_SMBUS,&args);
}

int i2cOpen(unsigned i2cBus, unsigned i2cFlags) {
  char dev[20], buf[100];
  int fd, slot, speed;
  uint32_t funcs;

  if (!(i2cBus == 0 || i2cBus == 1)) {
    printf("Bad i2c device (%d) only 0 or 1 are accepted\n", i2cBus);
    return -1;
  }

  if (i2cBus == 0) {
    i2cBus = 1;
  }
  else {
    i2cBus = 8;
  }
  
  if (!(i2cFlags == 0 || i2cFlags == 1 || i2cFlags == 2)) {
    printf("Only flags 0 to 2 are supported to set up bus speed\n");
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

  speed = speed * 1;
  
  slot = -5;
	
  if (i2cInfo[i2cBus].state == I2C_CLOSED) {
    slot = i2cBus;
    i2cInfo[slot].state = I2C_RESERVED;
  }
  else { printf("i2c bus already open\n");
    return -3;
  }
  
  FILE *fptr;
  snprintf(buf, sizeof(buf), "/sys/bus/i2c/devices/i2c-%d/bus_clk_rate", i2cBus);
  fptr = fopen(buf, "r");
	
  if (fptr == NULL) {
    printf("Not possible to read current bus speed\n");
    return -4;
  }
	
  fscanf(fptr, "%d", &i2c_speed[i2cBus]);
   	
  snprintf(buf, sizeof(buf), "echo %d > /sys/bus/i2c/devices/i2c-%d/bus_clk_rate", speed, i2cBus);
  if (system(buf) == -1) { 
    printf( "Not possible to change bus speed\n");
    return -7;
  }

  fclose(fptr);
  
  strcpy(buf, "modprobe i2c_dev");
    
  if (system(buf) == -1) { /* Ignore errors */
  }

  snprintf(dev, 19, "/dev/i2c-%d", i2cBus);
  fd = open(dev, O_RDWR);
  if (fd < 0) {
    printf( "Bad handle (%d)\n", fd);
    return -4;	
  }

  if (ioctl(fd, I2C_FUNCS, &funcs) < 0) {
    funcs = -1; /* assume all smbus commands allowed */
    return -6;
  }

  i2cInfo[slot].fd = fd;
  i2cInfo[slot].flags = i2cFlags;
  i2cInfo[slot].funcs = funcs;
  i2cInfo[slot].state = I2C_OPENED;

  return slot;
}

int i2cClose(unsigned handle) {	
  if (!(handle == 1 || handle == 8)) {
    printf( "Bad handle (%d)", handle);
    return -1;
  }

  if (i2cInfo[handle].state != I2C_OPENED) {
    printf( "I2c bus is already closed (%d)", handle);
    return -2;	
  }
     
  if (i2cInfo[handle].fd >= 0) {close(i2cInfo[handle].fd);}

  i2cInfo[handle].fd = -1;
  i2cInfo[handle].state = I2C_CLOSED;

  char buf[100];
  snprintf(buf, sizeof(buf), "echo %d > /sys/bus/i2c/devices/i2c-%d/bus_clk_rate", i2c_speed[handle], handle);
  if (system(buf) == -1) { 
    printf( "Not possible to return bus speed to original value\n");
    return -1;
  }
  
  return 0;
}

int i2cWriteByteData(unsigned handle, unsigned i2cAddr, unsigned reg, unsigned bVal) {	
  union i2c_smbus_data data;
  int status = 0;
	
  if (!(handle == 1 || handle == 8)) {
    printf( "Bad handle (%d)\n", handle);
    status = -1;
  }

  if (i2cInfo[handle].state != I2C_OPENED) {
    printf( "I2c%d is not open\n", handle);
    status = -2;
  }

  if (i2cAddr > 0x7f) {
    printf( "Bad I2C address (%d)\n", i2cAddr);
    status = -3;
  }
	
  if (reg > 0xFF) {
    printf( "Register address on device bigger than 0xFF\n");
    status = -4;
  }

  if (bVal > 0xFF) {
    printf( "Value to be written bigger than byte\n");
    status = -5;
  }

  i2cInfo[handle].addr = i2cAddr;

  if (ioctl(i2cInfo[handle].fd, I2C_SLAVE, i2cAddr) < 0) {
    printf( "I2C slave address not found on bus\n");
    status = -6;
  }

  if ((i2cInfo[handle].funcs & I2C_FUNC_SMBUS_WRITE_BYTE_DATA) == 0) {
    printf( "Write byte data function not supported by device\n");
    status = -7;
  }
	
  data.byte = bVal;
    
  if (i2c_smbus_access(i2cInfo[handle].fd,I2C_SMBUS_WRITE, reg, I2C_SMBUS_BYTE_DATA, &data)<0) {
    printf( "Not possible to write register\n");
    status = -8;}
  return status;
}

int i2cReadByteData(unsigned handle, unsigned i2cAddr, unsigned reg) {
  int status = 0;
  union i2c_smbus_data data;
	
  if (!(handle == 1 || handle == 8)) {
    printf( "Bad handle (%d)\n", handle);
    status = -1;
  }

  if (i2cInfo[handle].state != I2C_OPENED) {
    printf( "I2c%d is not open\n", handle);
    status = -2;
  }
    
  if (i2cAddr > 0x7f) {
    printf( "Bad I2C address (%d)\n", i2cAddr);
    status = -3;
  }

  if (reg > 0xFF) {
    printf( "Register address on device bigger than 0xFF\n");
    status = -4;
  }

  i2cInfo[handle].addr = i2cAddr;

  if (ioctl(i2cInfo[handle].fd, I2C_SLAVE, i2cAddr) < 0) {
    printf( "I2C slave address not found on bus\n");
    status = -5;
  }
    
  if ((i2cInfo[handle].funcs & I2C_FUNC_SMBUS_READ_BYTE_DATA) == 0) {
    printf( "Write byte data function not supported by device\n");
    status = -6;
  }
	
  if (i2c_smbus_access(i2cInfo[handle].fd,I2C_SMBUS_READ, reg, I2C_SMBUS_BYTE_DATA,&data)<0) {
    printf( "Not possible to read register\n");
    status = -7;}
  else {
    status = 0x0FF & data.byte;
  }
  return status;
}

int i2cWriteWordData(unsigned handle, unsigned i2cAddr, unsigned reg, unsigned wVal) {
    
  union i2c_smbus_data data;
  int status = 0;
	
  if (!(handle == 1 || handle == 8)) {
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

int i2cReadWordData(unsigned handle, unsigned i2cAddr, unsigned reg) {
    
  int status = 0;
  union i2c_smbus_data data;
	
  if (!(handle == 1 || handle == 8)) {
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

int spiOpen(unsigned spiChan, unsigned speed, unsigned mode, unsigned cs_delay, unsigned bits_word, unsigned lsb_first, unsigned cs_change) {
  char dev[20], buf[100];
  int fd, slot;
  int ret = 0;

  if (!(spiChan == 0 || spiChan == 1)) {
    printf( "Bad spi device (%d) only 0 or 1 are accepted\n", spiChan);
    return -1;
  }

  if (spiChan == 1) {
    spiChan ++; 
  }
  
  if (speed < 0 || speed > 50000000) {
    printf( "Speed in bits/second (%d) shouldn't be bigger that 50 Mbit/s\n", speed);
    return -2;
  }

  if (!(mode == 0 || mode == 1 || mode == 2 || mode == 3)) {
    printf( "Mode (%d) should be a number between 0 and 3\n", mode);
    return -3;
  }

  if (cs_delay < 0 || cs_delay > 1000) {
    printf( "Cs_delay in us (%d) shouldn't be bigger that 1000 us\n", cs_delay);
    return -5;
  }

  if (bits_word < 0 || bits_word > 32) {
    printf( "Bits per word (%d) should be a number between 0 and 32\n", bits_word);
    return -6;
  }

  if (!(lsb_first == 0 || lsb_first == 1)) {
    printf( "Least significant bit first option (%d) should be 0 or 1\n", lsb_first);
    return -7;
  }

  if (!(cs_change == 0 || cs_change == 1)) {
    printf( "Cs_change option (%d) should be 0 or 1\n", cs_change);
    return -10;
  }
    
  slot = -22;    
    
  if (SpiInfo[spiChan].state == SPI_CLOSED) {
    slot = spiChan;
    SpiInfo[slot].state = SPI_RESERVED;
  }
  else {
    printf("Spi bus already open\n");
    return -11;
  }
    
  if (spiChan == 0) {
    *pinmux19 = 0x00000400;
    *pincfg19 = CFGO_OUT;
    pin19->CNF[0] |= 0x00000003;
    *pinmux21 = 0x00000440;
    *pincfg21 = CFGO_IN;
    pin21->CNF[0] |= 0x00000001;
    *pinmux23 = 0x00000400;
    *pincfg23 = CFGO_OUT;
    pin23->CNF[0] |= 0x00000003;
    *pinmux24 = 0x00000400;
    *pincfg24 = CFGO_OUT;
    pin24->CNF[0] |= 0x00000003;
    pin_tracker |= (1 << 31);
  }
  
  if (spiChan == 1 || spiChan == 2) {
    *pinmux37 = 0x00000400;
    *pincfg37 = CFGO_OUT;
    pin37->CNF[0] |= 0x00000003;
    *pinmux22 = 0x00000440;
    *pincfg22 = CFGO_IN;
    pin22->CNF[0] |= 0x00000001;
    *pinmux13 = 0x00000400;
    *pincfg13 = CFGO_OUT;
    pin13->CNF[0] |= 0x00000003;
    *pinmux18 = 0x00000400;
    *pincfg18 = CFGO_OUT;
    pin18->CNF[0] |= 0x00000003; 
    pin_tracker |= (1UL << 32);
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
  if (ret < 0) {
    printf("Can't set spi mode\n");
    return -14;
  }
    
  ret = ioctl(fd, SPI_IOC_RD_MODE, &mode);
  if (ret < 0) {
    printf("Can't get spi mode\n");
    return -15; 
  }
    
  ret = ioctl(fd, SPI_IOC_WR_BITS_PER_WORD, &bits_word);
  if (ret < 0) {
    printf("Can't set bits per word\n");
    return -16;
  }
    
  ret = ioctl(fd, SPI_IOC_RD_BITS_PER_WORD, &bits_word);
  if (ret < 0) {
    printf("Can't get bits per word\n");
    return -17;
  }
  
  ret = ioctl(fd, SPI_IOC_WR_MAX_SPEED_HZ, &speed);
  if (ret < 0) {
    printf("Can't set max speed hz");
    return -18;
  }

  ret = ioctl(fd, SPI_IOC_RD_MAX_SPEED_HZ, &speed);
  if (ret < 0) {
    printf("Can't get max speed\n");
    return -19;
  }

  ret = ioctl(fd, SPI_IOC_WR_LSB_FIRST, &lsb_first);
  if (ret < 0) {
    printf("Can't set lsb first\n");
    return -20;
  }

  ret = ioctl(fd, SPI_IOC_RD_LSB_FIRST, &lsb_first);
  if (ret < 0) {
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

int spiClose(unsigned handle) {
  if (handle > 2) {
    printf( "Bad handle (%d)", handle);
    return -1;
  }
    
  if (SpiInfo[handle].state != SPI_OPENED) {
    printf( "Spi bus is already closed (%d)", handle);
    return -2;	
  }
     
  if (SpiInfo[handle].fd >= 0) {
    close(SpiInfo[handle].fd);
  }
    
  SpiInfo[handle].fd = -1;
  SpiInfo[handle].state = SPI_CLOSED;

  return 0;
}

int spiXfer(unsigned handle, char *txBuf, char *rxBuf, unsigned len) {
  int ret = 0;
  struct spi_ioc_transfer tr;
    
  if (handle > 2) {
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
  if (ret < 1) {
    printf("Can't send spi message\n");
    return -2;
  }
  return ret;
}
