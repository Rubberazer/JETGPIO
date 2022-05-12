/* Usage example of the JETGPIO library
 * Compile with: gcc -Wall -g -o jetgpio_example jetgpio_example.c -I./ -ljetgpio
 * Execute with: sudo ./jetgpio_example
*/

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include "jetgpio.h"

int main(int argc, char *argv[])
{
int Init;

Init = gpioInitialise();
if (Init < 0)
{
   /* jetgpio initialisation failed */
   printf("Jetgpio initialisation failed. Error code:  %d\n", Init);
   exit(Init);
}
else
{
   /* jetgpio initialised okay*/
   printf("Jetgpio initialisation OK. Return code:  %d\n", Init);
}	

// Setting up pin 3 as OUTPUT and 38 as INPUT

int stat1 = gpioSetMode(3, JET_OUTPUT);
if (stat1 < 0)
{
   /* gpio setting up failed */
   printf("gpio setting up failed. Error code:  %d\n", stat1);
   exit(Init);
}
else
{
   /* gpio setting up okay*/
   printf("gpio setting up okay. Return code:  %d\n", stat1);
}

int stat2 = gpioSetMode(7, 0);
if (stat2 < 0)
{
   /* gpio setting up failed */
   printf("gpio setting up failed. Error code:  %d\n", stat2);
   exit(Init);
}
else
{
   /* gpio setting up okay*/
   printf("gpio setting up okay. Return code:  %d\n", stat2);
}

/* Setting up PWM frequency pin 32 */

int PWMstat = gpioSetPWMfrequency(32, 10000);

if (PWMstat < 0)
{
   /* PWM frequency set up failed */
   printf("PWM frequency set up failed. Error code:  %d\n", PWMstat);
   exit(Init);
}
else
{
   /* PWM frequency set up okay*/
   printf("PWM frequency set up okay. Return code:  %d\n", PWMstat);
}

int PWMstat2 = gpioPWM(32, 200);


if (PWMstat2 < 0)
{
   /* PWM start on failed */
   printf("PWM start failed. Error code:  %d\n", PWMstat2);
   exit(Init);
}
else
{
   /* PWM started on okay*/
   printf("PWM started up okay. Return code:  %d\n", PWMstat2);
}

int x =0;
int level = 0;
    while (x<5) {
        gpioWrite(3, 1);
        usleep(1000);
        level = gpioRead(7);
        printf("level: %d\n",level);
        sleep(2);
        gpioWrite(3, 0);
        usleep(1000);
        level = gpioRead(7);
        printf("level: %d\n",level);
        sleep(2);
		x++;
    }
// Terminating library 

gpioTerminate();
exit(0);
	
}

