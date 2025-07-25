// Physical memory manager
// used to allocate pages and keep track of free pages

#define PHYSMEM_START       0x142000
#define PHYSMEM_END         0x8000000
#define VIRTMEM_START_KERN  0xFFFFFFFFF0142000
#define VIRTMEM_END_KERN    0xFFFFFFFFF8000000
#define VIRTMEM_START_USER  0x00007ffff0142000
#define VIRTMEM_END_USER    0x00007ffff8000000
#define PAGE_SIZE           0x1000

typedef struct page{
    struct page* next;
} page;


// page* freelist = 0;

static page* freelist = 0;

// free a page
void pfree(uint8_t* v){
    page* p;
    p = (page*)v;
    p->next = freelist;
    freelist = p;
}

// init the freelist
// void init_pmm(void){
//     for(uint8_t* p = (char*)PHYSMEM_START; p < (uint8_t*)(PHYSMEM_END); p += PAGE_SIZE){
//         pfree(0xFFFFFFFFF0000000 + p);
//     }

// }

size_t round_up_to_page(size_t size) {
    return (size + PAGE_SIZE - 1) & ~(PAGE_SIZE - 1);
}

void* palloc_npages(size_t npages) {
    size_t needed = npages;
    page* prev = 0;
    page* curr = freelist;

    while (curr) {
        page* start = curr;
        page* check = curr;
        size_t count = 1;

        // Check for contiguous physical memory
        while (check->next && 
               (uintptr_t)check->next == (uintptr_t)check + PAGE_SIZE &&
               count < npages) {
            check = check->next;
            count++;
        }

        if (count == npages) {
            // Found a block — cut it from the freelist
            if (prev) {
                prev->next = check->next;
            } else {
                freelist = check->next;
            }

            return start;  // convert to virtual
        }

        prev = curr;
        curr = curr->next;
    }

    return 0;  // No large enough block found
}


void init_pmm(void) {
    for (uintptr_t addr = VIRTMEM_START_KERN; addr + PAGE_SIZE <= VIRTMEM_END_KERN; addr += PAGE_SIZE) {
        page* new_page = (page*)addr;
        new_page->next = freelist;
        freelist = new_page;
    }
}

// used to allocate a page
uint8_t* palloc(void) {
    // kprintf("Kernel entry point: %p\n", freelist);
    if(!freelist) panic("palloc. out of memory...");
    page* p;
    p = freelist;
    freelist = p->next;


    uint8_t* clear = (uint8_t*)p;
    for(uint32_t i = 0; i < 4096; i++){
        *clear = 0;
        clear ++;
    }

    return (uint8_t*)p;
}

