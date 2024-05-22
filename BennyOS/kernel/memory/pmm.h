// Physical memory manager
// used to allocate pages and keep track of free pages

#define PHYSMEM_START   0x100000
#define PHYSMEM_END     0x7FFF000
#define VIRTMEM_START   0xFFFFFFFFFF300000
#define VIRTMEM_END     0xFFFFFFFFF7FFF000
#define PAGE_SIZE       0x1000


// 0xFFFF900000300000
//             200000
// 0xFFFF900007FFF000 

// 0  page table 0xFFFFFFFFFF100000-FFFFFFFFFF300000


// pml4 = 0x2000
// pdpt = 0x100000
// pd = 0x101000
// pt = 0x102000

// FIRST
// pml4[511] = pdpt = 0x100000
// pdpt[511] = pd   = 0x101000
// pd[504]   = pt   = 0x102000
// pt[256]   = addr = 0x100000

// LAST
// pml4[511] = pdpt = 0x100000
// pdpt[511] = pd   = 0x101000
// pd[505]   = pt   = 0x102000
// pt[255]   = addr = 0x100000




// this first table should be used for 

// 1  page table 0xFFFFFFFFFF300000-FFFFFFFFFF500000

// pml4 = 0x2000
// pdpt = 0x100000
// pd = 0x101000
// pt = 0x102000

// FIRST
// pml4[511] = pdpt = 0x100000
// pdpt[511] = pd   = 0x101000
// pd[505]   = pt   = 0x103000
// pt[256]   = addr = 0x100000

// LAST
// pml4[511] = pdpt = 0x100000
// pdpt[511] = pd   = 0x101000
// pd[506]   = pt   = 0x103000
// pt[255]   = addr = 0x100000

// 2  page table 0xFFFFFFFFFF500000-FFFFFFFFFF700000
// 3  page table 0xFFFFFFFFFF700000-FFFFFFFFFF900000
// 4  page table 0xFFFFFFFFFF900000-FFFFFFFFFFB00000
// 5  page table 0xFFFFFFFFFFB00000-FFFFFFFFFFD00000
// 6  page table 0xFFFFFFFFFFD00000-FFFFFFFFFFF00000

// 7  page table 0xFFFFFFFFFFF00000-FFFFFFFFF1100000
// 8  page table 0xFFFFFFFFF1100000-FFFFFFFFF1300000
// 9  page table 0xFFFFFFFFF1300000-FFFFFFFFF1500000
// 10 page table 0xFFFFFFFFF1500000-FFFFFFFFF1700000
// 11 page table 0xFFFFFFFFF1700000-FFFFFFFFF1900000
// 12 page table 0xFFFFFFFFF1900000-FFFFFFFFF1B00000
// 13 page table 0xFFFFFFFFF1B00000-FFFFFFFFF1D00000
// 14 page table 0xFFFFFFFFF1D00000-FFFFFFFFF1F00000


// cr2 = 



// TODO
// identity map the all of memory

typedef struct page{
    struct page* next;
} page;


page* freelist = 0;

// free a page
void pfree(char* v){


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

