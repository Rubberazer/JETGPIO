# JETGPIO library

C library to manage the JETSON NANO GPIO v0.1 (ALPHA)

FUNCTIONALITY:

    JETSON NANO (TX1) family only, so far it does not support the Xaviers,etc. No hardware to test it
  
    GPIO control of all the header pinout as input or output
    
    PWM (hardware) control on header pins 32 & 33
    
    I2C serial communication over pins: 27 (GEN1_I2C_SDA) & 28 (GEN1_I2C_SCL)  and 3 (GEN2_I2C_SDA) 
    
    & 5 (GEN2_I2C_SCL)
  
    SPI and UART are NOT supported yet
  
    No need of previous kernel configuration, the library will take care of that on runtime
  
    Low latency is expected (it does not go through the kernel) direct writing to the registers (DMA)
  
 INSTALLATION:
 
    Clone the content into any folder in your JETSON
  
    Change access rights to make.sh executable: sudo chmod +x make.sh (no proper makefile yet, it will come)
  
    Execute make.sh: sudo ./make.sh
  
    That's it, the library should be installed and ready to be used
  
 HOW TO:
 
    See 'jetgpio_example.c' for an example of how to use the library, the main functions provided are there
    
    See 'jetgpio_i2c_example.c' for an example of how to use the i2c comms with a MPU6050 gyroscope connected 
    
    to i2c 0 (pins 27 & 28)
    
    The library functions names and usage mimic the ones of the pigpio library (which I recommend 
    
    if you work with Raspberry Pis). Learnt a lot from that one
    

THIS IS A VERY ALPHA, INCOMPLETE VERSION AND ALL SORT OF BUGS ARE EXPECTED! 

The library has been tested on a Jetson nano SCO family: tegra210 (TX1), Board: P3449-0000

![image](https://user-images.githubusercontent.com/47650457/164944765-998ca31c-d72c-4d2b-8cbc-7bea594ce8d5.png)






