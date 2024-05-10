/* Reading data from an ADC7606 using parallel comms to an AGX Orin this is a
 * naive approach, each 16 bits sample output  are read sequentially, this process takes approx. 20us
 * per sample Vx
 * Assumptions are: 
 * 	CONVSTA & B start the conversion process, each conversion process needs arising edge from those pins
 *	While conversion is taking place, pin BUSY is high (1)
 * Compile with: gcc -Wall -o read_ADC7606_v1 read_ADC7606_v1.c -ljetgpio
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <jetgpio.h>

//Giving meaningful names to pins in the Orin 
#define CS_RD 3
#define CONVST_AB 5
#define BUSY 7

//#define BILLION 1000000000L
//uint64_t diff;
//struct timespec start, end;

int main(void) {

  int Vx = 0;
  //List of AGX data pins DB0 to DB15 = pin8 to 28 
  int pin_level[16] = {0};
  unsigned pin_number[16] = {8, 10, 11, 12, 13, 15, 16, 18, 19, 21, 22, 23, 24 \
                             ,26, 27, 28};
  gpioInitialise();
  
  //Setting BUSY and data pins as inputs
  for (unsigned i = 0;i < 16; i++) {
    gpioSetMode(pin_number[i], JET_INPUT);
  }
  gpioSetMode(BUSY, JET_INPUT);

  //Setting CS/RD and CONVSTA & B as outputs
  gpioSetMode(CS_RD, JET_OUTPUT);
  gpioSetMode(CONVST_AB, JET_OUTPUT);

  //Setting CONVST to low level, intial state 
  gpioWrite(CONVST_AB, 0);
  
  //Setting CS & RD to high level, output is all at tri state, initial state
  gpioWrite(CS_RD, 1);
  
  while(1) {
    //Measure how long it takes in nanoseconds
    //clock_gettime(CLOCK_MONOTONIC, &start);

    Vx = 0;
    
    //Setting CONVST to high level, conversion starts 
    gpioWrite(CONVST_AB, 1);
    
    //Checking if BUSY is low (conversion completed) if not wait 1us
    while(gpioRead(BUSY)) {
      usleep(1);
    }
    
    //Enabling data output CS/RD to 0 to move to the next value
    gpioWrite(CS_RD, 0);
    

    //Reading all the data inputs pins this process will take approximately 16us
    for (unsigned i = 0;i < 16; i++) {
      pin_level[i] = gpioRead(pin_number[i]);
    }
    
    //Setting CS/RD to 1
    gpioWrite(CS_RD, 1);
    
    //Setting CONVST to 0
    gpioWrite(CONVST_AB, 0);
    
    //Calculating final result (sample Vx) here most significant bit is pin 15
    for (unsigned i = 0;i < 16;i++) {

      Vx |= pin_level[i] << i; 
    }
    
    printf("\r%d", Vx);
    
    //clock_gettime(CLOCK_MONOTONIC, &end);
    //diff = BILLION * (end.tv_sec - start.tv_sec) + end.tv_nsec - start.tv_nsec;
    //printf("elapsed time = %llu nanoseconds\n", (long long unsigned int) diff);   
  }
    
  gpioTerminate();
  
  return 0;
}
