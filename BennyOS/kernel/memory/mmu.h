
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

    uint64_t *pml4 = (uint64_t*)0x9000;
    uint64_t *pdpt = (uint64_t*)0xA000;
    uint64_t *pd = (uint64_t*)0xB000;
    uint64_t *pt = (uint64_t*)0xC000;
    uint64_t *page = (uint64_t*)paddr;

    uint16_t pdpt_i = (vaddr >> 30) & 0x1ff;
    uint16_t pd_i = (vaddr >> 21) & 0x1ff;
    uint16_t pt_i = (vaddr >> 12) & 0x1ff;

    pdpt[pdpt_i] = (uint64_t)0xB003;
    pd[pd_i] = (uint64_t)0xC003;
    pt[pt_i] = (uint64_t)(paddr | 3);
}

void mem(void){

    // uint64_t *pml4 = (uint64_t*)0x9000;
    // uint64_t *pdpt = (uint64_t*)0xA000;
    // uint64_t *pd = (uint64_t*)0xB000;
    // uint64_t *pt = (uint64_t*)0xC000;
    // uint64_t *page = (uint64_t*)0x100000;

    // pdpt[3] = (uint64_t)0xB003;
    // pd[245] = (uint64_t)0xC003;
    // pt[219] = (uint64_t)0x100003;

    map_page(0x100000, 0xdeadb000);

    uint64_t *ptr = (uint64_t*)0xdeadb000;
    *ptr = 0xdeadbeef;
    // for(int i = 0; i < 100; i++){
    //     ptr[i] = 'h';
    // }

    uint64_t temp1 = *ptr;
    char str3[10];
    char* hex5 = citoa(temp1, str3, 16);
    kputs(hex5);



    // for(int i = 0; i < 500; i++){
    //     uint64_t temp = page[i];
    //     if(temp){
    //         char str[10];
    //         char* hex = citoa(temp, str, 16);
    //         kputs(hex);

    //         char str2[10];
    //         char* hex2 = citoa(i, str2, 10);
    //         kputs(hex2);
    //     }   
        
    // }
    


    // __asm__(
    //     "mov (%1), %%rax;"
    //     "mov %%rax, %0;"
    //     : "=r"(temp)
    //     : "r" (0xa000 + (8*3))
    //     : "rax"
    // );

    // 0x9000[0] -> 0xA000[0x3]
    // 0xa + 0x3
    // 0b1010000000100011
    // P=1, RW=1, Accessed=1

    // char* temp = (char*)0xdeadbeef;
    // temp[0] = 'j';

    // uint8_t temp = get_max_virt_addr(); = 48
    // uint8_t temp = get_max_phys_addr(); = 40



    


    // // kprintf("addr: 0x%x; ", 0xc000+i);
    // kprintf("val: 0x%x\n", temp);
    

}