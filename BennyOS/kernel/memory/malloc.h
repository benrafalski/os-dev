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
INIT_FREELIST(4096);


// If there is a free block in the N-size list, pick the first
// If not, look for a free block in the 2N-size list
// Split the 2N-size block in two N-size blocks and add them to the N-size list
// Now that we have the N-size list populated, pick the first free block from that list



void* kmalloc_work2048(uint32_t bin){
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


// can allocate
// 8, 16, 32, 64, 128, 256, 512, 1024, 2048, 4096 size blocks
void* kmalloc(uint32_t size){

    uint32_t bin = BIN(size);
    // kprintf("bin = %d\n", bin);

    
    switch(bin){
        case 8:{
            break;
        }
        case 16:{
            break;
        }
        case 32:{
            break;
        }
        case 64:{
            // kmalloc_work64(bin);
            break;
        }
        case 128:{
            break;
        }
        case 256:{
            break;
        }
        case 512:{
            break;
        }
        case 1024:{
            break;
        }
        case 2048:{
            return kmalloc_work2048(bin);
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