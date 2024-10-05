typedef struct kmfreelist{
    struct kmfreelist* next;
} kmfreelist;


typedef struct mblock {
    uint64_t size;
    uint64_t reserved;
    void* mptr;
} mblock;


#define RSHIFT_OR(n, b) (n | (n >> b))
#define BIN(n) (RSHIFT_OR(RSHIFT_OR(RSHIFT_OR(RSHIFT_OR(RSHIFT_OR(n-1, 1), 2), 4), 8), 16) + 1)         
#define INIT_FREELIST(size) kmfreelist* freelist ## size = 0; 
#define GET_FREELIST(size) freelist ## size


INIT_FREELIST(8);
INIT_FREELIST(16);
INIT_FREELIST(32);
INIT_FREELIST(64);
INIT_FREELIST(128);
INIT_FREELIST(256);
INIT_FREELIST(512);
INIT_FREELIST(1024);
INIT_FREELIST(2048);
// INIT_FREELIST(4096);


// If there is a free block in the N-size list, pick the first
// If not, look for a free block in the 2N-size list
// Split the 2N-size block in two N-size blocks and add them to the N-size list
// Now that we have the N-size list populated, pick the first free block from that list


void* kmalloc_work2048(){
    // if there is a free value in 2048 bin then get it
    if(freelist2048 != 0){
        // kputs("2048 free value found!");  
        kmfreelist* n;
        n = freelist2048;
        freelist2048 = n->next;
        return (void*)n;
    }

    // else get a free block from the 4096 list and split it
    else{
        // kputs("Getting free block from 4096 list");
        uint8_t* n1 = (uint8_t*)palloc();
        uint8_t* n2 = (uint8_t*)((uint64_t)n1 + 2048);
        kmfreelist* n;
        n = (kmfreelist*)n2;
        n->next = freelist2048;
        freelist2048 = n;
        return (void*)n1;
    }
}

void* kmalloc_work1024(){
    // if there is a free value in 2048 bin then get it
    if(freelist1024 != 0){
        // kputs("2048 free value found!");  
        kmfreelist* n;
        n = freelist1024;
        freelist1024 = n->next;
        return (void*)n;
    }

    // else get a free block from the 4096 list and split it
    else{
        // kputs("Getting free block from 4096 list");
        uint8_t* n1 = (uint8_t*)kmalloc_work2048();
        uint8_t* n2 = (uint8_t*)((uint64_t)n1 + 1024);
        kmfreelist* n;
        n = (kmfreelist*)n2;
        n->next = freelist1024;
        freelist1024 = n;
        return (void*)n1;
    }
}

void* kmalloc_work512(){
    // if there is a free value in 2048 bin then get it
    if(freelist512 != 0){
        // kputs("2048 free value found!");  
        kmfreelist* n;
        n = freelist512;
        freelist512 = n->next;
        return (void*)n;
    }

    // else get a free block from the 4096 list and split it
    else{
        // kputs("Getting free block from 4096 list");
        uint8_t* n1 = (uint8_t*)kmalloc_work1024();
        uint8_t* n2 = (uint8_t*)((uint64_t)n1 + 512);
        kmfreelist* n;
        n = (kmfreelist*)n2;
        n->next = freelist512;
        freelist512 = n;
        return (void*)n1;
    }
}

void* kmalloc_work256(){
    // if there is a free value in 2048 bin then get it
    if(freelist256 != 0){
        // kputs("2048 free value found!");  
        kmfreelist* n;
        n = freelist256;
        freelist256 = n->next;
        return (void*)n;
    }

    // else get a free block from the 4096 list and split it
    else{
        // kputs("Getting free block from 4096 list");
        uint8_t* n1 = (uint8_t*)kmalloc_work512();
        uint8_t* n2 = (uint8_t*)((uint64_t)n1 + 256);
        kmfreelist* n;
        n = (kmfreelist*)n2;
        n->next = freelist256;
        freelist256 = n;
        return (void*)n1;
    }
}

void* kmalloc_work128(){
    // if there is a free value in 2048 bin then get it
    if(freelist128 != 0){
        // kputs("2048 free value found!");  
        kmfreelist* n;
        n = freelist128;
        freelist128 = n->next;
        return (void*)n;
    }

    // else get a free block from the 4096 list and split it
    else{
        // kputs("Getting free block from 4096 list");
        uint8_t* n1 = (uint8_t*)kmalloc_work256();
        uint8_t* n2 = (uint8_t*)((uint64_t)n1 + 128);
        kmfreelist* n;
        n = (kmfreelist*)n2;
        n->next = freelist128;
        freelist128 = n;
        return (void*)n1;
    }
}

void* kmalloc_work64(){
    // if there is a free value in 2048 bin then get it
    if(freelist64 != 0){
        // kputs("2048 free value found!");  
        kmfreelist* n;
        n = freelist64;
        freelist64 = n->next;
        return (void*)n;
    }

    // else get a free block from the 4096 list and split it
    else{
        // kputs("Getting free block from 4096 list");
        uint8_t* n1 = (uint8_t*)kmalloc_work256();
        uint8_t* n2 = (uint8_t*)((uint64_t)n1 + 64);
        kmfreelist* n;
        n = (kmfreelist*)n2;
        n->next = freelist64;
        freelist64 = n;
        return (void*)n1;
    }
}

void* kmalloc_work32(){
    // if there is a free value in 2048 bin then get it
    if(freelist32 != 0){
        // kputs("2048 free value found!");  
        kmfreelist* n;
        n = freelist32;
        freelist32 = n->next;
        return (void*)n;
    }

    // else get a free block from the 4096 list and split it
    else{
        // kputs("Getting free block from 64 list");
        uint8_t* n1 = (uint8_t*)kmalloc_work64();
        uint8_t* n2 = (uint8_t*)((uint64_t)n1 + 32);
        kmfreelist* n;
        n = (kmfreelist*)n2;
        n->next = freelist32;
        freelist32 = n;
        return (void*)n1;
    }
}

// can allocate
// 32, 64, 128, 256, 512, 1024, 2048, 4096 size blocks
// lowest is 32 because there is 16 bytes of metadata for each alloc
void* kmalloc(uint32_t size){

    uint32_t bin = BIN(size+16);

    
    switch(bin){
        case 32:{
            mblock* block = (mblock*)kmalloc_work32();
            block->size = 32;
            return (void*)&block->mptr;
            break;
        }
        case 64:{
            mblock* block = (mblock*)kmalloc_work64();
            block->size = 64;
            return (void*)&block->mptr;
            break;
        }
        case 128:{
            mblock* block = (mblock*)kmalloc_work128();
            block->size = 128;
            return (void*)&block->mptr;
            break;
        }
        case 256:{
            mblock* block = (mblock*)kmalloc_work256();
            block->size = 256;
            return (void*)&block->mptr;
            break;
        }
        case 512:{
            mblock* block = (mblock*)kmalloc_work512();
            block->size = 512;
            return (void*)&block->mptr;
            break;
        }
        case 1024:{
            mblock* block = (mblock*)kmalloc_work1024();
            block->size = 1024;
            return (void*)&block->mptr;
            break;
        }
        case 2048:{
            mblock* block = (mblock*)kmalloc_work2048();
            block->size = 2048;
            return (void*)&block->mptr;
            break;
        }
        case 4096:{
            kputs("4096");
            break;
        }
        default:{
            panic("kmalloc. invalid bin...");
            break;
        }
    }
}


void kfree(void* ptr){
    mblock* block = ptr - 0x10;
    kmfreelist* n;
    n = (kmfreelist*)block;

    switch(block->size){
        case 32:{
            n->next = freelist32;
            freelist32 = n;
            break;
        }
        case 64:{
            n->next = freelist64;
            freelist64 = n;
            break;
        }
        case 128:{
            n->next = freelist128;
            freelist128 = n;
            break;
        }
        case 256:{
            n->next = freelist256;
            freelist256 = n;
            break;
        }
        case 512:{
            n->next = freelist512;
            freelist512 = n;
            break;
        }
        case 1024:{
            n->next = freelist1024;
            freelist1024 = n;
            break;
        }
        case 2048:{
            n->next = freelist2048;
            freelist2048 = n;
            break;
        }
        case 4096:{
            panic("kfree. invalid size (4096)...");
            break;
        }
        default:{
            panic("kfree. invalid size...");
            break;
        }
    }

}