typedef struct kmfreelist{
    struct kmfreelist* next;
} kmfreelist;


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
        // kputs("Getting free block from 4096 list");
        uint8_t* n1 = (uint8_t*)kmalloc_work64();
        uint8_t* n2 = (uint8_t*)((uint64_t)n1 + 32);
        kmfreelist* n;
        n = (kmfreelist*)n2;
        n->next = freelist32;
        freelist32 = n;
        return (void*)n1;
    }
}


void* kmalloc_work16(){
    // if there is a free value in 2048 bin then get it
    if(freelist16 != 0){
        // kputs("2048 free value found!");  
        kmfreelist* n;
        n = freelist16;
        freelist16 = n->next;
        return (void*)n;
    }

    // else get a free block from the 4096 list and split it
    else{
        // kputs("Getting free block from 4096 list");
        uint8_t* n1 = (uint8_t*)kmalloc_work32();
        uint8_t* n2 = (uint8_t*)((uint64_t)n1 + 16);
        kmfreelist* n;
        n = (kmfreelist*)n2;
        n->next = freelist16;
        freelist16 = n;
        return (void*)n1;
    }
}


void* kmalloc_work8(){
    // if there is a free value in 2048 bin then get it
    if(freelist8 != 0){
        // kputs("2048 free value found!");  
        kmfreelist* n;
        n = freelist8;
        freelist8 = n->next;
        return (void*)n;
    }

    // else get a free block from the 4096 list and split it
    else{
        // kputs("Getting free block from 4096 list");
        uint8_t* n1 = (uint8_t*)kmalloc_work16();
        uint8_t* n2 = (uint8_t*)((uint64_t)n1 + 8);
        kmfreelist* n;
        n = (kmfreelist*)n2;
        n->next = freelist8;
        freelist8 = n;
        return (void*)n1;
    }
}


// can allocate
// 8, 16, 32, 64, 128, 256, 512, 1024, 2048, 4096 size blocks
void* kmalloc(uint32_t size){

    uint32_t bin = BIN(size);
    // kprintf("bin = %d\n", bin);

    
    switch(bin){
        case 8:{
            return kmalloc_work8();
            break;
        }
        case 16:{
            return kmalloc_work16();
            break;
        }
        case 32:{
            return kmalloc_work32();
            break;
        }
        case 64:{
            return kmalloc_work64();
            break;
        }
        case 128:{
            return kmalloc_work128();
            break;
        }
        case 256:{
            return kmalloc_work256();
            break;
        }
        case 512:{
            return kmalloc_work512();
            break;
        }
        case 1024:{
            return kmalloc_work1024();
            break;
        }
        case 2048:{
            return kmalloc_work2048();
            break;
        }
        case 4096:{
            break;
        }
        default:{
            panic("kmalloc. invalid bin...");
            break;
        }
    }
}