/* Usage example of the JETGPIO library
 * Compile with: gcc -Wall -o jetgpio_edge jetgpio_edge.c -ljetgpio
 * Execute with: sudo ./jetgpio_edge
 */

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>	/* for catching exceptions e.g. control-C*/
#include <jetgpio.h>

/* Global variable to interrupt the loop later on*/
static volatile int interrupt = 1;
unsigned long timestamp;

/* Ctrl-c signal function handler */
void inthandler(int signum) 
{
  usleep(1000);
  printf("\nCaught Ctrl-c, coming out ...\n");
  interrupt = 0;
}

/* Function to be called upon if edge is detected */
void calling()
{
  printf("edge detected with EPOCH timestamp: %lu\n", timestamp);
  // terminating while loop
  //interrupt = 0;
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

  // Setting up pin 3 as INPUT first
  int stat = gpioSetMode(3, JET_INPUT);
  if (stat < 0)
    {
      // gpio setting up failed 
      printf("gpio setting up failed. Error code:  %d\n", stat);
      exit(Init);
    }
  else
    {
      // gpio setting up okay
      printf("gpio setting up okay. Return code:  %d\n", stat);
    }

  // Now setting up pin 3 to detect edges, rising & falling edge with a 1000 useconds debouncing and when event is detected calling func "calling"
  int stat2 = gpioSetISRFunc(3, EITHER_EDGE, 1000, &timestamp, &calling);
  if (stat2 < 0)
    {
      /* gpio setting up failed */
      printf("gpio edge setting up failed. Error code:  %d\n", stat2);
      exit(Init);
    }
  else
    {
      /* gpio setting up okay*/
      printf("gpio edge setting up okay. Return code:  %d\n", stat2);
    }

  /* Now wait for the edge to be detected */
  printf("Capturing edges, press Ctrl-c to terminate\n");
  while (interrupt) {
    // Do some stuff
    sleep(1);
  }
  // Terminating library
  gpioTerminate();
  exit(0);
}

