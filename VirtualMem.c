
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <string.h>
#include "project_headers.h"

#define VPAGES 1024 // number of virtual pages
#define TLB_SIZE 8 // TLB size
#define FRAMES 256 // number of frames


  //Global variables to keep track of the statistics in memory access to pass on to the case studys
  int THit = 0, TMiss = 0, TWrites = 0, counter = 0, 
  calculated = 0,PAccess = 0, PHit = 0, PMiss = 0, shootdown = 0, DiskR = 0,
  DiskW = 0,PWrites = 0, PageE = 0;


//Start of TLB Functions


// Returns the TLB entry that corresponds to a virtual page
// Returns -1 if the page's translation is not in the TLB
int TLB_lookup(unsigned int TLB[][5], int size, unsigned int vpn)
{
  int i, j;

  //Looping throught the TLB
  for(i = 0; i< size; i++)
  {
      //The value in the TLB is equal to vpn return it
      if( TLB[i][3] == vpn)
      {
        return i;
      }
      
  }

  return -1;
  
}

// Returns the index of the first available TLB entry
// Returns -1 if all TLB entries are used
int get_available_TLB_entry (unsigned int TLB[][5], int size)
{
  int i;

  //Looping through the TLB
  for(i = 0; i < size; i++)
  {
    //If the value is equal to 1 it is available
    if( TLB[i][0] == 1)
    {
      return i;
    }

    return -1;
  }

  return -1;
}

// Selects the TLB entry that will be evicted
// Pre-condition: All TLB entries are full
// Criteria: Select a random entry with ref.bit=0; if all ref.bit=1, select a random entry
unsigned int select_TLB_shootdown_candidate(unsigned int TLB[][5], int size)
{
  int min = 0;
  int counter = 0;
  int i, j = 0,  max = size;

  //Setting random num to be between 0 and size
  srand(time(0));
  int random2 = (rand() % (max-min + 1)) + min;
  
  //Looping through the TLB to get amount of candidates that equal 0
  for(i = 0; i<size; i++)
  {
    if(TLB[i][2] == 0)
    {
      counter++;
    }
  }

  if(counter != 0)
  {
    //This array is to randomize the entrys
   int CandidatesZero[counter]; 
   int random = (rand() % (counter-min + 1)) + min;

  //Looping again through the TLB to set values into my array
   for(i = 0; i < size; i++)
   {
      if(TLB[i][2] == 0)
      {
        //Setting the values into the array
        CandidatesZero[j] = i;
        j++;
        
      }
   }
  //Returning a random entry with reference bit equal to 0
   return CandidatesZero[random];
  }

  return TLB[random2][2];
}


// Perform a TLB shootdown (set V=0, copy D,R bits to the page table)
// Pre-condition: shootdown entry is currently valid
// Parameter index is the TLB entry to shoot down
void TLB_shootdown \
(unsigned int TLB[][5], int tlb_size, unsigned int PageTable[][4], int page_table_size, int index)
{
  int vpn;
 
   
    TLB[index][0] = 0;
    vpn = TLB[index][3];
    PageTable[vpn][1] = TLB[index][1];
    PageTable[vpn][1] = TLB[index][2];

}


// Copies a translation from the Page Table to the TLB
// The first available TLB entry is used; otherwise, a TLB shootdown is performed
// It copies the D,R bits and the frame number from the page table
// Parameter: virtual page number
// Returns: (+1: shootdown performed)
int cache_translation_in_TLB \
(unsigned int TLB[][5], int tlb_size, unsigned int PageTable[][4], int page_table_size, unsigned int vpn)
{
  int i;
  int available, object;
  int TLBvpn;
  int counter = 0;

  //Looking for first available TLB entry
  for(i=0; i < tlb_size; i++)
  {
    if(TLB[i][0] == 1)
    {
      available = i;
      counter++;
      break;
    }

  }
  
  if(counter != 0)
  {
    TLB[available][0] = 1;
    TLB[available][1] = PageTable[vpn][1];
    TLB[available][2] = PageTable[vpn][2];
    TLB[available][3] = vpn;
    TLB[available][4] = PageTable[vpn][3];

    return 0;
  }
  else
  {
    object = select_TLB_shootdown_candidate(TLB, tlb_size);
    TLB_shootdown(TLB, tlb_size, PageTable, page_table_size, object);

    TLB[available][0] = 1;
    TLB[available][1] = PageTable[vpn][1];
    TLB[available][2] = PageTable[vpn][2];
    TLB[available][3] = vpn;
    TLB[available][4] = PageTable[vpn][3];

    return 1;
  }
  
}


//Start of Page Table Functions


// Returns the index of the first available frame
// Returns -1 if all frames are allocated
int get_available_frame (unsigned int FrameTable[], int size)
{
  int i;
  for( i = 0; i < size; i++)
  {
    if(FrameTable[i] == 0)
    {
      return FrameTable[i];
    }
  }

  return -1;
}


// Search the PageTable for VDR values passed as parameters
// Return -1 if not found; otherwise, return the index of one such
// randomized entry (using rand function)
// Pre-condition: VDR are 0 or 1
int search_PageTable_by_VDR(unsigned int PageTable[][4], int size, int V, int D, int R)
{
  int i;
  int min = 0, j = 0;
  int counter = 0;

  srand(time(0));

  for(i = 0; i<size; i++)
  {
    if (PageTable[i][0] == V && PageTable[i][1] == D && 
    PageTable[i][2] == R)
    {
      counter++;
    }
  }
  if(counter != 0)
  {
    int Entry[counter];
    int random = (rand() % (counter-min + 1)) + min;

    for(i=0; i<size; i++)
    {
      if (PageTable[i][0] == V && PageTable[i][1] == D && 
      PageTable[i][2] == R)
     {
       Entry[j] = i;
        j++;
     }
    }

    return Entry[random];
  }

  return -1;
}

// Selects the virtual page that will be replaced
// Pre-condition: All the frames are allocated
// Criteria: Valid must be 1; choose in order as below
//     VDR.bits: 100   110   101   111
// Between pages with the same category, randomize (using rand)
unsigned int select_page_eviction_candidate(unsigned int PageTable[][4], int size)
{
  int i; 
  int counter = 0;
  for(i = 0; i< size; i++)
  {
    if(PageTable[i][0] == 1)
    {
      if((PageTable[i][1] == 0) && (PageTable[i][2] == 0))
      {
        counter++;
      }
      else if((PageTable[i][1] == 1) && (PageTable[i][2] == 0))
      {
        counter++;
      }
      else if((PageTable[i][1] == 0) && (PageTable[i][2] == 1))
      {
        counter++;
      }
      else if((PageTable[i][1] == 1) && (PageTable[i][2] == 1))
      {
        counter++;
      }
    }
  }

  if(counter != 0)
  {
    int j = 0;
    int min = 0;

    srand(time(0));
    int random = (rand() % (counter-min + 1)) + min;
    int virtual[counter];

    for(i = 0; i<size; i++)
    {
      if(PageTable[i][0] == 1)
      {

        if((PageTable[i][1] == 0) && (PageTable[i][2] == 0))
        {
          virtual[j] = i;
        }
        else if((PageTable[i][1] == 1) && (PageTable[i][2] == 0))
        {
          virtual[j] = i;
        }
        else if((PageTable[i][1] == 0) && (PageTable[i][2] == 1))
        {
          virtual[j] = i;
        }
        else if((PageTable[i][1] == 1) && (PageTable[i][2] == 1))
        {
          virtual[j] = i;
        }
        j++;
      }
    }

    return virtual[random];
  }

  return -1;
}


// Evict a page from RAM to the hard disk
// If its translation is in the TLB, perform a TLB shootdown
// If the page is dirty, write it to hard disk
// Update the Frame Table and the page table
// Pre-condition: the page is currently allocated in the RAM
// Returns (+1: TLB shootdown performed) (+10: hard disk write performed)
int page_evict \
(unsigned int PageTable[][4], int page_table_size, unsigned int TLB[][5], int tlb_size, int FrameTable[], int frame_table_size, int vpn)
{
  int i, calculated = 0;
  int index = select_TLB_shootdown_candidate(TLB, tlb_size);
  int page_evict = select_page_eviction_candidate(PageTable, page_table_size);

  if(TLB_lookup(TLB, tlb_size, page_evict) != -1)
  {
    TLB_shootdown(TLB, tlb_size, PageTable, page_table_size, index);

    calculated += 1;

  } 

  if(PageTable[page_evict][1] == 1)
  {
    calculated += 10;
  }

  FrameTable[page_evict] = 0;
  PageTable[page_evict][0] = 0;

  return calculated;
}


// Copies a page from the hard disk to the RAM 
// Pre-conditions: Page currently not in RAM; page's translation is not in the TLB
// Find a frame for the page; if all the frames are used, performa a page eviction
// Find a TLB entry for the page; if the TLB is full, perform a TLB shootdown
// Returns (+1: TLB shootdown performed) (+10: hard disk write performed) (+100: page eviction performed)
// Parameter read_write: indicates read access or write access
int cache_page_in_RAM \
(unsigned int PageTable[][4], int page_table_size, unsigned int TLB[][4], int tlb_size, unsigned int FrameTable[], int frame_table_size, unsigned int vpn, int read_write)
{
  int i, calculated = 0;
  int evict_result;
  int counter = 0;
  for( i = 0; i < frame_table_size; i++)
  {
    if(FrameTable[i] == 0)
    {
      counter++;
      break;
    }
  }

  if(counter != 0)
  {
    FrameTable[counter] = 1;
  }
  else
  {
    evict_result = page_evict(PageTable, page_table_size, TLB, tlb_size, FrameTable, frame_table_size, vpn);

    int frame = get_available_frame(FrameTable, frame_table_size);

    FrameTable[frame] = 1;

    calculated = 100 + evict_result;
  }

  if(read_write == 1)
  {
    PageTable[vpn][1] = 1;
  }
  else
  {
    PageTable[vpn][1] = 0;
  }

  int frame = get_available_frame(FrameTable, frame_table_size);

  FrameTable[frame] = 1;

  PageTable[vpn][0] = 1;
  PageTable[vpn][2] = 1;
  PageTable[vpn][3] = frame;

  return calculated;
}



//Destroyer Function
// Clears the reference bits of the TLB and the Page Table
void reset_reference_bits \
(unsigned int TLB[][5], int tlb_size, unsigned int PageTable[][4], int page_table_size)
{
  int i;

  //Looping through each reference bit in the TLB and setting it to 0
  for(i = 0; i < tlb_size; i++)
  {
    TLB[i][2] = 0;
  }

  //Looping through each reference bit in the PageTable and setting it to 0
  for(i = 0; i < page_table_size; i++)
  {
    PageTable[i][2] = 0;
  }


}

//OVERALL Function
// Simulates a memory access; updates all the data and statistics
void memory_access (unsigned int TLB[][5], int tlb_size, \
unsigned int PageTable[][4], int page_table_size, \
unsigned int FrameTable[], int frame_table_size, \
unsigned int address, int read_write)
{
  int frame = get_available_frame(FrameTable, frame_table_size);

  int index = TLB_lookup(TLB, tlb_size, address/page_table_size);

  if(index != -1)
  {
    //updating TLB bits
    TLB[index][1] = read_write;
    TLB[index][2] = 1;
    THit++;
  }
  else
  {
    PAccess++;
    TWrites++;
    TMiss++;
    if(PageTable[(address/page_table_size)][0] == 1)
    {
      //updating bits in PTable
      PageTable[address/page_table_size][1] = read_write;
      PageTable[address/page_table_size][2] = 1;
      calculated = cache_translation_in_TLB(TLB, tlb_size, PageTable, page_table_size, address/page_table_size);

      if(calculated == 1)
      {
        shootdown++;
      }

      PHit++;
    }
    else
    {
      DiskR++;
      PWrites++;
      PMiss++;
      if(frame != -1)
    {
      PageTable[address/page_table_size][0] = 1;
      PageTable[address/page_table_size][1] = read_write;
      PageTable[address/page_table_size][2] = 1;
      PageTable[address/page_table_size][3] = frame;
      calculated = cache_translation_in_TLB(TLB, tlb_size, PageTable, page_table_size, address/page_table_size);

      if(calculated == 1)
      {
        shootdown++;
      }
    }
    else
    {
      calculated = cache_page_in_RAM(PageTable,page_table_size, TLB, tlb_size,FrameTable, frame_table_size, address/page_table_size, read_write);

      if(calculated/10 >= 10)
      {
        PageE++;
      }
      if(calculated / 10 == 1 || calculated / 10 == 11)
      {
        DiskW++;
      }
      if(calculated % 2 == 1)
      {
        shootdown++;
      } 
    }
    }
  }
}
