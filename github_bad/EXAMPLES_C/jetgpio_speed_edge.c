/* Usage example of the JETGPIO library
 * Compile with: gcc -Wall -o jetgpio_speed jetgpio_speed_edge.c -ljetgpio -lrt
 * Execute with: sudo ./jetgpio_speed
 */

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>	/* for catching exceptions e.g. control-C*/
#include <time.h>
#include <jetgpio.h>

#define BILLION 1000000000L

/* Global variable to interrupt the loop later on*/
static volatile int interrupt = 1;
unsigned long timestamp;
// To measure time
unsigned long diff;
struct timespec start;

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
  diff = timestamp - (BILLION * (start.tv_sec) + start.tv_nsec);
  printf("time stamp = %lu nanoseconds\n", timestamp);
  printf("start time = %lu nanoseconds\n", (BILLION * (start.tv_sec) + start.tv_nsec));
  printf("elapsed time = %lu nanoseconds\n", diff);
  // terminating while loop
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

  // Setting up pin 38 as OUTPUT and 40 as INPUT
  gpioSetMode(38, JET_OUTPUT);
  gpioSetMode(40, JET_INPUT);

  // Now setting up pin 40 to detect rising edge
  gpioSetISRFunc(40, RISING_EDGE, 0, &timestamp, &calling);

  // Measuring time now
  clock_gettime(CLOCK_REALTIME, &start);	

  // Writing 1 to pin 38
  gpioWrite(38, 1);

  /* Now wait for the edge to be detected 
     printf("Capturing edges, press Ctrl-c to terminate\n"); */
  while (interrupt) {}

  gpioWrite(38, 0);

  // Terminating library
  gpioTerminate();
  exit(0);
}

