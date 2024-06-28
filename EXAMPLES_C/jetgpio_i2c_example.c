/* Usage example of the JETGPIO library
 * Compile with: gcc -Wall -o jetgpio_i2c_example jetgpio_i2c_example.c -ljetgpio
 * Execute with: sudo ./jetgpio_i2c_example
 */

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <jetgpio.h>

int main(int argc, char *argv[])
{
  int Init;
  int gyro_x_H = 0;
  int gyro_x_L = 0;
  float gyro_x = 0;
  int gyro_y_H = 0;
  int gyro_y_L = 0;
  float gyro_y = 0;
  int gyro_z_H = 0;
  int gyro_z_L = 0;
  float gyro_z = 0;

  /* MPU-6050 Registers */
  const int MPU6050_SLAVE_ADDRESS = 0x68;
  const int PWR_MGMT_1 = 0x6B;
  const int ACCEL_CONFIG = 0x1C;
  const int ACCEL_RANGE_4G = 0x08;
  const int GYRO_CONFIG = 0x1B;
  const int GYRO_RANGE_250DEG = 0x00;
  const float GYRO_SCALE_MODIFIER_250DEG = 131.0;
  const int GYRO_XOUT0 = 0x43;
  const int GYRO_YOUT0 = 0x45;
  const int GYRO_ZOUT0 = 0x47;


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

  /* Opening the connection to the I2C slave MPU6050 */
  int MPU6050 = i2cOpen(0,0);
  
  if (MPU6050 >=  0)
    {
      /* Opening I2C port OK*/
      printf("Open I2C port OK. Return code:  %d\n", MPU6050);
      printf("MPU6050 number handler:  %d\n", MPU6050);
    }
  else
    {
      /* Problems opening I2C port*/
      printf("Open I2C port failed. Quitting MPU6050 Error code:  %d\n", MPU6050);
    }
    
  /* Wake up the MPU-6050 with slave address 0x68 since it starts in sleep mode */
  int writestat = i2cWriteByteData(MPU6050, MPU6050_SLAVE_ADDRESS, PWR_MGMT_1, 0x00); 
  printf("write return: %d\n",writestat);
  usleep(100000);  
  
  /* Now set up the accelerator  range to 4G */
  writestat = i2cWriteByteData(MPU6050, MPU6050_SLAVE_ADDRESS, ACCEL_CONFIG, ACCEL_RANGE_4G); 
  printf("write return: %d\n",writestat);
  usleep(100000);
  
  /* Now set up the gyroscope  range to 250 deg/second */
  writestat = i2cWriteByteData(MPU6050, MPU6050_SLAVE_ADDRESS, GYRO_CONFIG, GYRO_RANGE_250DEG);
  printf("write return: %d\n",writestat);
  usleep(100000);

  int x = 0;

  while (x<1000) {
	
    // Reading gyroscope values
    gyro_x_H = i2cReadByteData(MPU6050, MPU6050_SLAVE_ADDRESS, GYRO_XOUT0); /* getting the H register 15:8 */
    gyro_x_L = i2cReadByteData(MPU6050, MPU6050_SLAVE_ADDRESS, GYRO_XOUT0+1); /* getting the L register 7:0 */
    gyro_x = (gyro_x_H << 8) + gyro_x_L;
    if (gyro_x >= 0x8000)
      {
	gyro_x = -(65535 - gyro_x) + 1;
      }
    gyro_x = gyro_x/GYRO_SCALE_MODIFIER_250DEG;
    printf("gyro_x: %f\n",gyro_x);
    
    
    gyro_y_H = i2cReadByteData(MPU6050, MPU6050_SLAVE_ADDRESS, GYRO_YOUT0); /* getting the H register 15:8 */
    gyro_y_L = i2cReadByteData(MPU6050, MPU6050_SLAVE_ADDRESS, GYRO_YOUT0+1); /* getting the L register 7:0 */
    gyro_y = (gyro_y_H << 8) + gyro_y_L;
    if (gyro_y >= 0x8000)
      {
	gyro_y = -(65535 - gyro_y) + 1;
      }
    gyro_y = gyro_y/GYRO_SCALE_MODIFIER_250DEG;
    printf("gyro_y: %f\n",gyro_y);
    
    
    gyro_z_H = i2cReadByteData(MPU6050, MPU6050_SLAVE_ADDRESS, GYRO_ZOUT0); /* getting the H register 15:8 */
    gyro_z_L = i2cReadByteData(MPU6050, MPU6050_SLAVE_ADDRESS, GYRO_ZOUT0+1); /* getting the L register 7:0 */
    gyro_z = (gyro_z_H << 8) + gyro_z_L;
    if (gyro_z >= 0x8000)
      {
	gyro_z = -(65535 - gyro_z) + 1;
      }
    gyro_z = gyro_z/GYRO_SCALE_MODIFIER_250DEG;
    printf("gyro_z: %f\n",gyro_z);
    
    usleep(10000);
    x++;
  }

  /* Closing i2c connection */
  int i2cstat = i2cClose(MPU6050);

  if (i2cstat >=  0)
    {
      /* I2C port closed OK*/
      printf("Closing I2C port OK. Return code:  %d\n", i2cstat);
    }
  else
    {
      /* Not possible to close I2C port */
      printf("Closing I2C port failed. Quitting MPU6050 thread Error code:  %d\n", i2cstat);
    }

  /* Terminating library */
  gpioTerminate();
  exit(0);
	
}

