# JETGPIO_TESTS
First attempts playing with the Jetson NANO GPIO

Testing pins of the Jetson nano 40 pin header GPIO, setting pin 33 (GPIO_PE6) as an output and changing the value, the thing works, tested with an oscilloscope. Still very initial attempts, will increase to add all pins when i have the time and extra functionality when i have the time.
No libraries used, mapped directly into user memory (mmap) and accessing the tegra registers bypassing hte kernel for low latency and higher degree of control, this looks promissing.


![image](https://user-images.githubusercontent.com/47650457/164944685-6fb50d38-ee48-4e68-86cd-bb8d47e4dcee.png)




