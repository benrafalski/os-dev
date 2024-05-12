
#define HEAP_BASE_VIRT 0xFFFFC90000000000
#define HEAP_BASE_PHYS 0x300000

#define FREE 0x1000
#define USED 0x2000


// 1. allocate a page

typedef struct {
    uint64_t* ptr;
    buddy_allocator_t* next;
} buddy_allocator_t;


buddy_allocator_t* head;

head->ptr = 0;
head->next = 0;

uint8_t get_max_phys_addr(void)
{
    int eax, unused;
    __cpuid(0x80000008, eax, unused, unused, unused);
    return eax & (0xff);                // 40
}

uint8_t get_max_virt_addr(void)
{
    int eax, unused;
    __cpuid(0x80000008, eax, unused, unused, unused);
    return (eax & (0xff00)) >> 8;       // 48
}

bool is_valid_canonical(uint64_t addr){
    if(addr >= 0xffff800000000000 && addr <= 0xffffffffffffffff){
        return true;
    }
    if(addr >= 0x0000000000000000 && addr <= 0x00007fffffffffff){
        return true;
    }
    return false;
}


// maps 0x1000 (4096 bytes) to RAM
void map_page(uint64_t paddr, uint64_t vaddr){
    paddr &= ~(0xFFF);
    vaddr &= ~(0xFFF);

    uint64_t *pml4 = (uint64_t*)0x2000;
    uint64_t *pdpt = (uint64_t*)0x100000;
    uint64_t *pd = (uint64_t*)0x101000;
    uint64_t *pt = (uint64_t*)0x102000;
    uint64_t *page = (uint64_t*)paddr;

    uint64_t pml4_i = (vaddr >> 39) & 0x1ff;
    uint64_t pdpt_i = (vaddr >> 30) & 0x1ff;
    uint64_t pd_i = (vaddr >> 21) & 0x1ff;
    uint64_t pt_i = (vaddr >> 12) & 0x1ff;

    pml4[pml4_i] = (uint64_t)0x100003;
    pdpt[pdpt_i] = (uint64_t)0x101003;
    pd[pd_i] = (uint64_t)0x102003;
    pt[pt_i] = (uint64_t)(paddr | 3);
}

// maps 0x200000 (2,097,152 bytes) to RAM
void map_block(uint64_t pstart, uint64_t vstart){
    for(int i = 0; i < 512; i++){
        map_page(pstart, vstart);
        pstart += 0x1000;
        vstart += 0x1000;
    }
}

void* kmalloc(uint64_t size){

    uint64_t size_real;

    if(size <= 8){
        // malloc 8
        size_real = 8;
    }else if(size <= 16 && size > 8){
        // mallic 16
        size_real = 16;
    }else if(size <= 32 && size > 16){
        // malloc 32
        size_real = 32;
    }else if(size <= 64 && size > 32){
        // malloc 64
        size_real = 64;
    }else if(size <= 128 && size > 64){
        // malloc 128
        size_real = 128;
    }else if(size <= 256 && size > 128){
        // malloc 256
        size_real = 256;
    }else if(size <= 512 && size > 256){
        // malloc 512
        size_real = 512;
    }else if(size <= 1024 && size > 512){
        // malloc 1024
        size_real = 1024;
    }else if(size <= 2048 && size > 1024){
        // malloc 2048
        size_real = 2048;
    }else if(size <= 4096 && size > 2048){
        // malloc 4096
        size_real = 4096;
    }

    if(!head->ptr){
        kputs("here");
    }
    else{
        kputs("there");
    }

}

void kfree(void* ptr){

}


// int 0x12 ; ax = 0x27f = 639 KB = 639,000 bytes

// ----------------------------------------------------------------------------------------------------------

// usable memory
// LOW MEMORY (< 1MB)
// 0x00000500	0x00007BFF	29.75 KiB	Conventional memory	usable memory
// 0x00007C00	0x00007DFF	512 bytes	Your OS BootSector
// 0x00007E00	0x0007FFFF  480.5 KiB	Conventional memory = 0x0007FFFF or 491,519 bytes

// Extended Memory
// 0x00100000	0x00EFFFFF	0x00E00000 (14 MiB)	RAM -- free for use (if it exists)
// 0x01000000	 ????????	 ???????? (whatever exists)	RAM -- free for use
// 0x0000000100000000 (possible memory above 4 GiB)	 ????????????????	 ???????????????? (whatever exists)	RAM -- free for use (PAE/64bit)

// ----------------------------------------------------------------------------------------------------------

// Memory Map
// Page tables
// PML4 = 0x2000
// PML4[0] = PDPT = 0x3000
// PDPT[0] = PDT = 0x4000
// PDT[0] -> PT = 0x5000

// stack
// 0x30000-0x40000

// Identiy Map
// 0x0-0x1ffff8 is identity mapped at 0x5000

// kernel heap
// currently 0xFFFFC90000000000 - 0xFFFFC90000200000 is the heap virtual space
// currently 0x300000 - 0x500000 is the heap physical space


// Kernel = 0x8000 - 0x28000 (131,072 bytes)

// ----------------------------------------------------------------------------------------------------------

// "Heap" (Dynamic Memory Management)
// Virtual Memory Management
// Physical Memory Management

