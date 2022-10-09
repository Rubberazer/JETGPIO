# JETGPIO library

C library to manage the JETSON NANO GPIO version 0.5

<h2 align="left">FUNCTIONALITY:</h2>

- JETSON NANO (TX1) family only, so far it does not support other models such as Xavier or Orin

- GPIO control of all the header pinout as input or output. Low latency is expected (it does not go through the kernel) writing directly to the CPU registers

- PWM (hardware) control on header pins 32 & 33 

- I2C serial communication over pins: 27 (GEN1_I2C_SDA) & 28 (GEN1_I2C_SCL)  and 3 (GEN2_I2C_SDA) & 5 (GEN2_I2C_SCL)

- SPI serial communication over pins: 19 (SPI1_MOSI), 21 (SPI1_MISO), 23 (SPI1_SCK) & 24 (SPI_CS0) and 37 (SPI2_MOSI), 22 (SPI2_MISO), 13 (SPI1_SCK) & 18 (SPI_CS0)
  
- No need of previous kernel configuration to use the pins as inputs or outputs, the device tree needs to be 
  configured properly to be able to use the I2C & SPI ports though. It is recommended to use the jetson-io.py tool like so:

      sudo /opt/nvidia/jetson-io/jetson-io.py
  
  
<h2 align="left">INSTALLATION:</h2>
 
Clone/download the content into any folder in your JETSON, cd to that folder and type:
  
    make   
    sudo make install                                             
  
That's it, the library should be installed and ready to be used. 

To uninstall the library:

    sudo make uninstall
  
<h2 align="left">HOW TO:</h2> 
 
You will find code examples to learn how to use the library in both: EXAMPLES_C & EXAMPLES_C++ folders, the first one contains all the C examples, the second one contains the same examples ready to compile in C++. To compile the examples you will find instructions in the comment section at the top of each of the example files

- 'jetgpio_example.c' & 'jetgpio_example.cpp' show how to setup and use any of the pins as Inputs/Outputs, this will allow you to toggle pins from 0V (logic 0) to 3.3V (logic 1) or read the pin as an input where 3.3V at the pin is a logic 1. Note that when reading inputs, floating pins will throw unreliable results (as there is no actual input)

- 'jetgpio_PWM_example.c' & 'jetgpio_PWM_example.cpp' show how to use the PWM funcionality at pin 32

- 'test_PWM.c' & 'test_PWM.cpp' show again how to use the PWM funcionality with some LEDs connected and also capturing interruptions

- 'jetgpio_i2c_example.c' & 'jetgpio_i2c_example.cpp' show how to use the i2c comms to talk to a MPU6050 gyroscope connected to i2c0 (pins 27 & 28)

- 'spi_loop.c' & 'spi_loop.cpp' show how to run a simple loop test on the SPI port(s) by conencting together pins 19 & 21 for port SPI1 and pins 22 7 37 
  for port SPI2

The library functions names and usage mimic the ones of the pigpio library (which I recommend if you work with Raspberry Pis). Learnt a lot from that one
    
<h2 align="left">JETSON NANO PINOUT:</h2>

The library uses the typical 40 pin header numbering, taking the dev kit as reference so for instance pin 3 is I2C_2_SDA, pin 1 is 3.3 VDC power and so on. You can check the official NVIDIA pinmux configuration for reference or if not available you can use this one below:

https://jetsonhacks.com/nvidia-jetson-nano-j41-header-pinout/

The library has been tested on a Jetson nano SCO family: tegra210 (TX1), Board: P3449-0000


![image](https://user-images.githubusercontent.com/47650457/164944765-998ca31c-d72c-4d2b-8cbc-7bea594ce8d5.png)








