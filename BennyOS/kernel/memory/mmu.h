
// 1. allocate a page

// typedef struct {
//     uint64_t* ptr;
//     buddy_allocator_t* next;
// } buddy_allocator_t;


// buddy_allocator_t* head;

// head->ptr = 0;
// head->next = 0;

// uint8_t get_max_phys_addr(void)
// {
//     int eax, unused;
//     __cpuid(0x80000008, eax, unused, unused, unused);
//     return eax & (0xff);                // 40
// }

// uint8_t get_max_virt_addr(void)
// {
//     int eax, unused;
//     __cpuid(0x80000008, eax, unused, unused, unused);
//     return (eax & (0xff00)) >> 8;       // 48
// }

// bool is_valid_canonical(uint64_t addr){
//     if(addr >= 0xffff800000000000 && addr <= 0xffffffffffffffff){
//         return true;
//     }
//     if(addr >= 0x0000000000000000 && addr <= 0x00007fffffffffff){
//         return true;
//     }
//     return false;
// }


// maps 0x1000 (4096 bytes) to RAM


// void* kmalloc(uint64_t size){

//     uint64_t size_real;

//     if(size <= 8){
//         // malloc 8
//         size_real = 8;
//     }else if(size <= 16 && size > 8){
//         // mallic 16
//         size_real = 16;
//     }else if(size <= 32 && size > 16){
//         // malloc 32
//         size_real = 32;
//     }else if(size <= 64 && size > 32){
//         // malloc 64
//         size_real = 64;
//     }else if(size <= 128 && size > 64){
//         // malloc 128
//         size_real = 128;
//     }else if(size <= 256 && size > 128){
//         // malloc 256
//         size_real = 256;
//     }else if(size <= 512 && size > 256){
//         // malloc 512
//         size_real = 512;
//     }else if(size <= 1024 && size > 512){
//         // malloc 1024
//         size_real = 1024;
//     }else if(size <= 2048 && size > 1024){
//         // malloc 2048
//         size_real = 2048;
//     }else if(size <= 4096 && size > 2048){
//         // malloc 4096
//         size_real = 4096;
//     }

//     if(!head->ptr){
//         kputs("here");
//     }
//     else{
//         kputs("there");
//     }

// }

// void kfree(void* ptr){

// }


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




// PMM
// 1. 


// Region                   | Start Address         | Notes
// Kernel code/data         | 0xFFFFFFFF80000000    | Identity map during boot, then switch to higher half
// Kernel heap              | 0xFFFFFFFFC0000000    | Virtual heap area for kmalloc
// Kernel stack             | 0xFFFFFFFFFFFFF000    | Stack grows down, maybe one per CPU/thread
// Page tables              | Anywhere              | Just needs to be accessible—track them yourself
// Kernel reserved mappings | N/A                   | Map MMIO regions or kernel-specific mappings here

// Region           | Start Address                 | Notes
// User code/data   | 0x0000000000400000            | Typical ELF load address
// Stack            | 0x00007FFFFFFFF000            | Stack grows down
// Heap             | Dynamically placed            | You can use brk()/mmap()-like interfaces later
// Shared libraries | Somewhere in mid-upper range  | e.g., 0x00007F...
// Reserved space   | Bottom pages                  | Avoid using first pages (null pointer traps)



#define KERNEL_BASE             0xFFFFFFFF80000000
#define KERNEL_HEAP_START       0xFFFFFFFFC0000000
#define KERNEL_STACK_TOP        0xFFFFFFFFFFE00000
#define PAGE_SIZE               0x1000
#define PHYS_MEM_START          0x00142000
#define PHYS_MEM_END            0x08000000
#define KERNEL_PHYS_BASE 0x8000 
#define KERNEL_VIRT_BASE 0xFFFFFFFF80000000 // end = 0xffffffff80100000
#define KERNEL_SIZE      0x100000 // 1MB
#define KERNEL_STACK_SIZE 0x4000 // 16KB
#define KERNEL_STACK_BASE   (KERNEL_STACK_TOP - KERNEL_STACK_SIZE)
#define PAGE_PRESENT    0x1
#define PAGE_WRITE      0x2
#define PAGE_USER       0x4
#define PAGE_SIZE_2MB   0x80
#define TOTAL_FRAMES    ((PHYS_MEM_END - PHYS_MEM_START) / PAGE_SIZE)
#define KERNEL_REMAP(addr) (KERNEL_VIRT_BASE + (addr - KERNEL_PHYS_BASE))
// Total memory: 0x08000000 - 0x00142000 = 0x7E5E000
// Total frames = 0x7E5E000 / 0x1000 = ~8102 frames

#define MAX_ORDER     10          // Max block size = 4MB (2^10 * 4KB)
#define ALIGN_UP(x, a)   (((x) + (a - 1)) & ~(a - 1))
#define ALIGN_DOWN(x, a) ((x) & ~(a - 1))

typedef struct buddy_block {
    struct buddy_block* next;
} buddy_block_t;

static buddy_block_t* free_list[MAX_ORDER + 1];
uint64_t* pml4_ptr = (uint64_t*)0x2000; // hard-coded PML4 address

static inline uintptr_t buddy_addr(uintptr_t addr, size_t order) {
    return addr ^ (PAGE_SIZE << order);
}

static void memset_u8(uint8_t* dest, uint8_t val, size_t len) {
    for (size_t i = 0; i < len; i++) {
        dest[i] = val;
    }
}

static void clear_free_list() {
    for (size_t i = 0; i <= MAX_ORDER; i++) {
        free_list[i] = 0;
    }
}

void buddy_init() {
    clear_free_list();

    uintptr_t start = ALIGN_UP(PHYS_MEM_START, PAGE_SIZE);
    uintptr_t end   = ALIGN_DOWN(PHYS_MEM_END, PAGE_SIZE);

    while (start + PAGE_SIZE <= end) {
        size_t order = MAX_ORDER;
        while (order > 0) {
            size_t size = PAGE_SIZE << order;
            if ((start % size) == 0 && start + size <= end) break;
            order--;
        }

        buddy_block_t* block = (buddy_block_t*)start;
        block->next = free_list[order];
        free_list[order] = block;

        start += (PAGE_SIZE << order);
    }
}

void* buddy_alloc(size_t order) {
    if (order > MAX_ORDER) return NULL;

    for (size_t current_order = order; current_order <= MAX_ORDER; current_order++) {
        if (free_list[current_order]) {
            while (current_order > order) {
                buddy_block_t* block = free_list[current_order];
                free_list[current_order] = block->next;
                current_order--;

                uintptr_t addr = (uintptr_t)block;
                buddy_block_t* buddy1 = (buddy_block_t*)addr;
                buddy_block_t* buddy2 = (buddy_block_t*)(addr + (PAGE_SIZE << current_order));

                // Fix: Properly link both buddies to the free list
                buddy1->next = free_list[current_order];
                buddy2->next = buddy1;
                free_list[current_order] = buddy2;
            }

            buddy_block_t* result = free_list[order];
            free_list[order] = result->next;
            return (void*)result;
        }
    }

    return NULL;
}

void buddy_free(void* ptr, size_t order) {
    if (!ptr || order > MAX_ORDER) return;

    uintptr_t base = (uintptr_t)ptr;

    while (order < MAX_ORDER) {
        uintptr_t buddy = buddy_addr(base, order);

        buddy_block_t** prev = &free_list[order];
        buddy_block_t* curr = free_list[order];
        while (curr) {
            if ((uintptr_t)curr == buddy) {
                *prev = curr->next;
                base = base < buddy ? base : buddy;
                order++;
                goto continue_merge;
            }
            prev = &curr->next;
            curr = curr->next;
        }
        break;
continue_merge:
        continue;
    }

    buddy_block_t* block = (buddy_block_t*)base;
    block->next = free_list[order];
    free_list[order] = block;
}

void vmm_map_page(uint64_t* pml4, uintptr_t virt, uintptr_t phys, uint64_t flags) {
    int pml4_i = (virt >> 39) & 0x1FF;
    int pdpt_i = (virt >> 30) & 0x1FF;
    int pd_i   = (virt >> 21) & 0x1FF;
    int pt_i   = (virt >> 12) & 0x1FF;

    // Walk or create the PDPT
    if (!(pml4[pml4_i] & PAGE_PRESENT)) {
        uintptr_t new_pdpt = (uintptr_t)buddy_alloc(0);
        for (int i = 0; i < 512; i++) ((uint64_t*)new_pdpt)[i] = 0;
        pml4[pml4_i] = new_pdpt | flags | PAGE_PRESENT;
    }

    uint64_t* pdpt = (uint64_t*)(pml4[pml4_i] & ~0xFFF);

    // Walk or create the PD
    if (!(pdpt[pdpt_i] & PAGE_PRESENT)) {
        uintptr_t new_pd = (uintptr_t)buddy_alloc(0);
        for (int i = 0; i < 512; i++) ((uint64_t*)new_pd)[i] = 0;
        pdpt[pdpt_i] = new_pd | flags | PAGE_PRESENT;
    }

    uint64_t* pd = (uint64_t*)(pdpt[pdpt_i] & ~0xFFF);

    // Walk or create the PT
    if (!(pd[pd_i] & PAGE_PRESENT)) {
        uintptr_t new_pt = (uintptr_t)buddy_alloc(0);
        for (int i = 0; i < 512; i++) ((uint64_t*)new_pt)[i] = 0;
        pd[pd_i] = new_pt | flags | PAGE_PRESENT;
    }

    uint64_t* pt = (uint64_t*)(pd[pd_i] & ~0xFFF);

    // Map the page
    pt[pt_i] = phys | flags | PAGE_PRESENT;
}

void vmm_identity_map_range(uint64_t* pml4, uintptr_t start, uintptr_t end, uint64_t flags) {
    start = ALIGN_DOWN(start, PAGE_SIZE);
    end   = ALIGN_UP(end, PAGE_SIZE);

    for (uintptr_t addr = start; addr < end; addr += PAGE_SIZE) {
        vmm_map_page(pml4, addr, addr, flags);
    }
}

void map_kernel_higher_half() {
    for (uintptr_t offset = 0; offset < KERNEL_SIZE; offset += PAGE_SIZE) {
        uintptr_t virt = KERNEL_VIRT_BASE + offset;
        uintptr_t phys = KERNEL_PHYS_BASE + offset;
        vmm_map_page(pml4_ptr, virt, phys, PAGE_WRITE);
    }
}

void setup_higher_half_stack() {
    uintptr_t stack_bottom = KERNEL_STACK_TOP - KERNEL_STACK_SIZE;
    for (uintptr_t addr = stack_bottom; addr < KERNEL_STACK_TOP; addr += PAGE_SIZE) {
        uintptr_t phys = (uintptr_t)buddy_alloc(0);
        vmm_map_page(pml4_ptr, addr, phys, PAGE_WRITE);
    }
}


void memory_init() {
    // Identity-map 0x00142000 – 0x08000000
    vmm_identity_map_range(pml4_ptr, 0x00142000, 0x08000000, PAGE_WRITE);
    buddy_init();
    map_kernel_higher_half();
    // setup_higher_half_stack();
}

