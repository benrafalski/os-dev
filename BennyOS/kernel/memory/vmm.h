// Virtual memory manager
// 

#define IS_CONONICAL(addr) (((addr >= 0xffff800000000000 && addr <= 0xffffffffffffffff) || \
                         (addr >= 0x0000000000000000 && addr <= 0x00007fffffffffff)) ? \
                         true : false)

#define KERN_BASE   0xFFFFFFFFF0000000
#define VIRT_TO_PHYS(addr) (addr - KERN_BASE)
#define PHYS_TO_VIRT(addr) (KERN_BASE + addr)


void map_page(uint64_t paddr, uint64_t vaddr, uint64_t ptaddr){
    paddr &= ~(0xFFF);
    vaddr &= ~(0xFFF);

    uint64_t *pml4 = (uint64_t*)0x2000;
    uint64_t *pdpt = (uint64_t*)0x100000;
    uint64_t *pd = (uint64_t*)0x101000;
    uint64_t *pt = (uint64_t*)ptaddr;
    uint64_t *page = (uint64_t*)paddr;

    uint64_t pml4_i = (vaddr >> 39) & 0x1ff;
    uint64_t pdpt_i = (vaddr >> 30) & 0x1ff;
    uint64_t pd_i = (vaddr >> 21) & 0x1ff;
    uint64_t pt_i = (vaddr >> 12) & 0x1ff;

    pml4[pml4_i] = (uint64_t)0x100003;
    pdpt[pdpt_i] = (uint64_t)0x101003;
    pd[pd_i] = (uint64_t)(ptaddr | 3);
    pt[pt_i] = (uint64_t)(paddr | 3);
}

// maps 0x100000 bytes to RAM
void map_256(uint64_t pstart, uint64_t vstart, uint64_t ptaddr){
    for(int i = 0; i < 256; i++){
        map_page(pstart, vstart, ptaddr);
        pstart += 0x1000;
        vstart += 0x1000;
    }
}

// maps 0x100000 bytes to RAM
void map_512(uint64_t pstart, uint64_t vstart, uint64_t ptaddr){
    for(int i = 0; i < 512; i++){
        map_page(pstart, vstart, ptaddr);
        pstart += 0x1000;
        vstart += 0x1000;
    }
}

void init_vmm(){
    // kernel mem = 0xFFFFFFFFF0100000-0xFFFFFFFFF8000000
    // mapped to = 0x100000-0x8000000
    // at 0x102000-0x142000 (physical and virtual)

    map_256(0x100000, 0xFFFFFFFFF0100000, 0x102000);
    uint64_t ptstart = 0x103000;
    for(uint64_t i = 0x200000; i < 0x8000000; i += 0x200000){
        map_512(i, 0xFFFFFFFFF0000000+i, ptstart);
        ptstart += 0x1000;
    }
}