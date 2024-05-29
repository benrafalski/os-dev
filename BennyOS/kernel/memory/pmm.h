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


page* freelist = 0;

// free a page
void pfree(uint8_t* v){
    page* p;
    p = (page*)v;
    p->next = freelist;
    freelist = p;
}

// init the freelist
void init_pmm(void){
    for(uint8_t* p = (char*)PHYSMEM_START; p < (uint8_t*)(PHYSMEM_END); p += PAGE_SIZE){
        pfree(0xFFFFFFFFF0000000 + p);
    }
}

// used to allocate a page
uint8_t* palloc(void) {
    if(!freelist) panic("palloc. out of memory...");
    page* p;
    p = freelist;
    freelist = p->next;
    return (uint8_t*)p;
}

