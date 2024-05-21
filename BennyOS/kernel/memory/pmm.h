// Physical memory manager
// used to allocate pages and keep track of free pages


// // currently 0x300000 - 0x500000 is the heap physical space

#define PHYSMEM_START   0x300000
#define PHYSMEM_END     0x7FFF000
#define PAGE_SIZE       0x1000


// TODO
// identity map the all of memory

typedef struct page{
    uint64_t ptr;
    struct page* next;
} page;


page* freelist = 0;

// free a page
void pfree(char* v){
    page new_page;
    new_page.ptr = (uint64_t)v;
    new_page.next = freelist;
    freelist = &new_page;   
}

// init the freelist
void init_physmem(void){
    for(char* p = (char*)PHYSMEM_START; p <= (char*)(PHYSMEM_END); p += PAGE_SIZE){
        pfree(p);
    }


    
    // kprintf()
}

// used to allocate a page
void palloc(void) {
    // if there is a free page get it
    // if

    // otherwise panic
    panic("palloc. out of memory..."); 
}

