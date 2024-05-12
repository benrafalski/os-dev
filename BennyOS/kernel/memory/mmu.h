
#define HEAP_BASE_VIRT 0xFFFFC90000000000
#define HEAP_BASE_PHYS 0x300000

#define FREE 0x1000
#define USED 0x2000


// 1. 8 pages are allocated
// 2. linked list where each page points to the next page
// 3. when one page is full, we go to the next page

typedef struct {
    uint64_t* page_list_head;
} buddy_allocator_t;


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

void map_page(uint64_t paddr, uint64_t vaddr){
    paddr &= ~(0xFFF);
    vaddr &= ~(0xFFF);

    uint64_t *pml4 = (uint64_t*)0x2000;
    uint64_t *pdpt = (uint64_t*)0x3000;
    uint64_t *pd = (uint64_t*)0x4000;
    uint64_t *pt = (uint64_t*)0x5000;
    uint64_t *page = (uint64_t*)paddr;

    uint64_t pml4_i = (vaddr >> 39) & 0x1ff;
    uint16_t pdpt_i = (vaddr >> 30) & 0x1ff;
    uint16_t pd_i = (vaddr >> 21) & 0x1ff;
    uint16_t pt_i = (vaddr >> 12) & 0x1ff;

    pml4[pml4_i] = (uint64_t)0x3003;
    pdpt[pdpt_i] = (uint64_t)0x4003;
    pd[pd_i] = (uint64_t)0x5003;
    pt[pt_i] = (uint64_t)(paddr | 3);
}


void map_kernel(){
    // kernel code = 0xFFFFFFFF80000000-0xFFFFFFFFA0000000
    uint64_t kstart = 0xFFFFFFFF80000000;
    uint64_t kend = 0xFFFFFFFF00001000;

    uint64_t phys = 0x1000;
    while(kstart < kend){
        map_page(phys, kstart);
        phys += 0x1000;
        kstart += 0x1000;
    }

}


void mem(void){
    // map_kernel();
    // map_page(0x1000, 0xdeadb000);
    map_page(0x300000, 0xFFFFC90000000000);
    // uint64_t *ptr = (uint64_t*)0xdeadb000;
    // uint64_t temp1 = *ptr;
    // char str3[10];
    // char* hex5 = citoa(temp1, str3, 16);
    // kputs(hex5);

}

void* kmalloc(uint64_t size){

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
// 0xFFFFC90000000000 - 0xFFFFEA0000000000 virtual address
// 0x300000 is metadata stuff
// 0x400000 is where the actual heap starts

// Kernel = 0x8000 - 0x28000 (131,072 bytes)

// ----------------------------------------------------------------------------------------------------------

// "Heap" (Dynamic Memory Management)
// Virtual Memory Management
// Physical Memory Management

