/* Usage example of the JETGPIO library
 * Compile with: g++ -Wall -o test_PWM test_PWM.cpp -ljetgpio
 * Execute with: sudo ./test_PWM
 */

#include <iostream>
#include <unistd.h>
#include <signal.h>	/* for catching exceptions e.g. control-C*/
#include <jetgpio.h>

void inthandler(int signum) 
{
  usleep(1500);
  gpioTerminate();
  printf("Caught signal %d, coming out ...\n", signum);
  exit(1);
}

int main(int argc, char *argv[])
{
  int Init;
  int status;
  signal(SIGINT, inthandler);
  signal(SIGHUP, inthandler);
  signal(SIGABRT, inthandler);
  signal(SIGILL, inthandler);
  signal(SIGSEGV, inthandler);
  signal(SIGTERM, inthandler);
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

  gpioSetPWMfrequency(33, 1000);

  gpioPWM(33, 0);

  int x = 0;

  int level = 0;

  // Increase brightness 
  while (x<=256) {

    usleep(100000);

    level = x;

    printf("level: %d - ",level);

    status = gpioPWM(33, x);

    printf("status: %d\n",status);

    usleep(100000);

    x=x+1;
  }

  x = 256;

  // Decrease brightness
  while (x>=0) {

    usleep(100000);

    level = x;

    printf("level: %d - ",level);

    status = gpioPWM(33, x);

    printf("status: %d\n",status);

    usleep(100000);

    x=x-1;

  }
  gpioTerminate();
  exit(0);
}
