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

#ifndef jetgpio_h__
#define jetgpio_h__

#include <stddef.h>
#include <stdint.h>

/* Definitions */

/* GPIO base address */

#define base_CNF 0x6000d000
#define base_PINMUX 0x70003000
#define base_CFG 0x70000000

/* PWM Control */

#define base_PWM 0x7000a000             // PWM Controller base address
#define PM3_PWM0 0x00                   // PWM0 pin 32 LCD_BL_PWM 
#define PM3_PWM2 0x20                   // PWM2 pin 33 GPIO_PE6

/* Clock and Reset Controller */

#define CAR 0x60006000                  			// Clock and Reset Controller (CAR) base address
#define CLK_RST_CONTROLLER_CLK_OUT_ENB_H_0 0x14 	// CLK_RST_CONTROLLER_CLK_OUT_ENB offset
#define CLK_RST_CONTROLLER_CLK_SOURCE_SPI2_0 0x118 	// CLK_RST_CONTROLLER_CLK_SOURCE_SPI2_0 source clock and divider spi2
#define CLK_RST_CONTROLLER_CLK_SOURCE_SPI1_0 0x134 	// CLK_RST_CONTROLLER_CLK_SOURCE_SPI1_0 source clokc and divider spi1
#define CLK_RST_CONTROLLER_RST_DEVICES_H_0 0x8	   	// Reset the spi controllers

/* SPI control */

#define base_SPI 0x7000d400				// SPI Controller base address

/* SPI1 */
#define SPI1_COMMAND_0 0x0				// SPI1 command0 offset
#define SPI1_COMMAND2_0 0x4				// SPI1 command2 offset	
#define SPI1_TIMING_REG1_0 0x8			// SPI1 timing reg1 offset
#define SPI1_TIMING_REG2_0 0xc			// SPI1 timing reg2 offset
#define SPI1_TRANSFER_STATUS_0 0x10		// SPI1 transfer status 
#define SPI1_FIFO_STATUS_0 0x14			// SPI1 fifo status
#define SPI1_TX_DATA_0 0x18				// SPI1 tx data
#define	SPI1_RX_DATA_0 0x1c				// SPI1 rx data
#define SPI1_DMA_CTL_0 0x20				// SPI1 dma control
#define SPI1_DMA_BLK_SIZE_0 0x24		// SPI1 dma block size
#define SPI1_TX_FIFO_0 0x108			// SPI1 TX FIFO Buffer
#define SPI1_RX_FIFO_0 0x188			// SPI1 RX FIFO Buffer
#define SPI1_INTR_MASK_0 0x18c			// SPI1 Interrupt Mask Register

/* SPI2 */
#define SPI2_COMMAND_0 0x200			// SPI2 command0 offset
#define SPI2_COMMAND2_0 0x204			// SPI2 command2 offset	
#define SPI2_TIMING_REG1_0 0x208		// SPI2 timing reg1 offset
#define SPI2_TIMING_REG2_0 0x20c		// SPI2 timing reg2 offset
#define SPI2_TRANSFER_STATUS_0 0x210	// SPI2 transfer status 
#define SPI2_FIFO_STATUS_0 0x214		// SPI2 fifo status
#define SPI2_TX_DATA_0 0x218			// SPI2 tx data
#define	SPI2_RX_DATA_0 0x21c			// SPI2 rx data
#define SPI2_DMA_CTL_0 0x220			// SPI2 dma control
#define SPI2_DMA_BLK_SIZE_0 0x224		// SPI2 dma block size
#define SPI2_TX_FIFO_0 0x308			// SPI2 TX FIFO Buffer
#define SPI2_RX_FIFO_0 0x388			// SPI2 RX FIFO Buffer
#define SPI2_INTR_MASK_0 0x38c			// SPI2 Interrupt Mask Register 

/* GPIO CNF registers */

#define CNF_3 0x204                     // Pin 3 GEN2_I2C_SDA 0x6000d204
#define CNF_5 0x204                     // Pin 5 GEN2_I2C_SCL
#define CNF_7 0x60C                     // Pin 7 AUD_MCLK
#define CNF_8 0x108                     // Pin 8 UART2_TX
#define CNF_10 0x108                    // Pin 10 UART2_RX
#define CNF_11 0x108                    // Pin 11 UART2_RTS
#define CNF_12 0x204                    // Pin 12 DAP4_SCLK
#define CNF_13 0x004                    // Pin 13 SPI2_SCK
#define CNF_15 0x600                    // Pin 15 LCD_TE
#define CNF_16 0x704                    // Pin 16 SPI2_CS1
#define CNF_18 0x004                    // Pin 18 SPI2_CS0
#define CNF_19 0x008                    // Pin 19 SPI1_MOSI
#define CNF_21 0x008                    // Pin 21 SPI1_MISO
#define CNF_22 0x008                    // Pin 22 SPI2_MISO
#define CNF_23 0x008                    // Pin 23 SPI1_SCK
#define CNF_24 0x008                    // Pin 24 SPI1_CS0
#define CNF_26 0x008                    // Pin 26 SPI1_CS1
#define CNF_27 0x204                    // Pin 27 GEN1_I2C_SDA
#define CNF_28 0x204                    // Pin 28 GEN1_I2C_SCL
#define CNF_29 0x408                    // Pin 29 CAM_AF_EN
#define CNF_31 0x604                    // Pin 31 GPIO_PZ0
#define CNF_32 0x504                    // Pin 32 LCD_BL_PWM
#define CNF_33 0x100                    // Pin 33 GPIO_PE6
#define CNF_35 0x204                    // Pin 35 DAP4_FS
#define CNF_36 0x108                    // Pin 36 UART2_CTS
#define CNF_37 0x004                    // Pin 37 SPI2_MOSI
#define CNF_38 0x204                    // Pin 38 DAP4_DIN
#define CNF_40 0x204                    // Pin 40 DAP4_DOUT

/* GPIO Pinmux registers */

#define PINMUX_3 0x0c8                  // Pinmux 3 PINMUX_AUX_GEN2_I2C_SDA_0 0x700030c8
#define PINMUX_5 0x0c4                  // Pinmux 5 PINMUX_AUX_GEN2_I2C_SCL_0
#define PINMUX_7 0x180                  // Pinmux 7 PINMUX_AUX_AUD_MCLK_0
#define PINMUX_8 0x0f4                  // Pinmux 8 PINMUX_AUX_UART2_TX_0
#define PINMUX_10 0x0f8                 // Pinmux 10 PINMUX_AUX_UART2_RX_0
#define PINMUX_11 0x0fc                 // Pinmux 11 PINMUX_AUX_UART2_RTS_0
#define PINMUX_12 0x150                 // Pinmux 12 PINMUX_AUX_DAP4_SCLK_0
#define PINMUX_13 0x06c                 // Pinmux 13 PINMUX_AUX_SPI2_SCK_0
#define PINMUX_15 0x1f8                 // Pinmux 15 PINMUX_AUX_LCD_TE_0
#define PINMUX_16 0x074                 // Pinmux 16 PINMUX_AUX_SPI2_CS1_0
#define PINMUX_18 0x070                 // Pinmux 18 PINMUX_AUX_SPI2_CS0_0
#define PINMUX_19 0x050                 // Pinmux 19 PINMUX_AUX_SPI1_MOSI_0
#define PINMUX_21 0x050                 // Pinmux 21 PINMUX_AUX_SPI1_MISO_0
#define PINMUX_22 0x068                 // Pinmux 22 PINMUX_AUX_SPI2_MISO_0
#define PINMUX_23 0x058                 // Pinmux 23 PINMUX_AUX_SPI1_SCK_0
#define PINMUX_24 0x05c                 // Pinmux 24 PINMUX_AUX_SPI1_CS0_0
#define PINMUX_26 0x060                 // Pinmux 26 PINMUX_AUX_SPI1_CS1_0
#define PINMUX_27 0x0c0                 // Pinmux 27 PINMUX_AUX_GEN1_I2C_SDA_0
#define PINMUX_28 0x0bc                 // Pinmux 28 PINMUX_AUX_GEN1_I2C_SCL_0
#define PINMUX_29 0x1e4                 // Pinmux 29 PINMUX_AUX_CAM_AF_EN_0
#define PINMUX_31 0x27c                 // Pinmux 31 PINMUX_AUX_GPIO_PZ0_0
#define PINMUX_32 0x1fc                 // Pinmux 32 PINMUX_AUX_LCD_BL_PWM_0
#define PINMUX_33 0x248                 // Pinmux 33 PINMUX_AUX_GPIO_PE6_0
#define PINMUX_35 0x144                 // Pinmux 35 PINMUX_AUX_DAP4_FS_0
#define PINMUX_36 0x100                 // Pinmux 36 PINMUX_AUX_UART2_CTS_0
#define PINMUX_37 0x064                 // Pinmux 37 PINMUX_AUX_SPI2_MOSI_0
#define PINMUX_38 0x148                 // Pinmux 38 PINMUX_AUX_DAP4_DIN_0
#define PINMUX_40 0x14c                 // Pinmux 40 PINMUX_AUX_DAP4_DOUT_0

/* GPIO Cfg registers */

#define CFG_3 0x9b4                     // Config 3 GEN2_I2C_SDA_CFG 0x700009b4
#define CFG_5 0x9b0                     // Config 5 GEN2_I2C_SCL_CFG
#define CFG_7 0x8f4                     // Config 7 AUD_MCLK_CFG
#define CFG_8 0xb38                     // Config 8 UART2_TX_CFG
#define CFG_10 0xb34                    // Config 10 UART2_RX_CFG
#define CFG_11 0xb30                    // Config 11 UART2_RTS_CFG
#define CFG_12 0x980                    // Config 12 DAP4_SCLK_CFG
#define CFG_13 0xaf8                    // Config 13 SPI2_SCK_CFG
#define CFG_15 0xa44                    // Config 15 LCD_TE_CFG
#define CFG_16 0xaec                    // Config 16 SPI2_CS1_CFG
#define CFG_18 0xae8                    // Config 18 SPI2_CS0_CFG
#define CFG_19 0xae0                    // Config 19 SPI1_MOSI_CFG
#define CFG_21 0xadc                    // Config 21 SPI1_MISO_CFG
#define CFG_22 0xaf0                    // Config 22 SPI2_MISO_CFG
#define CFG_23 0xae4                    // Config 23 SPI1_SCK_CFG
#define CFG_24 0xad4                    // Config 24 SPI1_CS0_CFG
#define CFG_26 0xad8                    // Config 26 SPI1_CS1_CFG
#define CFG_27 0x9ac                    // Config 27 GEN1_I2C_SDA_CFG
#define CFG_28 0x9a8                    // Config 28 GEN1_I2C_SCL_CFG
#define CFG_29 0x92c                    // Config 29 CAM_AF_EN_CFG
#define CFG_31 0x9fc                    // Config 31 GPIO_PZ0_CFG
#define CFG_32 0xa34                    // Config 32 LCD_BL_PWM_CFG
#define CFG_33 0x9c8                    // Config 33 GPIO_PE6_CFG
#define CFG_35 0x97c                    // Config 35 DAP4_FS_CFG
#define CFG_36 0xb2c                    // Config 36 UART2_CTS_CFG
#define CFG_37 0xaf4                    // Config 37 SPI2_MOSI_CFG
#define CFG_38 0x974                    // Config 38 DAP4_DIN_CFG
#define CFG_40 0x978                    // Config 40 DAP4_DOUT_CFG

/* Typical values Pinmux & Cfg registers */

#define PINMUX_IN 0x00000040            // Typical for pinmux register as input
#define PINMUX_OUT 0x00000400           // Typical for pinmux register as output
#define CFG_IN 0x00000000               // Typical for config register as input
#define CFG_OUT 0x01F1F000              // Typical for config register as output
#define CFG_OUT1 0xF0000000             // Typical for config register as output

/* Define typical input/output */

#define JET_INPUT 0
#define JET_OUTPUT 1

/* i2c definitions */

#define I2C_CLOSED   0
#define I2C_RESERVED 1
#define I2C_OPENED   2

#ifdef __cplusplus
extern "C" {
#endif

/* Variables */

typedef struct {
    uint32_t CNF[4];
    uint32_t OE[4];
    uint32_t OUT[4];
    uint32_t IN[4];
    uint32_t INT_STA[4];
    uint32_t INT_ENB[4];
    uint32_t INT_LVL[4];
    uint32_t INT_CLR[4];
} GPIO_CNF;

typedef struct {
    uint32_t CNF3;
    uint32_t CNF5;
    uint32_t CNF7;
    uint32_t CNF8;
    uint32_t CNF10;
    uint32_t CNF11;
    uint32_t CNF12;
    uint32_t CNF13;
    uint32_t CNF15;
    uint32_t CNF16;
    uint32_t CNF18;
    uint32_t CNF19;
    uint32_t CNF21;
    uint32_t CNF22;
    uint32_t CNF23;
    uint32_t CNF24;
    uint32_t CNF26;
    uint32_t CNF27;
    uint32_t CNF28;
    uint32_t CNF29;
    uint32_t CNF31;
    uint32_t CNF32;
    uint32_t CNF33;
    uint32_t CNF35;
    uint32_t CNF36;
    uint32_t CNF37;
    uint32_t CNF38;
    uint32_t CNF40;
} GPIO_CNF_Init;

typedef struct {
    uint32_t PINMUX3;
    uint32_t PINMUX5;
    uint32_t PINMUX7;
    uint32_t PINMUX8;
    uint32_t PINMUX10;
    uint32_t PINMUX11;
    uint32_t PINMUX12;
    uint32_t PINMUX13;
    uint32_t PINMUX15;
    uint32_t PINMUX16;
    uint32_t PINMUX18;
    uint32_t PINMUX19;
    uint32_t PINMUX21;
    uint32_t PINMUX22;
    uint32_t PINMUX23;
    uint32_t PINMUX24;
    uint32_t PINMUX26;
    uint32_t PINMUX27;
    uint32_t PINMUX28;
    uint32_t PINMUX29;
    uint32_t PINMUX31;
    uint32_t PINMUX32;
    uint32_t PINMUX33;
    uint32_t PINMUX35;
    uint32_t PINMUX36;
    uint32_t PINMUX37;
    uint32_t PINMUX38;
    uint32_t PINMUX40;
} GPIO_PINMUX_Init;

typedef struct {
    uint32_t CFG3;
    uint32_t CFG5;
    uint32_t CFG7;
    uint32_t CFG8;
    uint32_t CFG10;
    uint32_t CFG11;
    uint32_t CFG12;
    uint32_t CFG13;
    uint32_t CFG15;
    uint32_t CFG16;
    uint32_t CFG18;
    uint32_t CFG19;
    uint32_t CFG21;
    uint32_t CFG22;
    uint32_t CFG23;
    uint32_t CFG24;
    uint32_t CFG26;
    uint32_t CFG27;
    uint32_t CFG28;
    uint32_t CFG29;
    uint32_t CFG31;
    uint32_t CFG32;
    uint32_t CFG33;
    uint32_t CFG35;
    uint32_t CFG36;
    uint32_t CFG37;
    uint32_t CFG38;
    uint32_t CFG40;
} GPIO_CFG_Init;

typedef struct {
    uint32_t PWM_0;
    uint32_t Padding_0;
    uint32_t Padding_1;
    uint32_t Padding_3;
    uint32_t PWM_1;
    uint32_t Padding_4;
    uint32_t Padding_5;
    uint32_t Padding_6;
    uint32_t PWM_2;
    uint32_t Padding_7;
    uint32_t Padding_8;
    uint32_t Padding_9;
    uint32_t PWM_3;
} GPIO_PWM;

typedef struct {
	int32_t state;
	int32_t	 fd;
	int32_t addr;
	int32_t flags;
	int32_t funcs;
} i2cInfo_t;

/* Functions */

int gpioInitialise(void);
/*
Initialises the library.
Returns 0 if OK, otherwise -1 or -2.
gpioInitialise must be called before using the other library functions, 
it stores the status of all the relevant registers before using/modifying them
...
if (gpioInitialise() < 0)
{
   // jetgpio initialisation failed.
}
else
{
   // jetgpio initialised okay.
}

*/

void gpioTerminate(void);
/*
Terminates the library.
Returns nothing.
Call before program exit.
This function restores the used registers and releases memory, 
...
gpioTerminate();
...
*/

int gpioSetMode(unsigned gpio, unsigned mode);
/*D
Sets the GPIO mode, typically input or output.
. .
gpio: 3-40
mode: JET_INPUT, JET_OUTPUT
. .
Returns 0 if OK, -1 otherwise.

...
gpioSetMode(17, JET_INPUT);  // Set GPIO17 as input.
gpioSetMode(18, JET_OUTPUT); // Set GPIO18 as output.
...
*/

int gpioRead(unsigned gpio);
/*
Reads the GPIO level, on or off.
. .
gpio: 3-40
. .
Returns the GPIO level if OK, otherwise -1
Arduino style: digitalRead.
...
printf("GPIO24 is level %d", gpioRead(24));
...
*/

int gpioWrite(unsigned gpio, unsigned level);
/*
Sets the GPIO level, on or off.
. .
 gpio: 3-40
level: 0-1
. .
Returns 0 if OK, otherwise -1.
Arduino style: digitalWrite
...
gpioWrite(24, 1); // Set GPIO24 high.
...
*/

int gpioSetPWMfrequency(unsigned gpio, unsigned frequency);
/*
Sets the frequency in hertz to be used for the GPIO.
. .
gpio: 32, 33
frequency: 25Hz to 200 kHz
. .
Returns 1 if OK, otherwise -1.

...
*/

int gpioPWM(unsigned gpio, unsigned dutycycle);
/*
Starts PWM on the GPIO, dutycycle between 0 (off) and range (fully on).
. .
gpio: 32, 33
dutycycle: 0-256 (0 to 100%)
. .
Returns 0 if OK, otherwise -1.
Arduino style: analogWrite
...
gpioPWM(32, 256); // Sets pin 32 full on.
gpioPWM(33, 128); // Sets pin 33 half on.
...
*/

int i2cOpen(unsigned i2cBus, unsigned i2cAddr, unsigned i2cFlags);
/*
This returns a handle for the device at the address on the I2C bus.
. .
i2cBus: 0 or 1, 0 are pins 27 (SDA) & 28 (SCL), 1 are pins 3(SDA) & 5(SCL)
i2cAddr: 0-0x7F
i2cFlags: 0
. .
Flags allow you to change the bus speed:
i2cFlags: 0 -> 100 kHz
i2cFlags: 1 -> 400 kHz
i2cFlags: 0 -> 1 MHz
. .
Returns a handle with the I2C bus number being opened (>=0) if OK, otherwise -1.
. .
*/

int i2cClose(unsigned handle);
/*
This closes the I2C device associated with the handle.
. .
handle: >=0, as returned by a call to [*i2cOpen*]
. .
Returns 0 if OK, otherwise -1.
*/

int i2cWriteByteData(unsigned handle, unsigned i2cReg, unsigned bVal);
/*
This writes a single byte to the specified register of the device
associated with handle.
. .
handle: >=0, as returned by a call to [*i2cOpen*]
i2cReg: 0-255, the register to write
  bVal: 0-0xFF, the value to write
. .
Returns 0 if OK, otherwise -1.
Write byte. SMBus 2.0 5.5.4
. .
S Addr Wr [A] i2cReg [A] bVal [A] P
. .
*/

int i2cReadByteData(unsigned handle, unsigned i2cReg);
/*
This reads a single byte from the specified register of the device
associated with handle.
. .
handle: >=0, as returned by a call to [*i2cOpen*]
i2cReg: 0-255, the register to read
. .
Returns the byte read (>=0) if OK, otherwise -1.
Read byte. SMBus 2.0 5.5.5
. .
S Addr Wr [A] i2cReg [A] S Addr Rd [A] [Data] NA P
. .
*/

int spiOpen(unsigned spiChan, unsigned baud, unsigned spiFlags);
/*
This function returns a handle for the SPI device on the channel.
Data will be transferred at baud bits per second.  The flags may
be used to modify the default behaviour of 4-wire operation, mode 0,
active low chip select.
There are 2 SPI channels SPI1 & SPI2.

The GPIO used are given in the following table.
         @ MISO @ MOSI @ SCLK @ CS0 @ CS1
SPI1 	 @   21 @   19 @   23 @  24 @  26 
SPI2 	 @   22 @   37 @   13 @  18 @  16 
. .
spiChan: 0-1 (o stand for SPI1 and 1 for SPI2)
baud: up to 50M (beyond that expect problems)
spiFlags: see below
. .
Returns a handle (>=0) if OK, otherwise a negavitve number.
spiFlags consists of the least significant 22 bits.
. .
10  9  8  7  6  5  4  3  2  1  0
 b  b  b  b  b  R  T p1 p0  m  m
. .
mm defines the SPI mode.

. .
Mode POL PHA
 0    0   0
 1    0   1
 2    1   0
 3    1   1
. .
px is 0 if CSx is active low (default) and 1 for active high.

T is 1 if the least significant bit is transmitted on MOSI first, the
default (0) shifts the most significant bit out first.  
* 
R is 1 if the least significant bit is received on MISO first, the
default (0) receives the most significant bit first.  Auxiliary SPI
only.
bbbbbb defines the word size in bits (0-32).  The default (0)
sets 8 bits per word.
The [*spiRead*], [*spiWrite*], and [*spiXfer*] functions
transfer data packed into 1, 2, or 4 bytes according to
the word size in bits.
For bits 1-8 there will be one byte per word. 
For bits 9-16 there will be two bytes per word. 
For bits 17-32 there will be four bytes per word.
Multi-byte transfers are made in least significant byte first order.
E.g. to transfer 32 11-bit words buf should contain 64 bytes
and count should be 64.
E.g. to transfer the 14 bit value 0x1ABC send the bytes 0xBC followed
by 0x1A.
The other bits in flags should be set to zero.
*/

int spiClose(unsigned handle);
/*
This functions closes the SPI device identified by the handle.
. .
handle: >=0, as returned by a call to [*spiOpen*]
. .
Returns 0 if OK, otherwise -1.
*/

int spiRead(unsigned handle, char *buf, unsigned count);
/*
This function reads count bytes of data from the SPI
device associated with the handle.
. .
handle: >=0, as returned by a call to [*spiOpen*]
   buf: an array to receive the read data bytes
 count: the number of bytes to read
. .
Returns the number of bytes transferred if OK, otherwise -1
*/

int spiWrite(unsigned handle, char *buf, unsigned count);
/*
This function writes count bytes of data from buf to the SPI
device associated with the handle.
. .
handle: >=0, as returned by a call to [*spiOpen*]
   buf: the data bytes to write
 count: the number of bytes to write
. .
Returns the number of bytes transferred if OK, otherwise -1.
*/

int spiXfer(unsigned handle, char *txBuf, char *rxBuf, unsigned count);
/*
This function transfers count bytes of data from txBuf to the SPI
device associated with the handle.  Simultaneously count bytes of
data are read from the device and placed in rxBuf.
. .
handle: >=0, as returned by a call to [*spiOpen*]
 txBuf: the data bytes to write
 rxBuf: the received data bytes
 count: the number of bytes to transfer
. .
Returns the number of bytes transferred if OK, otherwise a negative number.
*/

#ifdef __cplusplus
}
#endif
 
#endif  // jetgpio_h__
