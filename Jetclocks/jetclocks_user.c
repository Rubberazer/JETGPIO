/*
 * Compile with gcc -Wall -Werror -o jetclocks_user jetclocks_user.c 
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/ioctl.h>

#include "jetclocks.h"

int main() {
    
    struct jetclk clock = {0};

    int dev = open("/dev/jetclocks", O_WRONLY);
    if(dev < 0) {
	printf("Opening /dev/jetclocks not possible\n");
	return -1;
    }
    
     /* Setting up rate to clock "spi1" */

    strncpy(clock.clk, "spi1", sizeof(clock.clk));
    clock.clk_set_rate = 40800000; //Default is 81600000

    ioctl(dev, CLK_SET_RATE, &clock);
    printf("Setting rate: %lu on clock %s: \n",clock.clk_set_rate, clock.clk);
    
    /* Checking current rate on clock "spi1" */

    ioctl(dev, CLK_GET_RATE, &clock);
    printf("Rate on clock %s is %lu\n",clock.clk, clock.clk_rate);

     /* Enabling clock "spi1" */
    
    ioctl(dev, CLK_ENABLE, &clock);
    printf("Enabling clock %s: \n",clock.clk);

    /* Now checking whether the clok is enabled*/

    ioctl(dev, CLK_IS_ENABLED, &clock);
    printf("clock %s status(0 disabled, 1 enabled): %d\n",clock.clk, clock.clk_enabled);
    
    close(dev);
    return 0;
}
