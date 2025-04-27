



// typedef struct kmfreelist{
//     struct kmfreelist* next;
// } kmfreelist;


// typedef struct mblock {
//     uint64_t size;
//     uint64_t reserved;
//     void* mptr;
// } mblock;


// #define RSHIFT_OR(n, b) (n | (n >> b))
// #define BIN(n) (RSHIFT_OR(RSHIFT_OR(RSHIFT_OR(RSHIFT_OR(RSHIFT_OR(n-1, 1), 2), 4), 8), 16) + 1)         
// #define INIT_FREELIST(size) kmfreelist* freelist ## size = 0; 
// #define GET_FREELIST(size) freelist ## size


// INIT_FREELIST(8);
// INIT_FREELIST(16);
// INIT_FREELIST(32);
// INIT_FREELIST(64);
// INIT_FREELIST(128);
// INIT_FREELIST(256);
// INIT_FREELIST(512);
// INIT_FREELIST(1024);
// INIT_FREELIST(2048);
// // INIT_FREELIST(4096);


// // If there is a free block in the N-size list, pick the first
// // If not, look for a free block in the 2N-size list
// // Split the 2N-size block in two N-size blocks and add them to the N-size list
// // Now that we have the N-size list populated, pick the first free block from that list


// void* kmalloc_work2048(){
//     // if there is a free value in 2048 bin then get it
//     if(freelist2048 != 0){
//         // kputs("2048 free value found!");  
//         kmfreelist* n;
//         n = freelist2048;
//         freelist2048 = n->next;
//         return (void*)n;
//     }

//     // else get a free block from the 4096 list and split it
//     else{
//         // kputs("Getting free block from 4096 list");
//         uint8_t* n1 = (uint8_t*)palloc();
//         uint8_t* n2 = (uint8_t*)((uint64_t)n1 + 2048);
//         kmfreelist* n;
//         n = (kmfreelist*)n2;
//         n->next = freelist2048;
//         freelist2048 = n;
//         return (void*)n1;
//     }
// }

// void* kmalloc_work1024(){
//     // if there is a free value in 2048 bin then get it
//     if(freelist1024 != 0){
//         // kputs("2048 free value found!");  
//         kmfreelist* n;
//         n = freelist1024;
//         freelist1024 = n->next;
//         return (void*)n;
//     }

//     // else get a free block from the 4096 list and split it
//     else{
//         // kputs("Getting free block from 4096 list");
//         uint8_t* n1 = (uint8_t*)kmalloc_work2048();
//         uint8_t* n2 = (uint8_t*)((uint64_t)n1 + 1024);
//         kmfreelist* n;
//         n = (kmfreelist*)n2;
//         n->next = freelist1024;
//         freelist1024 = n;
//         return (void*)n1;
//     }
// }

// void* kmalloc_work512(){
//     // if there is a free value in 2048 bin then get it
//     if(freelist512 != 0){
//         // kputs("2048 free value found!");  
//         kmfreelist* n;
//         n = freelist512;
//         freelist512 = n->next;
//         return (void*)n;
//     }

//     // else get a free block from the 4096 list and split it
//     else{
//         // kputs("Getting free block from 4096 list");
//         uint8_t* n1 = (uint8_t*)kmalloc_work1024();
//         uint8_t* n2 = (uint8_t*)((uint64_t)n1 + 512);
//         kmfreelist* n;
//         n = (kmfreelist*)n2;
//         n->next = freelist512;
//         freelist512 = n;
//         return (void*)n1;
//     }
// }

// void* kmalloc_work256(){
//     // if there is a free value in 2048 bin then get it
//     if(freelist256 != 0){
//         // kputs("2048 free value found!");  
//         kmfreelist* n;
//         n = freelist256;
//         freelist256 = n->next;
//         return (void*)n;
//     }

//     // else get a free block from the 4096 list and split it
//     else{
//         // kputs("Getting free block from 4096 list");
//         uint8_t* n1 = (uint8_t*)kmalloc_work512();
//         uint8_t* n2 = (uint8_t*)((uint64_t)n1 + 256);
//         kmfreelist* n;
//         n = (kmfreelist*)n2;
//         n->next = freelist256;
//         freelist256 = n;
//         return (void*)n1;
//     }
// }

// void* kmalloc_work128(){
//     // if there is a free value in 2048 bin then get it
//     if(freelist128 != 0){
//         // kputs("2048 free value found!");  
//         kmfreelist* n;
//         n = freelist128;
//         freelist128 = n->next;
//         return (void*)n;
//     }

//     // else get a free block from the 4096 list and split it
//     else{
//         // kputs("Getting free block from 4096 list");
//         uint8_t* n1 = (uint8_t*)kmalloc_work256();
//         uint8_t* n2 = (uint8_t*)((uint64_t)n1 + 128);
//         kmfreelist* n;
//         n = (kmfreelist*)n2;
//         n->next = freelist128;
//         freelist128 = n;
//         return (void*)n1;
//     }
// }

// void* kmalloc_work64(){
//     // if there is a free value in 2048 bin then get it
//     if(freelist64 != 0){
//         // kputs("2048 free value found!");  
//         kmfreelist* n;
//         n = freelist64;
//         freelist64 = n->next;
//         return (void*)n;
//     }

//     // else get a free block from the 4096 list and split it
//     else{
//         // kputs("Getting free block from 4096 list");
//         uint8_t* n1 = (uint8_t*)kmalloc_work256();
//         uint8_t* n2 = (uint8_t*)((uint64_t)n1 + 64);
//         kmfreelist* n;
//         n = (kmfreelist*)n2;
//         n->next = freelist64;
//         freelist64 = n;
//         return (void*)n1;
//     }
// }

// void* kmalloc_work32(){
//     // if there is a free value in 2048 bin then get it
//     if(freelist32 != 0){
//         // kputs("2048 free value found!");  
//         kmfreelist* n;
//         n = freelist32;
//         freelist32 = n->next;
//         return (void*)n;
//     }

//     // else get a free block from the 4096 list and split it
//     else{
//         // kputs("Getting free block from 64 list");
//         uint8_t* n1 = (uint8_t*)kmalloc_work64();
//         uint8_t* n2 = (uint8_t*)((uint64_t)n1 + 32);
//         kmfreelist* n;
//         n = (kmfreelist*)n2;
//         n->next = freelist32;
//         freelist32 = n;
//         return (void*)n1;
//     }
// }

// // can allocate
// // 32, 64, 128, 256, 512, 1024, 2048, 4096 size blocks
// // lowest is 32 because there is 16 bytes of metadata for each alloc
// void* kmalloc(uint32_t size){

//     uint32_t bin = BIN(size+16);

//     if(bin <= 4096){
//         switch(bin){
//             case 32:{
//                 mblock* block = (mblock*)kmalloc_work32();
//                 block->size = 32;
//                 return (void*)&block->mptr;
//                 break;
//             }
//             case 64:{
//                 mblock* block = (mblock*)kmalloc_work64();
//                 block->size = 64;
//                 return (void*)&block->mptr;
//                 break;
//             }
//             case 128:{
//                 mblock* block = (mblock*)kmalloc_work128();
//                 block->size = 128;
//                 return (void*)&block->mptr;
//                 break;
//             }
//             case 256:{
//                 mblock* block = (mblock*)kmalloc_work256();
//                 block->size = 256;
//                 return (void*)&block->mptr;
//                 break;
//             }
//             case 512:{
//                 mblock* block = (mblock*)kmalloc_work512();
//                 block->size = 512;
//                 return (void*)&block->mptr;
//                 break;
//             }
//             case 1024:{
//                 mblock* block = (mblock*)kmalloc_work1024();
//                 block->size = 1024;
//                 return (void*)&block->mptr;
//                 break;
//             }
//             case 2048:{
//                 mblock* block = (mblock*)kmalloc_work2048();
//                 block->size = 2048;
//                 return (void*)&block->mptr;
//                 break;
//             }
//             case 4096:{
//                 return palloc();
//                 // panic("in kmalloc: got bin size 4096");
//                 break;
//             }
//             default:{
//                 kprintf("bin %d\n", bin);
//                 panic("kmalloc. invalid bin...");
//                 break;
//             }
//         }
//     }else{
//         void* ptr;
//         while(bin > 0){
//             ptr = palloc();
//             bin -= 4096;
//         }
//         return ptr;
//     }

    
    
// }


// void kfree(void* ptr){
//     mblock* block = ptr - 0x10;
//     kmfreelist* n;
//     n = (kmfreelist*)block;

//     switch(block->size){
//         case 32:{
//             n->next = freelist32;
//             freelist32 = n;
//             break;
//         }
//         case 64:{
//             n->next = freelist64;
//             freelist64 = n;
//             break;
//         }
//         case 128:{
//             n->next = freelist128;
//             freelist128 = n;
//             break;
//         }
//         case 256:{
//             n->next = freelist256;
//             freelist256 = n;
//             break;
//         }
//         case 512:{
//             n->next = freelist512;
//             freelist512 = n;
//             break;
//         }
//         case 1024:{
//             n->next = freelist1024;
//             freelist1024 = n;
//             break;
//         }
//         case 2048:{
//             n->next = freelist2048;
//             freelist2048 = n;
//             break;
//         }
//         case 4096:{
//             // panic("kfree. invalid size (4096)...");
//             pfree((char*)n);
//             break;
//         }
//         default:{
//             panic("kfree. invalid size...");
//             break;
//         }
//     }

// }


typedef struct kmalloc_block {
    size_t size;          // Size of the allocated block (metadata)
    struct kmalloc_block* next;  // Pointer to the next block in the freelist (for buddy system)
    // The actual memory begins right after this struct
} kmalloc_block;


#define MAX_BINS 7  // For sizes from 32 bytes up to 4096 bytes (2^5 to 2^16)

typedef struct kmfreelist {
    struct kmfreelist* next;
} kmfreelist;

kmalloc_block* freelists[MAX_BINS] = {0};  // Bin freelist table
// 0=32, 1=64, 2=128, 3=256, 4=512, 5=1024, 6=2048, 7=4096


static inline int size_to_bin_index(size_t size) {
    int index = 0;
    while ((1 << index) < size) {
        index++;
    }
    return index - 5;
}


void* kmalloc_buddy(size_t size) {
    if (size == 0) {
        return NULL;
    }

    // Align the size to the nearest power of two
    int bin_idx = size_to_bin_index(size);
    if (bin_idx >= MAX_BINS) {
        // panic("kmalloc_buddy: size too large");  // Size exceeds the maximum bin size
        // return NULL;  // Size too large

        void* ptr;
        while(size > 0){
            ptr = palloc();
            size -= 4096;
        }
        return ptr;
    }

    // Check if there's an available block in the bin
    if (freelists[bin_idx] != NULL) {
        kmalloc_block* block = (kmalloc_block*)freelists[bin_idx];
        freelists[bin_idx] = block->next;  // Remove block from freelist

        // Set the block size in the metadata
        block->size = (1 << (bin_idx + 5));

        // Return the user data pointer (skip the metadata struct)
        return (void*)(block + 1);  // Skip the metadata, return the actual memory
    }

    // asm("cli;hlt");

    // No block in this bin, attempt to split larger blocks
    for (int i = bin_idx + 1; i < MAX_BINS; i++) {
        if (freelists[i] != NULL) {
            kmalloc_block* block = freelists[i];
            // kprintf("%p\n", block);
            if ((uintptr_t)block < 0xFFFFFFFFF0000000 || (uintptr_t)block > 0xFFFFFFFFFFFFFFFF) {
                kputs("GPF: Invalid freelist pointer!\n");
                while (1);  // Halt the system for debugging
            }
            freelists[i] = block->next;  // Remove block from freelist

            // Split the block into two halves and push the second half into the next smaller bin
            size_t block_size = 1 << (i + 5);  // Size of the block at this bin (2^i)
            size_t half_size = block_size / 2;

            // Set the size in the metadata for the first block
            block->size = half_size;

            // Return the first half of the block
            kmalloc_block* buddy = (kmalloc_block*)((uint8_t*)block + half_size);
            buddy->next = freelists[i - 1];
            freelists[i - 1] = buddy;

            // Return the user data pointer (skip the metadata struct)
            return (void*)(block + 1);  // Skip the metadata, return the actual memory
        }
    }

    // If no larger block is available, allocate a new block (e.g., 4096 bytes)
    void* larger_block = palloc();
    if (larger_block == NULL) {
        return NULL;  // Out of memory
    }

    // Split the large block into smaller blocks and add them to the freelist
    for (int i = MAX_BINS - 1; i >= bin_idx; i--) {
        kmalloc_block* buddy = (kmalloc_block*)((uint8_t*)larger_block + (1 << i));
        buddy->next = freelists[i];
        freelists[i] = buddy;
        larger_block = (void*)((uint8_t*)larger_block + (1 << (i - 1)));  // Move to the next block
    }

    // Set the size in the metadata of the first block
    kmalloc_block* block = (kmalloc_block*)larger_block;
    block->size = (1 << (bin_idx + 5));  // Store size in metadata

    // Return the user data pointer (skip the metadata struct)
    return (void*)(block + 1);  // Skip the metadata, return the actual memory
}

void kfree_buddy(void* ptr) {
    if (ptr == NULL) {
        return;
    }

    // Retrieve the kmalloc_block struct just before the user data
    kmalloc_block* block = (kmalloc_block*)ptr - 1;  // Move to the block header
    size_t block_size = block->size;  // Get the block size from the metadata

    // Find the bin index based on the block size
    int bin_idx = size_to_bin_index(block_size);
    if (bin_idx >= MAX_BINS) {
        return;  // Invalid size for deallocation
    }

    // Add the block back to the freelist
    block->next = freelists[bin_idx];
    freelists[bin_idx] = block;

    // Optionally: Merge buddies if you want a more advanced buddy allocator
}


void kfree(void* ptr){
    kfree_buddy(ptr);
}

void* kmalloc(uint32_t size){
    kmalloc_buddy(size);
}