# JETGPIO_TESTS
First attempts playing with the Jetson NANO GPIO

Testing pins of the Jetson nano 40 pin header GPIO, setting pin 33 (GPIO_PE6) as an output and changing the value, the thing works, tested with an oscilloscope. still very initial attempts.
No libraries used, mapped directly into user memory (mmap) and accessing the tegra registers bypassing hte kernel for low latency and higher degree of control, this looks promissing.


