
#define RSHIFT_OR(n, b) (n | (n >> b))
#define BIN(n) (RSHIFT_OR(RSHIFT_OR(RSHIFT_OR(RSHIFT_OR(RSHIFT_OR(n-1, 1), 2), 4), 8), 16) + 1)
            



// can allocate
// 8, 16, 32, 64, 128, 256, 512, 1024, 2048, 4096 size blocks
void* malloc(uint32_t size){

    uint32_t bin = BIN(size);
    kprintf("%d\n", bin);




}