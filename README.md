# JETGPIO library

C library to manage the JETSON nano GPIO v0.1 (ALPHA)

FUNCTIONALITY:

    JETSON NANO (TX1) family only, so far it does not support the Xaviers,etc
  
    GPIO control setup of all the header pinout as input or output
  
    SFIO (special functions) such as I2C, Hardware PWM and others are not supported yet
  
    No need of previous kernel configuration, the library will take care of that on runtime
  
    Low latency is expected (it does not go through the kernel) direct writing to the registers
    by DMA
  
 INSTALLATION:
 
    CLone the content into any folder in your JETSON
  
    Change access rights to the make.sh: chmod +x make.sh
  
    Execute make.sh: sudo ./make.sh
  
    That's it the library should be installed and ready to be used
  
 HOW TO:
 
    See 'jetgpio_example.c' for an example of how to use the library, the main functions provided are there

THIS IS A VERY ALPHA, INCOMPLETE VERSION AND ALL SORT OF BUGS ARE EXPECTED! 

The library has been tested on a Jetson nano SCO family: tegra210 (TX1), Board: P3449-0000

![image](https://user-images.githubusercontent.com/47650457/164944765-998ca31c-d72c-4d2b-8cbc-7bea594ce8d5.png)






