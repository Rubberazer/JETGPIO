/* Usage example of the JETGPIO library
 * Compile with: g++ -Wall -o spi_loop spi_loop.cpp -ljetgpio
 * Execute with: sudo ./spi_loop
 */

#include <iostream>
#include <unistd.h>
#include <jetgpio.h>

int main(int argc, char *argv[])
{
  int Init;
  int SPI_init;
  int SPI_stat;
  char tx[7] = {0,};
  char rx[7] = {0,};

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

  /* Port SPI2 pins: 37, 22, 13 & 18
     to perform a simple loop test, pins 22: SPI2_MISO & 37:SPI2_MOSI should be connected
     with a short jumper cable, when the cable is disconnected the output on screen will show all the rx values as zeros 
     spiOpen() parameters go as follows: spiOpen(port number, speed in Hz, mode, cs pin delay in us, 
     bits per word, least significant bit first, cs change)
  */

  SPI_init = spiOpen(1, 5000000, 0, 0, 8, 1, 1);
  if (SPI_init < 0)
    {
      /* Port SPI2 opening failed */
      printf("Port SPI2 opening failed. Error code:  %d\n", SPI_init);
      exit(Init);
    }
  else
    {
      /* Port SPI2 opened  okay*/
      printf("Port SPI2 opened OK. Return code:  %d\n", SPI_init);
    }

  tx[0] = 0xFF;
  tx[1] = 0xAA;
  tx[2] = 0xBB;
  tx[3] = 0xCC;
  tx[4] = 0xDD;
  tx[5] = 0xFF;
  tx[6] = 0x11;

  //Transfer data
  int i = 0;
  while (i<20){

    SPI_stat = spiXfer(SPI_init, tx, rx, 7);

    if (SPI_stat < 0)
      {
	/* Spi transfer failed */
	printf("Spi port transfer failed. Error code:  %d\n", SPI_stat);
	exit(Init);
      }
    else
      {
	/* Spi transfer okay*/
	printf("Spi port transfer OK. Return code:  %d\n", SPI_stat);
      }

    printf("tx0:%x --> rx0:%x\n",tx[0], rx[0]);
    printf("tx1:%x --> rx1:%x\n",tx[1], rx[1]);
    printf("tx2:%x --> rx2:%x\n",tx[2], rx[2]);
    printf("tx3:%x --> rx3:%x\n",tx[3], rx[3]);
    printf("tx4:%x --> rx4:%x\n",tx[4], rx[4]);
    printf("tx5:%x --> rx5:%x\n",tx[5], rx[5]);
    printf("tx6:%x --> rx6:%x\n",tx[6], rx[6]);

    rx[0] = 0;
    rx[1] = 0;
    rx[2] = 0;
    rx[3] = 0;
    rx[4] = 0;
    rx[5] = 0;
    rx[6] = 0;

    i++;
    sleep(1);
  }

  // Closing spi port
  spiClose(SPI_init);

  // Terminating library
  gpioTerminate();

  exit(0);
}
