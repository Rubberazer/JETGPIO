/* Usage example of the JETGPIO library
 * Compile with: gcc -Wall -o jetgpio_extperiph jetgpio_extperiph.c -ljetgpio
 * Execute with: sudo ./jetgpio_extperiph
 */

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <jetgpio.h>

int main(void) {
  gpioInitialise();

  // Change clock rate of extperiph4 on both Orin Nano and AGX
  extPeripheralRate(EXTPERIPH4, 3200000);

  // Enable clock on pin 31 on Nano or pin 7 on AGX
  extPeripheralEnable(EXTPERIPH4);

  // Sleep a bit so we can check the pin
  sleep(5); 

  // Disable clock on pin 31/7
  extPeripheralDisable(EXTPERIPH4);
  
  gpioTerminate();

  exit(0);
	
}

