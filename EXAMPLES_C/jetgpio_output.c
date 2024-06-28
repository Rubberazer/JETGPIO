/* Usage example of the JETGPIO library
 * Compile with: gcc -Wall -o jetgpio_output jetgpio_output.c -ljetgpio
 * Execute with: sudo ./jetgpio_output
 */

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <jetgpio.h>
#include <time.h>
#include <signal.h>

/* Global variable to interrupt the loop later on*/
static volatile int interrupt = 1;

/* Ctrl-c signal function handler */
void inthandler(int signum) 
{
  usleep(1000);
  printf("\nCaught Ctrl-c, coming out ...\n");
  interrupt = 0;
}

int main(int argc, char *argv[])
{
  int Init;

  /* Capture Ctrl-c */
  signal(SIGINT, inthandler);

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

  // Setting up pin 38 as OUTPUT

  gpioSetMode(38, JET_OUTPUT);

  while (interrupt) {
    gpioWrite(38,1);
    gpioWrite(38,0);

  }

  // Pin 38 at 0
  gpioWrite(38, 0);

  // Terminating library 
  gpioTerminate();

  exit(0);
	
}

