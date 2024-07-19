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
/** @file jetgpio.h */

#ifndef jetgpio_h__
#define jetgpio_h__

#include <stddef.h>
#include <stdint.h>

/* Definitions */

/* Chip id for Nano Classic & Orin */

#define APB_MISC_BASE 0x70000000  	//Base Nano already defined as baseCFG
#define MISC 0x00100000			//Base Orin
#define APB_MISC_GP_HIDREV_0 0x804 	//Offset Nano
#define MISCREG_HIDREV_0 0x4		//Offset Orin

/* GPIO base address Nano Classic */

#define base_CNF 0x6000d000
#define base_PINMUX 0x70003000
#define base_CFG 0x70000000

/* PWM Control Nano Classic */

#define base_PWM 0x7000a000             // PWM Controller base address
#define PM3_PWM0 0x00                   // PWM0 pin 32 LCD_BL_PWM 
#define PM3_PWM2 0x20                   // PWM2 pin 33 GPIO_PE6

/* Clock and Reset Controller Nano Classic */

#define CAR 0x60006000                  		// Clock and Reset Controller (CAR) base address
#define CLK_RST_CONTROLLER_CLK_OUT_ENB_L_0 0x10 	// CLK_RST_CONTROLLER_CLK_OUT_ENB_L_0 offset
#define CLK_RST_CONTROLLER_CLK_OUT_ENB_H_0 0x14 	// CLK_RST_CONTROLLER_CLK_OUT_ENB offset
#define CLK_RST_CONTROLLER_RST_DEVICES_H_0 0x8	   	// Reset the spi controllers
#define CLK_RST_CONTROLLER_CLK_SOURCE_SPI2_0 0x118 	// CLK_RST_CONTROLLER_CLK_SOURCE_SPI2_0 source clock and divider spi2
#define CLK_RST_CONTROLLER_CLK_SOURCE_SPI1_0 0x134 	// CLK_RST_CONTROLLER_CLK_SOURCE_SPI1_0 source clokc and divider spi1
#define CLK_RST_CONTROLLER_CLK_ENB_L_SET_0 0x320 	// CLK_RST_CONTROLLER_CLK_ENB_L_SET_0 offset

/* Power Management Controller Nano Classic */

#define base_PMC 0x7000e000                  	    // Power Management Controller (PMC) base address
#define APBDEV_PMC_PWR_DET_VAL_0 0xe4               // APBDEV_PMC_PWR_DET_VAL_0
#define APBDEV_PMC_PWR_DET_0 0x48                   // APBDEV_PMC_PWR_DET_0
#define APBDEV_PMC_PWR_DET_LATCH_0 0x4c             // APBDEV_PMC_PWR_DET_LATCH_0

/* GPIO CNF registers Nano Classic */

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
#define CNF_22 0x004                    // Pin 22 SPI2_MISO
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

/* GPIO Pinmux registers Nano Classic */

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
#define PINMUX_21 0x054                 // Pinmux 21 PINMUX_AUX_SPI1_MISO_0
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

/* GPIO Cfg registers Nano Classic */

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

/* Typical values Pinmux & Cfg registers Nano Classic */

#define PINMUX_IN 0x00000040            // Typical for pinmux register as input
#define PINMUX_OUT 0x00000440           // Typical for pinmux register as output
#define PINMUX_OUT1 0x0000e240          // Typical for pinmux spi pins register as output 
#define CFG_IN 0x00000000               // Typical for config register as input
#define CFG_OUT 0x01F1F000              // Typical for config register as output
#define CFG_OUT1 0xF0000000             // Typical for config spi pins register as output

/* GPIO base address Orin */

#define base_CNF_AON 0x0c2f1000         // Base address Nano AON: 3,5,27,28 AGX: 3,5,16,27,28,29,31,32,33,37
#define base_CNF_NAON 0x02210000        // Base address Nano Non AON: 7,8,10,11,29,31,36,15,12,32,33,35,38,40,13,16,18,19,21,22,23,24,26,37 AGX: 7,8,10,11,36,15,12,35,38,40,13,18,19,21,22,23,24,26
#define Pinmux_AON 0x0c302000           // Pinmux + config GPIO PADCTL_A14 pad Nano & AGX: 3,5,27,28
#define Pinmux_AONHV 0x0c303000         // Pinmux + config GPIO PADCTL_A15 pad AGX: 29,31,33,37 
#define Pinmux_G7 0x02448000            // Pinmux + config GPIO PADCTL_A24 pad Nano: 7
#define Pinmux_G3 0x02430000            // Pinmux + config GPIO PADCTL_A0 pad Nano: 8,10,11,29,31,36 AGX: 8,10,11,13,22,26
#define Pinmux_EDP 0x02440000           // Pinmux + config GPIO PADCTL_A16 pad Nano & AGX: 15
#define Pinmux_G4 0x02434000            // Pinmux + config GPIO PADCTL_A4 pad Nano: 12,32,33,35,38,40 AGX: 18
#define Pinmux_G2 0x0243d000            // Pinmux + config GPIO PADCTL_A13 pad Nano: 13,16,18,19,21,22,23,24,26,37 AGX: 19,21,23,24,26

/* PWM Control Orin */

#define base_PWM1 0x03280000            // PWM1 Controller base address Nano & AGX: pin 15
#define base_PWM5 0x032c0000            // PWM5 Controller base address Nano: pin 33, AGX: pin 18 
#define base_PWM7 0x032e0000            // PWM7 Controller base address Nano: pin 32
#define base_PWM8 0x032f0000            // PWM8 Controller base address AGX: pin 13 

/* GPIO CNF registers Orin */

#define CNFO_3 0x0640     		// Pin 3  AO_GEN8_I2C_SDA_0
#define CNFO_5 0x0620     		// Pin 5  AO_GEN8_I2C_SCL_0
#define CNFO_7 0x002c0    		// Pin 7  G7_SOC_GPIO59_0
#define CNFO_8 0x02840    		// Pin 8  G3_UART1_TX_0
#define CNFO_10 0x02860   		// Pin 10 G3_UART1_RX_0
#define CNFO_11 0x02880   		// Pin 11 G3_UART1_RTS_0
#define CNFO_12 0x042e0   		// Pin 12 G4_SOC_GPIO41_0
#define CNFO_13 0x01200   		// Pin 13 G2_SPI3_SCK_0
#define CNFO_15 0x02220   		// Pin 15 EDP_SOC_GPIO39_0
#define CNFO_16 0x01280   		// Pin 16 G2_SPI3_CS1_0
#define CNFO_18 0x01260   		// Pin 18 G2_SPI3_CS0_0
#define CNFO_19 0x014a0   		// Pin 19 G2_SPI1_MOSI_0
#define CNFO_21 0x01480   		// Pin 21 G2_SPI1_MISO_0
#define CNFO_22 0x01220   		// Pin 22 G2_SPI3_MISO_0
#define CNFO_23 0x01460   		// Pin 23 G2_SPI1_SCK_0
#define CNFO_24 0x014c0   		// Pin 24 G2_SPI1_CS0_0
#define CNFO_26 0x014e0   		// Pin 26 G2_SPI1_CS1_0
#define CNFO_27 0x0600    		// Pin 27 AO_GEN2_I2C_SDA_0
#define CNFO_28 0x04e0    		// Pin 28 AO_GEN2_I2C_SCL_0
#define CNFO_29 0x026a0   		// Pin 29 G3_SOC_GPIO32_0
#define CNFO_31 0x026c0   		// Pin 31 G3_SOC_GPIO33_0
#define CNFO_32 0x040c0   		// Pin 32 G4_SOC_GPIO19_0
#define CNFO_33 0x04200   		// Pin 33 G4_SOC_GPIO21_0
#define CNFO_35 0x04440   		// Pin 35 G4_SOC_GPIO44_0
#define CNFO_36 0x028a0   		// Pin 36 G3_UART1_CTS_0
#define CNFO_37 0x01240   		// Pin 37 G2_SPI3_MOSI_0
#define CNFO_38 0x04420   		// Pin 38 G4_SOC_GPIO43_0
#define CNFO_40 0x04400   		// Pin 40 G4_SOC_GPIO42_0

/* GPIO Pinmux registers Orin */

#define PINMUXO_3 0x18    		// Pinmux 3  AO_GEN8_I2C_SDA_0 
#define PINMUXO_5 0x20    		// Pinmux 5  AO_GEN8_I2C_SCL_0 
#define PINMUXO_7 0x30    		// Pinmux 7  G7_SOC_GPIO59_0 
#define PINMUXO_8 0xa8    		// Pinmux 8  G3_UART1_TX_0 
#define PINMUXO_10 0xa0   		// Pinmux 10 G3_UART1_RX_0
#define PINMUXO_11 0x98   		// Pinmux 11 G3_UART1_RTS_0
#define PINMUXO_12 0x88   		// Pinmux 12 G4_SOC_GPIO41_0
#define PINMUXO_13 0x30   		// Pinmux 13 G2_SPI3_SCK_0
#define PINMUXO_15 0x20   		// Pinmux 15 EDP_SOC_GPIO39_0
#define PINMUXO_16 0x20   		// Pinmux 16 G2_SPI3_CS1_0
#define PINMUXO_18 0x10   		// Pinmux 18 G2_SPI3_CS0_0
#define PINMUXO_19 0x40   		// Pinmux 19 G2_SPI1_MOSI_0
#define PINMUXO_21 0x18   		// Pinmux 21 G2_SPI1_MISO_0
#define PINMUXO_22 0x0    		// Pinmux 22 G2_SPI3_MISO_0
#define PINMUXO_23 0x28   		// Pinmux 23 G2_SPI1_SCK_0
#define PINMUXO_24 0x8    		// Pinmux 24 G2_SPI1_CS0_0
#define PINMUXO_26 0x38   		// Pinmux 26 G2_SPI1_CS1_0
#define PINMUXO_27 0x40   		// Pinmux 27 AO_GEN2_I2C_SDA_0
#define PINMUXO_28 0x30   		// Pinmux 28 AO_GEN2_I2C_SCL_0
#define PINMUXO_29 0x68   		// Pinmux 29 G3_SOC_GPIO32_0
#define PINMUXO_31 0x70   		// Pinmux 31 G3_SOC_GPIO33_0
#define PINMUXO_32 0x80   		// Pinmux 32 G4_SOC_GPIO19_0
#define PINMUXO_33 0X40   		// Pinmux 33 G4_SOC_GPIO21_0
#define PINMUXO_35 0xa0   		// Pinmux 35 G4_SOC_GPIO44_0
#define PINMUXO_36 0x90   		// Pinmux 36 G3_UART1_CTS_0
#define PINMUXO_37 0x48   		// Pinmux 37 G2_SPI3_MOSI_0
#define PINMUXO_38 0x98   		// Pinmux 38 G4_SOC_GPIO43_0
#define PINMUXO_40 0x90   		// Pinmux 40 G4_SOC_GPIO42_0

/* GPIO Cfg registers Orin */

#define CFGO_3 0x1c   			// Config 3  AO_CFG2TMC_GEN8_I2C_SDA_0 
#define CFGO_5 0X24   			// Config 5  AO_CFG2TMC_GEN8_I2C_SCL_0 
#define CFGO_7 0x34   			// Config 7  G7_CFG2TMC_SOC_GPIO59_0 
#define CFGO_8 0xac   			// Config 8  G3_CFG2TMC_UART1_TX_0
#define CFGO_10 0xa4  			// Config 10 G3_CFG2TMC_UART1_RX_0
#define CFGO_11 0x9c  			// Config 11 G3_CFG2TMC_UART1_RTS_0
#define CFGO_12 0x8c  			// Config 12 G4_CFG2TMC_SOC_GPIO41_0
#define CFGO_13 0x34  			// Config 13 G2_CFG2TMC_SPI3_SCK_0
#define CFGO_15 0x24  			// Config 15 EDP_CFG2TMC_SOC_GPIO39_0
#define CFGO_16 0x24  			// Config 16 G2_CFG2TMC_SPI3_CS1_0
#define CFGO_18 0x14  			// Config 18 G2_CFG2TMC_SPI3_CS0_0
#define CFGO_19 0x44  			// Config 19 G2_CFG2TMC_SPI1_MOSI_0
#define CFGO_21 0x1c  			// Config 21 G2_CFG2TMC_SPI1_MISO_0
#define CFGO_22 0x4   			// Config 22 G2_CFG2TMC_SPI3_MISO_0
#define CFGO_23 0x2c  			// Config 23 G2_CFG2TMC_SPI1_SCK_0
#define CFGO_24 0xc   			// Config 24 G2_CFG2TMC_SPI1_CS0_0
#define CFGO_26 0x3c  			// Config 26 G2_CFG2TMC_SPI1_CS1_0
#define CFGO_27 0x44  			// Config 27 AO_CFG2TMC_GEN2_I2C_SDA_0
#define CFGO_28 0x34  			// Config 28 AO_CFG2TMC_GEN2_I2C_SCL_0
#define CFGO_29 0x6c  			// Config 29 G3_CFG2TMC_SOC_GPIO32_0
#define CFGO_31 0x74  			// Config 31 G3_CFG2TMC_SOC_GPIO33_0
#define CFGO_32 0x84  			// Config 32 G4_CFG2TMC_SOC_GPIO19_0
#define CFGO_33 0x44  			// Config 33 G4_CFG2TMC_SOC_GPIO21_0
#define CFGO_35 0xa4  			// Config 35 G4_CFG2TMC_SOC_GPIO44_0
#define CFGO_36 0x94  			// Config 36 G3_CFG2TMC_UART1_CTS_0
#define CFGO_37 0x4c  			// Config 37 G2_CFG2TMC_SPI3_MOSI_0
#define CFGO_38 0x9c  			// Config 38 G4_CFG2TMC_SOC_GPIO43_0
#define CFGO_40 0x94  			// Config 40 G4_CFG2TMC_SOC_GPIO42_0

/* GPIO CNF registers Orin AGX */

#define CNFOX_7 0x026c0    		// Pin 7  G3_SOC_GPIO33_0
#define CNFOX_13 0x02800   		// Pin 13 G3_SOC_GPIO37_0
#define CNFOX_16 0x0a20   		// Pin 16 AO_HV_CAN1_EN_0
#define CNFOX_18 0x04200   		// Pin 18 G4_SOC_GPIO21_0
#define CNFOX_22 0x02480   		// Pin 22 G3_SOC_GPIO23_0
#define CNFOX_29 0x0820   		// Pin 29 AO_HV_CAN0_DIN_0
#define CNFOX_31 0x0800   		// Pin 31 AO_HV_CAN0_DOUT_0
#define CNFOX_32 0x0a00   		// Pin 32 AO_HV_CAN1_STB_0 
#define CNFOX_33 0x0840   		// Pin 33 AO_HV_CAN1_DOUT_0
#define CNFOX_37 0x0860   		// Pin 37 AO_HV_CAN1_DIN_0

/* GPIO Pinmux registers Orin AGX */

#define PINMUXOX_7 0x70    		// Pinmux 7  G3_SOC_GPIO33_0 
#define PINMUXOX_13 0x80   		// Pinmux 13 G3_SOC_GPIO37_0
#define PINMUXOX_16 0x48   		// Pinmux 16 AO_HV_CAN1_EN_0
#define PINMUXOX_18 0x40   		// Pinmux 18 G4_SOC_GPIO21_0
#define PINMUXOX_22 0x20    		// Pinmux 22 G3_SOC_GPIO23_0
#define PINMUXOX_29 0x18   		// Pinmux 29 AO_HV_CAN0_DIN_0
#define PINMUXOX_31 0x10   		// Pinmux 31 AO_HV_CAN0_DOUT_0
#define PINMUXOX_32 0x40   		// Pinmux 32 AO_HV_CAN1_STB_0
#define PINMUXOX_33 0X0   		// Pinmux 33 AO_HV_CAN1_DOUT_0
#define PINMUXOX_37 0x8   		// Pinmux 37 AO_HV_CAN1_DIN_0

/* GPIO Cfg registers Orin AGX */

#define CFGOX_7 0x74   			// Config 7  G3_CFG2TMC_SOC_GPIO33_0 
#define CFGOX_13 0x84  			// Config 13 G3_CFG2TMC_SOC_GPIO37_0
#define CFGOX_16 0x4c  			// Config 16 AO_HV_CFG2TMC_CAN1_EN_0
#define CFGOX_18 0x44  			// Config 18 G4_CFG2TMC_SOC_GPIO21_0
#define CFGOX_22 0x24   		// Config 22 G3_CFG2TMC_SOC_GPIO23_0
#define CFGOX_29 0x1c  			// Config 29 AO_HV_CFG2TMC_CAN0_DIN_0
#define CFGOX_31 0x14  			// Config 31 AO_HV_CFG2TMC_CAN0_DOUT_0
#define CFGOX_32 0x44  			// Config 32 AO_HV_CFG2TMC_CAN1_STB_0
#define CFGOX_33 0x4  			// Config 33 AO_HV_CFG2TMC_CAN1_DOUT_0
#define CFGOX_37 0xc  			// Config 37 AO_HV_CFG2TMC_CAN1_DIN_0

/* Typical values Pinmux & Cfg registers Orin */

#define CNFO_IN 0x00000001               // Typical for CNF register as input
#define CNFO_OUT 0x00000003              // Typical for CNF register as output
#define PINMUXO_IN 0x00000040            // Typical for pinmux register as input
#define PINMUXO_IN1 0x00000041
#define PINMUXO_OUT 0x00000040           // Typical for pinmux register as output
#define PINMUXO_OUT1 0x00000060          // Pinmux register as output for I2C pins
#define CFGO_IN 0x00000000               // Typical for config register as input
#define CFGO_OUT 0x01f1f000              // Typical for config register as output

/* Define outputs get_chip_id */

#define NANO 210
#define ORIN 234
#define ORINAGX 235

/* Define typical input/output */

#define JET_INPUT 0
#define JET_OUTPUT 1

/* Define the typical interruption trigger */

#define RISING_EDGE 1
#define FALLING_EDGE 2
#define EITHER_EDGE 3

/* i2c definitions */

#define I2C_CLOSED   0
#define I2C_RESERVED 1
#define I2C_OPENED   2

/* SPI definitions */

#define SPI_CLOSED   0
#define SPI_RESERVED 1
#define SPI_OPENED   2

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
  uint32_t CNF[1];
  uint32_t DEB[1];
  uint32_t IN[1];
  uint32_t OUT[1];
  uint32_t OUT_VLE[1];
  uint32_t INT_CLR[1];
} GPIO_CNFO;

  
typedef struct {
  uint32_t pin3;
  uint32_t pin5;
  uint32_t pin7;
  uint32_t pin8;
  uint32_t pin10;
  uint32_t pin11;
  uint32_t pin12;
  uint32_t pin13;
  uint32_t pin15;
  uint32_t pin16;
  uint32_t pin18;
  uint32_t pin19;
  uint32_t pin21;
  uint32_t pin22;
  uint32_t pin23;
  uint32_t pin24;
  uint32_t pin26;
  uint32_t pin27;
  uint32_t pin28;
  uint32_t pin29;
  uint32_t pin31;
  uint32_t pin32;
  uint32_t pin33;
  uint32_t pin35;
  uint32_t pin36;
  uint32_t pin37;
  uint32_t pin38;
  uint32_t pin40;
} GPIO_CNF_Init;

typedef struct {
  uint32_t stat_reg;
  uint32_t gpio;
  uint32_t edge;
  uint32_t gpio_offset;
  uint64_t *timestamp;
  void (*f)();
  uint32_t debounce;
} ISRFunc;

typedef ISRFunc *PISRFunc;

typedef struct {
  uint32_t PWM_0[4];
  uint32_t PWM_1[4];
  uint32_t PWM_2[4];
  uint32_t PWM_3[4];
} GPIO_PWM;

typedef struct {
  uint32_t state;
  int32_t fd;
  uint32_t addr;
  uint32_t flags;
  uint32_t funcs;
} i2cInfo_t;

typedef struct {
  uint32_t state;
  int32_t fd;
  uint32_t mode;
  uint32_t speed;
  uint32_t cs_delay;
  uint32_t cs_change;
  uint32_t bits_word;
} SPIInfo_t;

/* Functions */

int gpioInitialise(void);
/**<
 * @brief Initialises the library.
 * gpioInitialise must be called before using the other library functions, it stores the status of all the relevant registers before using/modifying them.
 * 
 * @return Returns 0 if OK, otherwise a negative number
 *
 * @code if (gpioInitialise() < 0)
 * {
 *   // jetgpio initialisation failed.
 * }
 * else
 * {
 *   // jetgpio initialised okay.
 * } @endcode
 */

void gpioTerminate(void);
/**<
 * @brief Terminates the library.
 * This function restores the used registers to their previous state and releases memory.
 *
 * @return Returns nothing
 *
 * @code gpioTerminate(); @endcode
*/

int gpioSetMode(unsigned gpio, unsigned mode);
/**<
 * @brief Sets the GPIO mode, typically input or output.
 *
 * @param gpio 3-40
 * @param mode JET_INPUT, JET_OUTPUT
 * @return Returns 0 if OK, -1 otherwise
 *
 * @code gpioSetMode(17, JET_INPUT);  // Sets pin 17 as input. @endcode
 * @code gpioSetMode(18, JET_OUTPUT); // Sets pin 18 as output. @endcode
*/

int gpioRead(unsigned gpio);
/**<
 * @brief Reads the GPIO level, on or off, 0 or 1.
 * Arduino style: digitalRead. Wheter a pin has been set as input or output it can be read with this function.
 * @param gpio 3-40
 * @return Returns the GPIO level if OK, otherwise a negative number
 *
 * @code printf("pin 24 is level %d", gpioRead(24)); @endcode
*/

int gpioWrite(unsigned gpio, unsigned level);
/**<
 * @brief Sets the GPIO level, on or off.
 * Arduino style: digitalWrite
 * @param gpio 3-40
 * @param level 0-1
 * @return Returns 0 if OK, otherwise a negative number
 *
 * @code gpioWrite(24, 1); // Sets pin 24 high. @endcode
*/

int gpioSetISRFunc(unsigned gpio, unsigned edge, unsigned debounce, unsigned long *timestamp, void (*f)());
/**<
 * @brief Registers a function to be called (a callback) whenever the specified.
 * @brief GPIO interrupt occurs.
 * This function will start a thread that will monitor the status of the interrupt.One function may be registered per GPIO.
 * @param gpio 3-40
 * @param edge RISING_EDGE, FALLING_EDGE, or EITHER_EDGE
 * @param debounce 0-1000 useconds, to avoid bouncing specially on mechanical inputs
 * @param timestamp: timestamp of the detected edge in nanoseconds in EPOCH format
 * @param f the callback function, this will be execute if an edge is detected in the selected pin
 * @return Returns 0 if OK, otherwise a negative number
 *
 * @code gpioSetISRFunc(3, RISING_EDGE, 1000, &timestamp, &callback); // Calls the 'callback' function when a rising edge is detected on pin 3, with a debouncing time of 1000 useconds
 *  it also delivers the event timestamp on the "timestamp" variable in EPOCH format (nanoseconds) @endcode
*/

int gpioSetPWMfrequency(unsigned gpio, unsigned frequency);
/**<
 * @brief Sets the frequency in hertz to be used for the GPIO.
 * @param gpio Nano: 32, 33 Orin Nano,NX: 15, 32, 33 Orin AGX: 13, 15, 18
 * @param frequency 25Hz to 187 kHz on Nano and 50 Hz to 1593 kHz on Orin
 * @return Returns 1 if OK, a negative number otherwise
 *
 * @code gpioSetPWMfrequency(32, 10000); // Setting up PWM frequency=10kHz @ pin 32 @endcode
*/

int gpioPWM(unsigned gpio, unsigned dutycycle);
/**<
 * @brief Starts PWM on the GPIO, dutycycle between 0 (off) and range (fully on).
 * Arduino style: analogWrite
 * @param gpio Nano: 32, 33 Orin Nano,NX: 15, 32, 33 Orin AGX: 13, 15, 18
 * @param dutycycle: 0-256 (0 to 100%)
 * @return Returns 0 if OK, a negative number otherwise
 *
 * @code gpioPWM(32, 256); // Sets pin 32 full on.
 *  gpioPWM(33, 128); // Sets pin 33 half on. @endcode
*/

int i2cOpen(unsigned i2cBus, unsigned i2cFlags);
/**<
 * @brief This returns a handle for the device at the address on the I2C bus.
 * @param i2cBus 0 or 1, 0 are pins 27 (SDA) & 28 (SCL), 1 are pins 3(SDA) & 5(SCL)
 * In Orin the pins are the same but the i2c devices are: 0->i2c-1 & 1->i2c-7
 * Flags allow you to change the bus speed. NOTE - FLAGS WILL NOT CAUSE ANY EFFECT FOR MACHINES WITH JETPACK >= 6:
 * @param i2cFlags 0 -> 100 kHz
 * @param i2cFlags 1 -> 400 kHz
 * @param i2cFlags 2 -> 1 MHz
 * @return Returns a handle with the I2C bus number being opened (>=0) if OK, otherwise a negative number
 *
 * @code int MPU6050 = i2cOpen(0,0); //Opening a connection on pins 27/28 at speed 100kHz @endcode
*/

int i2cClose(unsigned handle);
/**<
 * @brief This closes the I2C device associated with the handle.
 * @param handle >=0, as returned by a call to [*i2cOpen*]
 * @return Returns 0 if OK, otherwise a negative number
 *
 * @code i2cClose(MPU6050); //Closing previously opened connection @endcode
*/

int i2cWriteByteData(unsigned handle, unsigned i2cAddr, unsigned i2cReg, unsigned bVal);
/**<
 * @brief This writes a single byte to the specified register of the device associated with handle.
 *
 * @param handle >=0, as returned by a call to [*i2cOpen*]
 * @param i2cAddr 0-0x7F, the I2C slave address
 * @param i2cReg 0-255, the register to write
 * @param bVal 0-0xFF, the value to write
 * @return Returns 0 if OK, negative number otherwise
 *
 * @code writestat = i2cWriteByteData(MPU6050, 0x68, 0x1B, 0x00); // writing 0x00 to register address 0x1B on opened chanel MPU6050 with i2C address 0x68 @endcode
*/

int i2cReadByteData(unsigned handle, unsigned i2cAddr, unsigned i2cReg);
/**<
 * @brief This reads a single byte from the specified register of the device associated with handle.
 * @param handle >=0, as returned by a call to [*i2cOpen*]
 * @param i2cAddr 0-0x7F, the I2C slave address
 * @param i2cReg 0-255, the register to read
 * @return Returns the byte read (>=0) if OK, otherwise a negative number
 *
 * @code gyro_x_H = i2cReadByteData(MPU6050, 0x68, 0x43); // getting register 0x43 out of opened connection MPU6050 with i2C address 0x68 @endcode
*/

int i2cWriteWordData(unsigned handle, unsigned i2cAddr, unsigned i2cReg, unsigned wVal);
/**<
 * @brief This writes two bytes to the specified consecutive register(s) of the device associated with handle.
 *
 * @param handle >=0, as returned by a call to [*i2cOpen*]
 * @param i2cAddr 0-0x7F, the I2C slave address
 * @param i2cReg 0-255, the register to write
 * @param wVal 0-0xFFFF, the value to write
 * @return Returns 0 if OK, negative number otherwise
 *
 * @code writestat = i2cWriteWordData(MPU6050, 0x68, 0x1B, 0x0000); // writing 0x0000 to register address 0x1B & 0x1C on opened chanel MPU6050 with i2C address 0x68 @endcode
*/

int i2cReadWordData(unsigned handle, unsigned i2cAddr, unsigned i2cReg);
/**<
 * @brief This reads two bytes from the specified consecutive register(s) of the device associated with handle.
 * @param handle >=0, as returned by a call to [*i2cOpen*]
 * @param i2cAddr 0-0x7F, the I2C slave address
 * @param i2cReg 0-255, the register to read
 * @return Returns the word read (>=0) if OK, otherwise a negative number
 *
 * @code gyro_x_H = i2cReadWordData(MPU6050, 0x68, 0x43); // getting register 0x43 and 0x44 out of opened connection MPU6050 with i2C address 0x68 @endcode
*/

int spiOpen(unsigned spiChan, unsigned speed, unsigned mode, unsigned cs_delay, unsigned bits_word, unsigned lsb_first, unsigned cs_change);
/**<
 * @brief This function returns a handle for the SPI device on the channel.
 * Data will be transferred at baud bits per second.  The flags may be used to modify the default behaviour of a 4-wire operation, mode 0, active low chip select.
 * There are 2 SPI channels called SPI1 & SPI2 on Nano and SPI0 & SPI1 on Orin Nano & NX. For Orin AGX there is only 1 SPI channel: SPI1 (pins 21, 19...so forth).
 * The pins used are given in the following table.
 * 
 * |Port      |MISO |MOSI |SCLK |CS0 |CS1 |
 * |----------|-----|-----|-----|----|----|
 * |SPI1/SPI0 |21   |19   |23   |24  |26  |
 * |SPI2/SPI1 |22   |37   |13   |18  |16  |
 * 
 * @param spiChan 0-1 (0 stands for SPI1 on Nano, SPI0 on Orin Nano and SPI1 on Orin AGX, 1 stands for SPI2 on Nano and SPI1 on Orin Nano)
 * @param speed up to 50M (beyond that expect problems, actually we are not going beyond that)
 * @param mode defines the SPI mode
 * 
 * |Mode |POL |PHA |
 * |-----|----|----|
 * |0    |0   |0   |
 * |1    |0   |1   |
 * |2    |1   |0   |
 * |3    |1   |1   |
 *
 * @param cs_delay delay if nonzero, how long to delay after the last bit transfer in us before optionally deselecting the device before the next transfer
 * @param bits_word bits per word from 1 to 32 bits
 * @param lsb_first is 1 if least significant bit first 0 otherwise
 * @param cs_change is 1 to deselect device before starting the next transfer otherwise 0
 * @return Returns a handle (>=0) if OK, otherwise a negative number
 *
 * @code SPI_init = spiOpen(1, 5000000, 0, 0, 8, 1, 1); // SPI2 (pins 22/37, 500 kHz, mode 0, cs delay 0, 8 bits/word,lsb first 1, cs change 1 @endcode
*/

int spiClose(unsigned handle);
/**<
 * @brief This functions closes the SPI device identified by the handle.
 * @param handle >=0, as returned by a call to [*spiOpen*]
 * @return Returns 0 if OK, otherwise a negative number
 *
 * @code spiClose(SPI_init); //closing previously opened spi port with handle SPI_init @endcode
*/

int spiXfer(unsigned handle, char *txBuf, char *rxBuf, unsigned len);
/**<
 * @brief This function transfers len bytes of data from txBuf to the SPI device associated with the handle. Simultaneously len bytes of data are read from the device and placed in rxBuf.
 * @param handle >=0, as returned by a call to [*spiOpen*]
 * @param txBuf the data bytes to write
 * @param rxBuf the received data bytes
 * @param len the number of bytes to transfer
 * @return Returns the number of bytes transferred if OK, otherwise a negative number.
 *
 * @code spiXfer(SPI_init, tx, rx, 7); //transfers tx data with a data lenght of 7 words and  receiving rx data from prevously opened connection with handle SPI_init @endcode
*/

#ifdef __cplusplus
}
#endif
 
#endif  // jetgpio_h__
