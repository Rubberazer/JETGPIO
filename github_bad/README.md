# JETGPIO library  

C library to manage the GPIO header of the Nvidia JETSON boards

[UPDATE: ORIN AGX NOW SUPPORTED](https://github.com/Rubberazer/JETGPIO/discussions/30)


<h2 align="left">FUNCTIONALITY:</h2>

- Supported models:

  - JETSON NANO and TX1

  - JETSON ORIN NANO and ORIN NX 

  - JETSON ORIN AGX

- GPIO control of all the header pinout as input or output. Low latency, see also [The need for speed](#the-need-for-speed) below for some more information on this

- Catching rising or falling edges in any header pin working as input. Timestamp of the event in nanoseconds in epoch format is provided 

- PWM (hardware) control on header pins 32, 33 on Nano and also 15 on Orin Nano/NX, for Orin AGX the pins are: 13, 15 & 18

- I2C serial communication over pins: 27 (SDA) & 28 (SCL)  and 3 (SDA) & 5 (SCL)

- SPI serial communication over pins: 19 (MOSI), 21 (MISO), 23 (SCK) & 24 (CS0) and 37 (MOSI), 22 (MISO), 13 (SCK) & 18 (CS0)
  
- No need of previous kernel configuration, no need to change the device tree, no need to reconfigure via /opt/nvidia/jetson-io/jetson-io.py or the like  
  
<h2 align="left">INSTALLATION:</h2>
 
Clone/download the content into any folder in your JETSON, cd to that folder and type:
  
    sudo make   
    sudo make install                                             
  
That's it, the library should be installed and ready to be used. To uninstall the library:

    sudo make uninstall
  
<h2 align="left">HOW TO:</h2> 
 
You will find code examples to learn how to use the library in both: EXAMPLES_C & EXAMPLES_C++ folders, the first one contains all the C examples, the second one contains the same examples ready to compile in C++. To compile the examples you will find instructions in the comment section at the top of each of the example files. I have also added a folder with examples in the Zig programming language to test interoperability.

- [jetgpio_example.c](https://github.com/Rubberazer/JETGPIO/blob/main/EXAMPLES_C/jetgpio_example.c) & [jetgpio_example.cpp](https://github.com/Rubberazer/JETGPIO/blob/main/EXAMPLES_C++/jetgpio_example.cpp) show how to setup and use any of the pins as Inputs/Outputs, this will allow you to toggle pins from 0V (logic 0) to 3.3V (logic 1) or read the pin as an input where 3.3V at the pin is a logic 1. Note that when reading inputs, floating pins will throw unreliable results (as there is no actual input)

- [jetgpio_edge.c](https://github.com/Rubberazer/JETGPIO/blob/main/EXAMPLES_C/jetgpio_edge.c) & [jetgpio_edge.cpp](https://github.com/Rubberazer/JETGPIO/blob/main/EXAMPLES_C++/jetgpio_edge.cpp) show how to catch rising or falling edges in any pin working as Input, timestamp in epoch format in nanoseconds is provided

- [jetgpio_PWM_example.c](https://github.com/Rubberazer/JETGPIO/blob/main/EXAMPLES_C/jetgpio_PWM_example.c) & [jetgpio_PWM_example.cpp](https://github.com/Rubberazer/JETGPIO/blob/main/EXAMPLES_C++/jetgpio_PWM_example.cpp) show how to use the PWM funcionality at pin 32

- [test_PWM.c](https://github.com/Rubberazer/JETGPIO/blob/main/EXAMPLES_C/test_PWM.c) & [test_PWM.cpp](https://github.com/Rubberazer/JETGPIO/blob/main/EXAMPLES_C++/test_PWM.cpp) show again how to use the PWM funcionality with some LEDs connected and also capturing interruptions

- [jetgpio_i2c_example.c](https://github.com/Rubberazer/JETGPIO/blob/main/EXAMPLES_C/jetgpio_i2c_example.c) & [jetgpio_i2c_example.cpp](https://github.com/Rubberazer/JETGPIO/blob/main/EXAMPLES_C++/jetgpio_i2c_example.cpp) show how to use the i2c comms to talk to a MPU6050 gyroscope connected to i2c0 (pins 27 & 28)

- [lcd_i2c.c](https://github.com/Rubberazer/JETGPIO/blob/main/EXAMPLES_C/lcd_i2c.c) & [lcd_i2c.cpp](https://github.com/Rubberazer/JETGPIO/blob/main/EXAMPLES_C++/lcd_i2c.cpp) minimalistic example of how to show a message on the screen of a Freenove i2c 1602 LCD display connected to i2c1 (pins 3 & 5)

- [spi_loop.c](https://github.com/Rubberazer/JETGPIO/blob/main/EXAMPLES_C/spi_loop.c) & [spi_loop.cpp](https://github.com/Rubberazer/JETGPIO/blob/main/EXAMPLES_C++/spi_loop.cpp) show how to run a simple loop test on the SPI port(s) by connecting together pins 19 & 21 for port SPI1 and pins 22 & 37 
  for port SPI2

- [ZIG Examples](https://github.com/Rubberazer/JETGPIO/tree/main/EXAMPLES_Zig) just for the sake of it

<h2 align="left">DOCUMENTATION:</h2>

[Some doxygen documentation here](https://rubberazer.github.io/JETGPIO/html/index.html). As a rule of thumb, the library functions names and usage mimic the ones of the pigpio library (which I recommend if you work with Raspberry Pis). Learnt a lot from that one

<h2 align="left">THE NEED FOR SPEED:</h2>

I created a couple of little programs to measure reaction time e.g. how fast an output pin turns from 0 to 1 (3.3v), or how fast a change to an input pin is detected by the library, a diagram of the physical setup is shown below, basically I set up pin 38 as an output and pin 40 as an input and connect both through a resistor to observe the interaction:

![jetson_speed](https://user-images.githubusercontent.com/47650457/227725735-0edb04d1-0d8f-465f-9212-18e41e2cc364.png)

Compiling and running [jetgpio_round_trip.c](https://github.com/Rubberazer/JETGPIO/blob/main/EXAMPLES_C/jetgpio_round_trip.c) I am measuring the time from before executing the function that writes a logic 1 (3.3v) to pin 38 until the point when this is detected (by voltage level not edge interrupt) at pin 40. Here the intention is to measure the worst case scenario of a combination of 2 different actions: 

- a pin changes state from 0 to 1 (output)
- a second pin detects a change on its state from 0 to 1 (input) being this change produced by the output pin

The results that I am getting for the round trip (total time to execute both actions) by running this program are:

|              | Nano Classic   | Orin Nano     |
| :---         |     :---:      |          ---: |
| Minimum      |  1.3 us        |  3.1 us **    |
| Maximum      |  1.8 us        |  4.2 us **    |


Compiling and running [jetgpio_speed_edge.c](https://github.com/Rubberazer/JETGPIO/blob/main/EXAMPLES_C/jetgpio_speed_edge.c) I am trying to measure the time using a similar setup as described above, the difference here is that I am using the library function: gpioSetISRFunc() which basically goes through the linux gpio driver in order to catch rising and falling edges, the reason to use the linux driver for this has to do with the fact that catching interrupts from user space (this is a library after all) is basically 'problematic' for a number of reasons, in short, if driver performance and/or device tree stuff got in my way I would basically replace the current driver by my own, but that is beyond the scope of this library.


|              | Nano Classic   | Orin Nano     |
| :---         |     :---:      |          ---: |
| Minimum      |  250 us        |  200 us       |
| Maximum      |  700 us        |  1000 us      |


Note that this doesn't measure individual actions but the total time to execute both (round trip). It is clear that the timestamp produced by the linux driver is the one to blame for the slow reaction on detecting a change on the input pin, still interesting as there is no meaningful cpu waste as the hardware is producing the interrupt for us (no polling)

Compiling and running [jetgpio_output.c](https://github.com/Rubberazer/JETGPIO/blob/main/EXAMPLES_C/jetgpio_output.c) I am writing high/low to pin 38 on a continuous loop, what I am getting on the oscilloscope are the following results:

|              | Nano Classic   | Orin Nano     |
| :---         |     :---:      |          ---: |
| Average      |  0.6 us        |  2 us **      |


**Yes, the new Orin has a slower response/higher latency than the old Jetson Nano, this is due to the fact that writing to some registers is being monitored by an external CPU called BPMP (Boot and Power Management Processor). This CPU is an addition to what is called CPU Complex (the 6 Arm A78A cores that are described on the Orin Nano/NX specs) and is completely independent from the main system, running its own firmware and with an independent device tree. Some of the tasks performed by this CPU are clock and power supply management for peripherals inside the SOM e.g. PWM, GPIO... but it also plays a "firewall" role, in other words before writing to some registers in the standard CPU Complex the writing instructions have to pass through this "firewall" making the whole thing slower. This extra, out of the system security manager can be very useful on some applications but it has obviously a downside. Again sorting this out goes beyond the scope of any user space application (library) and it would imply flashing the Orin after modifying stuff, which is something along with other things like modifying the device tree (and reflashing probably) that I wanted to avoid when I created this library.



![freenove_i2c_1602_lcd](https://github.com/Rubberazer/JETGPIO/assets/47650457/321d70bb-d5f2-4577-b83c-b189f7ed72c9)


    
<h2 align="left">JETSON NANO AND ORIN FAMILY PINOUT:</h2>

The library uses the typical 40 pin header numbering, taking the dev kit as reference so for instance pin 3 is I2C_2_SDA on Nano, I2C1_SDA on Orin Nano and I2C5_DAT on Orin AGX, pin 1 is 3.3 VDC power and so on. You can check the official NVIDIA pinmux configuration for reference or if not available you can use the ones below:

https://jetsonhacks.com/nvidia-jetson-nano-j41-header-pinout/

https://jetsonhacks.com/nvidia-jetson-orin-nano-gpio-header-pinout/

https://jetsonhacks.com/nvidia-jetson-agx-orin-gpio-header-pinout/

The library has been tested on a Jetson Nano: tegra210 (TX1), on a Jetson Orin Nano and also on an Orin AGX.








