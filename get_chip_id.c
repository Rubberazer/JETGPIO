/*
 * Try to identify the hardware and verify compatibility
 * Compile with: gcc -Wall -Werror -o get_chip_id get_chip_id.c
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/file.h>
#include <sys/types.h>
#include <string.h>
#include "jetgpio.h"

int chip_get_id(void) {
  int model = 0;
  int fd_id;
  int pagesize = sysconf(_SC_PAGESIZE);
  static void *baseAPBMISC;
  static void *baseMISC;
  static volatile uint32_t *nano_get_id;
  static volatile uint32_t *orin_get_id;
  size_t page = sysconf(_SC_PAGESIZE);
  size_t n_pages = sysconf(_SC_PHYS_PAGES);
  size_t total_ram = page*n_pages/(1024*1024*1024);
  
  fd_id = open("/dev/mem", O_RDWR | O_SYNC);
  if (fd_id < 0) {
    perror("/dev/mem");
    fprintf(stderr, "Please run this program as root (for example with sudo)\n");
    exit (-1);
  }

  //  Mapping APB_MISC_BASE
  baseAPBMISC = mmap(0, pagesize, PROT_READ | PROT_WRITE, MAP_SHARED, fd_id, APB_MISC_BASE);
  if (baseAPBMISC == MAP_FAILED) {
    fprintf(stderr, "mmap failure on chip_get_id function\n");
    exit (-2);
  }

  //  Mapping MISC
  baseMISC = mmap(0, pagesize, PROT_READ | PROT_WRITE, MAP_SHARED, fd_id, MISC);
  if (baseMISC == MAP_FAILED) {
    fprintf(stderr, "mmap failure on chip_get_id function\n");
    exit (-3);
  }

  nano_get_id = (uint32_t volatile *)((char *)baseAPBMISC + APB_MISC_GP_HIDREV_0);
  orin_get_id = (uint32_t volatile *)((char *)baseMISC + MISCREG_HIDREV_0);

  // Trying for Orin first
  if (((*orin_get_id >>8) & 0xFF) == 0x23) {
      if (total_ram > 24) {
	model = ORINAGX;
	printf("T234/Orin AGX detected\n");
      }
      else {
	model = ORIN;
	printf("T234/Orin Nano or NX detected\n");
      }
  }
  
  // Trying for Nano Classic next
  else if (((*nano_get_id >>8) & 0xFF) == 0x21) {
    model = NANO;
    printf("T210/Jetson Nano detected\n");
  }
  else {
    model = 0;
    printf("No compatible Jetson hardware detected\n");
  }
  
  // Ummapping APB_MISC_BASE registers 
  munmap(baseAPBMISC, pagesize);

  // Ummapping MISC registers 
  munmap(baseMISC, pagesize);

  // close /dev/mem 
  close(fd_id);
  
  return model;
}

int main (void) {

  int model = 0;
  char hardware[10];
  model = chip_get_id();
  switch (model) {
  case NANO:  
    strcpy(hardware, "nano");
    break;
  case ORIN:
    strcpy(hardware, "orin");
    break;
  case ORINAGX:
    strcpy(hardware, "orinagx");
    break;
  default:
    /* Unsupported hardware */
    printf("\tUnsupported hardware, if you think this is not correct and you believe this is either a:\n");
    printf("\t\tJetson Nano\n");
    printf("\t\tJetson Orin Nano\n");
    printf("\t\tJetson Orin NX\n");
    printf("\t\tJetson Orin AGX\n");
    printf("\tAnd therefore supported, you can still force install with:\n");
    printf("\t\tsudo make <model> followed by sudo make install\n");
    printf("\te.g. sudo make orin to compile for Orin Nano or NX\n");
    exit(EXIT_FAILURE);
  }
  FILE *fp = fopen("hardware", "w+");
  fwrite(hardware, 1, strlen(hardware), fp);
  fclose(fp);
  exit(EXIT_SUCCESS);
}

