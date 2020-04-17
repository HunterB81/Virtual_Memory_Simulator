#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <string.h>
#include "project_headers.h"
#include "VirtualMem.c"

#define VPAGES 1024 // number of virtual pages
#define TLB_SIZE 8 // TLB size
#define FRAMES 256 // number of frames


//Global variables to keep track of the statistics in memory access to pass on to the case studys
  int THit , TMiss , TWrites , counter , 
  calculated ,PAccess , PHit , PMiss , shootdown , DiskR ,
  DiskW ,PWrites , PageE ;


int main(void)
 {
   
  int i, read_write;
  unsigned int address;
  char r_w;

  double THitRate = 0.0, PHitRate = 0.0;
  srand(time(0));
  
  unsigned int PageTable[VPAGES][4];
  unsigned int TLB [TLB_SIZE][5];
  unsigned int FrameTable[FRAMES];


  memset(TLB, 0, sizeof(TLB[0][0]) * TLB_SIZE * 5);
  memset(PageTable, 0, sizeof(PageTable[0][0]) * VPAGES * 4);
  memset(FrameTable, 0, sizeof(FrameTable[0]) * FRAMES);


  printf("-----------------------------------------\n");
  printf("--------VIRTUAL MEMORY SIMULATION--------\n");
  printf("-----------------------------------------\n");

  //Generating 5 addresses
  for(i = 0; i<5; i++)
  {
    address = (rand() % VPAGES);
    read_write = (rand() % 2);

    if(read_write == 0)
    {
      r_w = 'r';
    }
    else
    {
      r_w = 'w';
    }
    //Printing addresses
     printf("T: %7d     Address:   %3c   %7d       VPN: %d\n", i+1, r_w, address, (address/VPAGES));

    memory_access(TLB, TLB_SIZE, PageTable, VPAGES, FrameTable, FRAMES, address, read_write);

  }

  THitRate = (double) THit / (THit + TMiss);
  PHitRate = (double)  PHit / (PHit + PMiss);

  printf("\n-----------------------------------------------------------------------------------\n");
  printf("\n--------------------------------SIMULATION RESULTS---------------------------------\n");
  printf("\n-----------------------------------------------------------------------------------\n");
  printf("TLB hits: %-18d TLB misses: %-15d TLB hit rate: %.2lf%%\n", THit, TMiss, (THitRate*100));
  printf("TLB Shootdowns: %-12d TLB Writes: %d\n\n",shootdown, TWrites);
  printf("Pg Table Accesses: %d\n", PAccess);
  printf("Pg Table hits: %-13d Pg Faults: %-16d Pg Table hit rate: %.2lf%%\n", PHitRate, PMiss, (PHitRate*100));
  printf("Pg evictions: %-14d Pg Table writes: %d\n\n", PageE, PWrites);
  printf("Hard disk reads: %-11d Hard disk writes: %d\n", DiskR, DiskW);
  printf("-----------------------------------------------------------------------------------\n");

  return 0;
}