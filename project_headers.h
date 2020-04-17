//******************************************************************************
//***********************  TLB  Functions  *************************************
//******************************************************************************

// Returns the TLB entry that corresponds to a virtual page
// Returns -1 if the page's translation is not in the TLB
int TLB_lookup(unsigned int TLB[][5], int size, unsigned int vpn);

// Returns the index of the first available TLB entry
// Returns -1 if all TLB entries are used
int get_available_TLB_entry (unsigned int TLB[][5], int size);

// Selects the TLB entry that will be evicted
// Pre-condition: All TLB entries are full
// Criteria: Select a random entry with ref.bit=0; if all ref.bit=1, select a random entry
unsigned int select_TLB_shootdown_candidate(unsigned int TLB[][5], int size);

// Perform a TLB shootdown (set V=0, copy D,R bits to the page table)
// Pre-condition: shootdown entry is currently valid
// Parameter index is the TLB entry to shoot down
void TLB_shootdown \
(unsigned int TLB[][5], int tlb_size, unsigned int PageTable[][4], int page_table_size, int index);

// Copies a translation from the Page Table to the TLB
// The first available TLB entry is used; otherwise, a TLB shootdown is performed
// It copies the D,R bits and the frame number from the page table
// Parameter: virtual page number
// Returns: (+1: shootdown performed)
int cache_translation_in_TLB \
(unsigned int TLB[][5], int tlb_size, unsigned int PageTable[][4], int page_table_size, unsigned int vpn);


//******************************************************************************
//***********************  Page Table Functions  *******************************
//******************************************************************************


// Returns the index of the first available frame
// Returns -1 if all frames are allocated
int get_available_frame (unsigned int FrameTable[], int size);

// Search the PageTable for VDR values passed as parameters
// Return -1 if not found; otherwise, return the index of one such
// randomized entry (using rand function)
// Pre-condition: VDR are 0 or 1
int search_PageTable_by_VDR(unsigned int PageTable[][4], int size, int V, int D, int R);

// Selects the virtual page that will be replaced
// Pre-condition: All the frames are allocated
// Criteria: Valid must be 1; choose in order as below
//     VDR.bits: 100   110   101   111
// Between pages with the same category, randomize (using rand)
unsigned int select_page_eviction_candidate(unsigned int PageTable[][4], int size);

// Evict a page from RAM to the hard disk
// If its translation is in the TLB, perform a TLB shootdown
// If the page is dirty, write it to hard disk
// Update the Frame Table and the page table
// Pre-condition: the page is currently allocated in the RAM
// Returns (+1: TLB shootdown performed) (+10: hard disk write performed)
int page_evict \
(unsigned int PageTable[][4], int page_table_size, unsigned int TLB[][5], int tlb_size, int FrameTable[], int frame_table_size, int vpn);

// Copies a page from the hard disk to the RAM 
// Pre-conditions: Page currently not in RAM; page's translation is not in the TLB
// Find a frame for the page; if all the frames are used, performa a page eviction
// Find a TLB entry for the page; if the TLB is full, perform a TLB shootdown
// Returns (+1: TLB shootdown performed) (+10: hard disk write performed) (+100: page eviction performed)
// Parameter read_write: indicates read access or write access
int cache_page_in_RAM \
(unsigned int PageTable[][4], int page_table_size, unsigned int TLB[][4], int tlb_size, unsigned int FrameTable[], int frame_table_size, unsigned int vpn, int read_write);




//******************************************************************************
// Clears the reference bits of the TLB and the Page Table
void reset_reference_bits \
(unsigned int TLB[][5], int tlb_size, unsigned int PageTable[][4], int page_table_size);




//******************************************************************************
//***********************  Overall  Function  **********************************
//******************************************************************************

// Simulates a memory access; updates all the data and statistics
void memory_access (unsigned int TLB[][5], int tlb_size, \
unsigned int PageTable[][4], int page_table_size, \
unsigned int FrameTable[], int frame_table_size, \
unsigned int address, int read_write);










//
