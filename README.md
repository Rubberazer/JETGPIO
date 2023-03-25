# JETGPIO library  

C library to manage the JETSON NANO GPIO 


[ANNOUNCEMENT: Looking for hardware donors to keep expanding the library](https://github.com/Rubberazer/JETGPIO/discussions/12)


<h2 align="left">FUNCTIONALITY:</h2>

- JETSON NANO (TX1) family only, so far it does not support other models such as Xavier or Orin

- GPIO control of all the header pinout as input or output. Low latency, see also [The need for speed](https://github.com/Rubberazer/JETGPIO/blob/main/README.md#the-need-for-speed) below for some more information on this

- Catching rising or falling edges in any header pin working as input. Timestamp of the event in nanoseconds in epoch format is provided 

- PWM (hardware) control on header pins 32 & 33 

- I2C serial communication over pins: 27 (GEN1_I2C_SDA) & 28 (GEN1_I2C_SCL)  and 3 (GEN2_I2C_SDA) & 5 (GEN2_I2C_SCL)

- SPI serial communication over pins: 19 (SPI1_MOSI), 21 (SPI1_MISO), 23 (SPI1_SCK) & 24 (SPI1_CS0) and 37 (SPI2_MOSI), 22 (SPI2_MISO), 13 (SPI2_SCK) & 18 (SPI2_CS0)
  
- No need of previous kernel configuration, no need to change the device tree, no need to reconfigure via /opt/nvidia/jetson-io/jetson-io.py or the like  
  
<h2 align="left">INSTALLATION:</h2>
 
Clone/download the content into any folder in your JETSON, cd to that folder and type:
  
    make   
    sudo make install                                             
  
That's it, the library should be installed and ready to be used. 

To uninstall the library:

    sudo make uninstall
  
<h2 align="left">HOW TO:</h2> 
 
You will find code examples to learn how to use the library in both: EXAMPLES_C & EXAMPLES_C++ folders, the first one contains all the C examples, the second one contains the same examples ready to compile in C++. To compile the examples you will find instructions in the comment section at the top of each of the example files

- [jetgpio_example.c](https://github.com/Rubberazer/JETGPIO/blob/main/EXAMPLES_C/jetgpio_example.c) & [jetgpio_example.cpp](https://github.com/Rubberazer/JETGPIO/blob/main/EXAMPLES_C++/jetgpio_example.cpp) show how to setup and use any of the pins as Inputs/Outputs, this will allow you to toggle pins from 0V (logic 0) to 3.3V (logic 1) or read the pin as an input where 3.3V at the pin is a logic 1. Note that when reading inputs, floating pins will throw unreliable results (as there is no actual input)

- [jetgpio_edge.c](https://github.com/Rubberazer/JETGPIO/blob/main/EXAMPLES_C/jetgpio_edge.c) & [jetgpio_edge.cpp](https://github.com/Rubberazer/JETGPIO/blob/main/EXAMPLES_C++/jetgpio_edge.cpp) show how to catch rising or falling edges in any pin working as Input, timestamp in epoch format in nanoseconds is provided

- [jetgpio_PWM_example.c](https://github.com/Rubberazer/JETGPIO/blob/main/EXAMPLES_C/jetgpio_PWM_example.c) & [jetgpio_PWM_example.cpp](https://github.com/Rubberazer/JETGPIO/blob/main/EXAMPLES_C++/jetgpio_PWM_example.cpp) show how to use the PWM funcionality at pin 32

- [test_PWM.c](https://github.com/Rubberazer/JETGPIO/blob/main/EXAMPLES_C/test_PWM.c) & [test_PWM.cpp](https://github.com/Rubberazer/JETGPIO/blob/main/EXAMPLES_C++/test_PWM.cpp) show again how to use the PWM funcionality with some LEDs connected and also capturing interruptions

- [jetgpio_i2c_example.c](https://github.com/Rubberazer/JETGPIO/blob/main/EXAMPLES_C/jetgpio_i2c_example.c) & [jetgpio_i2c_example.cpp](https://github.com/Rubberazer/JETGPIO/blob/main/EXAMPLES_C++/jetgpio_i2c_example.cpp) show how to use the i2c comms to talk to a MPU6050 gyroscope connected to i2c0 (pins 27 & 28)

- [spi_loop.c](https://github.com/Rubberazer/JETGPIO/blob/main/EXAMPLES_C/spi_loop.c) & [spi_loop.cpp](https://github.com/Rubberazer/JETGPIO/blob/main/EXAMPLES_C++/spi_loop.cpp) show how to run a simple loop test on the SPI port(s) by connecting together pins 19 & 21 for port SPI1 and pins 22 & 37 
  for port SPI2

<h2 align="left">DOCUMENTATION:</h2>

[Some doxygen documentation here](https://rubberazer.github.io/JETGPIO/html/index.html). As a rule of thumb, the library functions names and usage mimic the ones of the pigpio library (which I recommend if you work with Raspberry Pis). Learnt a lot from that one

<h2 align="left">THE NEED FOR SPEED:</h2>

I created a bunch of little programs to measure reaction time e.g. how fast an output pin turns from 0 to 1 (3.3v), or how fast a change to an input pin is detected by the library, a diagram of the physical setup is shown below, basically I set up pin 38 as an output and pin 40 as an input and connect both through a resistor to observe the interaction:

![jetson_speed](https://user-images.githubusercontent.com/47650457/227725735-0edb04d1-0d8f-465f-9212-18e41e2cc364.png)

Compiling and running [jetgpio_round_trip.c](https://github.com/Rubberazer/JETGPIO/blob/main/EXAMPLES_C/jetgpio_round_trip.c) I am measuring the time from before executing the function that writes a logic 1 (3.3v) to pin 38 until the point when this is detected (by voltage level not edge interrupt) at pin 40. Here the intention is to measure the worst case scenario of a combination of 2 different actions: 

- a pin changes state from 0 to 1 (output)
- a second pin detects a change on its state from 0 to 1 (input) being this change produced by the output pin

The average result that I am getting by running this program is an average 1500 nano seconds (1.5 us) for the round trip (total time to execute both actions) with minimum values around 1300 nano seconds (1.3 us) and maximum values around 1800 nano seconds (1.8 us). Note that this doesn't measure individual actions but the total time to execute both (round trip)

Compiling and running [jetgpio_speed_edge.c](https://github.com/Rubberazer/JETGPIO/blob/main/EXAMPLES_C/jetgpio_speed_edge.c) I am trying to measure the time using a similar setup as described above, the difference here is that I am using the library function: gpioSetISRFunc() which basically goes through the linux gpio driver in order to catch rising and falling edges, the reason to use the linux driver for this has to do with the fact that catching interrupts from user space (this is a library after all) is basically 'problematic' for a number of reasons, in short, if driver performance and/or device tree stuff got in my way I would basically replace the current driver by my own, but that is beyond the scope of my library, and yes this is intentional

The average result that I am getting by running this program is an average 500000 nano seconds (0.5 ms) for the round trip (total time to execute both actions) with minimum values around 250000 nano seconds (0.25 ms) and maximum values around 700000 nano seconds (0.7 ms). Note that this doesn't measure individual actions but the total time to execute both (round trip). It is clear that the timestamp produced by the linux driver is the one to blame for the slow reaction on detecting a change on the input pin, still interesting as there is pretty much no meaningful cpu waste as the hardware is producing the interrupt for us (no polling)
    
<h2 align="left">JETSON NANO PINOUT:</h2>

The library uses the typical 40 pin header numbering, taking the dev kit as reference so for instance pin 3 is I2C_2_SDA, pin 1 is 3.3 VDC power and so on. You can check the official NVIDIA pinmux configuration for reference or if not available you can use this one below:

https://jetsonhacks.com/nvidia-jetson-nano-j41-header-pinout/

The library has been tested on a Jetson nano SCO family: tegra210 (TX1), Board: P3449-0000


![image](https://user-images.githubusercontent.com/47650457/164944765-998ca31c-d72c-4d2b-8cbc-7bea594ce8d5.png)








