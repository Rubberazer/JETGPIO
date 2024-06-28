/* Usage example of the JETGPIO library
 * Compile with: gcc -Wall -o jetgpio_round jetgpio_round_trip.c -ljetgpio -lrt
 * Execute with: sudo ./jetgpio_round
 */

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <jetgpio.h>

#define BILLION 1000000000L

int main(int argc, char *argv[])
{
  int Init;
  uint64_t diff;
  struct timespec start, end;

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

  // Measuring monotonic time now
  clock_gettime(CLOCK_MONOTONIC, &start);	

  // Writing 1 to pin 38
  gpioWrite(38, 1);

  while (!gpioRead(40)) {}

  // Measuring monotonic time after the loop
  clock_gettime(CLOCK_MONOTONIC, &end);	

  // Calculating time difference for round trip e.g. time invested in activating the output + the time it takes for the input to detect the level
  diff = BILLION * (end.tv_sec - start.tv_sec) + end.tv_nsec - start.tv_nsec;
  printf("elapsed time = %llu nanoseconds\n", (long long unsigned int) diff);

  gpioWrite(38, 0);

  // Terminating library 
  gpioTerminate();

  exit(0);
	
}

