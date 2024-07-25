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
        
        uint64_t* n1 = (uint64_t*)n;
        *n1 = 2048;
        return (void*)(((uint8_t*)n1) + 16);
    }

    // else get a free block from the 4096 list and split it
    else{
        kputs("Getting free block from 4096 list");
        // gets ptr to a 4096 size block
        // n1 will be the block that is returned
        uint8_t* n1 = (uint8_t*)palloc();
        // n2 will be added to the 2048 free list
        uint8_t* n2 = (uint8_t*)((uint64_t)n1 + 2048);


        kmfreelist* n;
        n = (kmfreelist*)n2;
        n->next = freelist2048;
        freelist2048 = n;
        
        *n1 = 2048;
        return (void*)(((uint8_t*)n1) + 16);
    }
}

void* kmalloc_work1024(){
    // if there is a free value in 2048 bin then get it
    if(freelist1024 != 0){
        // kputs("2048 free value found!");  
        kmfreelist* n;
        n = freelist1024;
        freelist1024 = n->next;
        
        uint64_t* n1 = (uint64_t*)n;
        *n1 = 1024;
        return (void*)(((uint8_t*)n1) + 16);
    }

    // else get a free block from the 4096 list and split it
    else{
        kputs("Getting free block from 2048 list");
        uint32_t* n1 = (uint32_t*)kmalloc_work2048();
        uint8_t* n2 = (uint8_t*)((uint64_t)n1 + 1024);
        kmfreelist* n;
        n = (kmfreelist*)n2;
        n->next = freelist1024;
        freelist1024 = n;
        
        *n1 = 1024;
        return (void*)(((uint8_t*)n1) + 16);
    }
}

void* kmalloc_work512(){
    // if there is a free value in 2048 bin then get it
    if(freelist512 != 0){
        // kputs("2048 free value found!");  
        kmfreelist* n;
        n = freelist512;
        freelist512 = n->next;
        
        uint32_t* n1 = (uint32_t*)n;
        *n1 = 512;
        return (void*)(((uint8_t*)n1) + 16);
    }

    // else get a free block from the 4096 list and split it
    else{
        // kputs("Getting free block from 1024 list");
        uint32_t* n1 = (uint32_t*)kmalloc_work1024();
        uint8_t* n2 = (uint8_t*)((uint64_t)n1 + 512);
        kmfreelist* n;
        n = (kmfreelist*)n2;
        n->next = freelist512;
        freelist512 = n;
        
        *n1 = 512;
        return (void*)(((uint8_t*)n1) + 16);
    }
}

void* kmalloc_work256(){
    // if there is a free value in 2048 bin then get it
    if(freelist256 != 0){
        // kputs("2048 free value found!");  
        kmfreelist* n;
        n = freelist256;
        freelist256 = n->next;
        
        uint32_t* n1 = (uint32_t*)n;
        *n1 = 256;
        return (void*)(((uint8_t*)n1) + 16);
    }

    // else get a free block from the 4096 list and split it
    else{
        // kputs("Getting free block from 512 list");
        uint32_t* n1 = (uint32_t*)kmalloc_work512();
        uint8_t* n2 = (uint8_t*)((uint64_t)n1 + 256);
        kmfreelist* n;
        n = (kmfreelist*)n2;
        n->next = freelist256;
        freelist256 = n;
        
        *n1 = 256;
        return (void*)(((uint8_t*)n1) + 16);
    }
}

void* kmalloc_work128(){
    // if there is a free value in 2048 bin then get it
    if(freelist128 != 0){
        // kputs("2048 free value found!");  
        kmfreelist* n;
        n = freelist128;
        freelist128 = n->next;
        
        uint32_t* n1 = (uint32_t*)n;
        *n1 = 128;
        return (void*)(((uint8_t*)n1) + 16);
    }

    // else get a free block from the 4096 list and split it
    else{
        // kputs("Getting free block from 256 list");
        uint32_t* n1 = (uint32_t*)kmalloc_work256();
        uint8_t* n2 = (uint8_t*)((uint64_t)n1 + 128);
        kmfreelist* n;
        n = (kmfreelist*)n2;
        n->next = freelist128;
        freelist128 = n;
        
        *n1 = 128;
        return (void*)(((uint8_t*)n1) + 16);
    }
}

void* kmalloc_work64(){
    // if there is a free value in 2048 bin then get it
    if(freelist64 != 0){
        // kputs("2048 free value found!");  
        kmfreelist* n;
        n = freelist64;
        freelist64 = n->next;
        
        uint32_t* n1 = (uint32_t*)n;
        *n1 = 64;
        return (void*)(((uint8_t*)n1) + 16);
    }

    // else get a free block from the 4096 list and split it
    else{
        // kputs("Getting free block from 256 list");
        uint32_t* n1 = (uint32_t*)kmalloc_work256();
        uint8_t* n2 = (uint8_t*)((uint64_t)n1 + 64);
        kmfreelist* n;
        n = (kmfreelist*)n2;
        n->next = freelist64;
        freelist64 = n;
        
        *n1 = 64;
        return (void*)(((uint8_t*)n1) + 16);
    }
}

void* kmalloc_work32(){
    // if there is a free value in 2048 bin then get it
    if(freelist32 != 0){
        // kputs("2048 free value found!");  
        kmfreelist* n;
        n = freelist32;
        freelist32 = n->next;

        uint32_t* n1 = (uint32_t*)n;
        *n1 = 32;
        return (void*)(((uint8_t*)n1) + 16);
    }

    // else get a free block from the 4096 list and split it
    else{
        // kputs("Getting free block from 64 list");
        uint32_t* n1 = (uint32_t*)kmalloc_work64();
        uint8_t* n2 = (uint8_t*)((uint64_t)n1 + 32);
        kmfreelist* n;
        n = (kmfreelist*)n2;
        n->next = freelist32;
        freelist32 = n;

        *n1 = 32;
        return (void*)(((uint8_t*)n1) + 16);
        // return (void*)n1;
    }
}


// void* kmalloc_work16(){
//     // if there is a free value in 2048 bin then get it
//     if(freelist16 != 0){
//         // kputs("2048 free value found!");  
//         kmfreelist* n;
//         n = freelist16;
//         freelist16 = n->next;
//         return (void*)n;
//     }

//     // else get a free block from the 4096 list and split it
//     else{
//         // kputs("Getting free block from 4096 list");
//         uint8_t* n1 = (uint8_t*)kmalloc_work32();
//         uint8_t* n2 = (uint8_t*)((uint64_t)n1 + 16);
//         kmfreelist* n;
//         n = (kmfreelist*)n2;
//         n->next = freelist16;
//         freelist16 = n;
//         return (void*)n1;
//     }
// }


// void* kmalloc_work8(){
//     // if there is a free value in 2048 bin then get it
//     if(freelist8 != 0){
//         // kputs("2048 free value found!");  
//         kmfreelist* n;
//         n = freelist8;
//         freelist8 = n->next;
//         return (void*)n;
//     }

//     // else get a free block from the 4096 list and split it
//     else{
//         // kputs("Getting free block from 4096 list");
//         uint8_t* n1 = (uint8_t*)kmalloc_work16();
//         uint8_t* n2 = (uint8_t*)((uint64_t)n1 + 8);
//         kmfreelist* n;
//         n = (kmfreelist*)n2;
//         n->next = freelist8;
//         freelist8 = n;
//         return (void*)n1;
//     }
// }


// can allocate
// 8, 16, 32, 64, 128, 256, 512, 1024, 2048, 4096 size blocks
void* kmalloc(uint32_t size){

    uint32_t bin = BIN(size+16);
    // kprintf("bin = %d\n", bin);

    
    switch(bin){
        // case 8:{
        //     return kmalloc_work8();
        //     break;
        // }
        // case 16:{
        //     return kmalloc_work16();
        //     break;
        // }
        case 32:{
            // kputs("here");
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
    uint32_t* realptr = ptr - 0x10;
    kmfreelist* n;
    n = (kmfreelist*)realptr;

    switch(*realptr){
        case 8:{
            n->next = freelist8;
            freelist8 = n;
            break;
        }
        case 16:{
            n->next = freelist16;
            freelist16 = n;
            break;
        }
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
            kputs("4096");
            break;
        }
        default:{
            panic("kfree. invalid size...");
            break;
        }
    }

}