
// Metadata structure for tracking allocations
typedef struct kmalloc_header {
    size_t size;        // Size of the allocation in bytes
    size_t order;       // Buddy allocator order used
    uint32_t magic;     // Magic number for corruption detection
} kmalloc_header_t;

#define KMALLOC_MAGIC 0xDEADBEEF
#define HEADER_SIZE sizeof(kmalloc_header_t)

// Convert size to buddy allocator order
static size_t size_to_order(size_t size) {
    // Add header size to the requested size
    size += HEADER_SIZE;
    
    // Find the order needed (minimum order 0 = 4096 bytes)
    if (size <= PAGE_SIZE) {
        return 0;
    }
    
    size_t order = 0;
    size_t block_size = PAGE_SIZE;
    
    while (block_size < size && order <= MAX_ORDER) {
        block_size <<= 1;
        order++;
    }
    
    return order > MAX_ORDER ? MAX_ORDER + 1 : order;
}

void* kmalloc(size_t size) {
    if (size == 0) {
        return NULL;
    }
    
    // Calculate the order needed
    size_t order = size_to_order(size);
    
    // Check if size is too large
    if (order > MAX_ORDER) {
        return NULL;
    }
    
    // Allocate using buddy allocator
    void* block = buddy_alloc(order);
    if (!block) {
        return NULL;
    }

    // Set up the header
    kmalloc_header_t* header = (kmalloc_header_t*)block;
    header->size = size;
    header->order = order;
    header->magic = KMALLOC_MAGIC;
    
    // Return pointer after the header
    return (void*)((uint8_t*)block + HEADER_SIZE);
}

void kfree(void* ptr) {
    if (!ptr) {
        return;
    }
    
    // Get the header
    kmalloc_header_t* header = (kmalloc_header_t*)((uint8_t*)ptr - HEADER_SIZE);
    
    // Validate magic number
    if (header->magic != KMALLOC_MAGIC) {
        // Handle corruption - you might want to panic here
        return;
    }
    
    // Clear the magic number
    header->magic = 0;
    
    // Free using buddy allocator
    buddy_free((void*)header, header->order);
}

// Utility function for allocating and clearing memory
void* kcalloc(size_t num, size_t size) {
    size_t total_size = num * size;
    void* ptr = kmalloc(total_size);
    
    if (ptr) {
        // Clear the allocated memory
        uint8_t* clear_ptr = (uint8_t*)ptr;
        for (size_t i = 0; i < total_size; i++) {
            clear_ptr[i] = 0;
        }
    }
    
    return ptr;
}

// Utility function for reallocating memory
void* krealloc(void* ptr, size_t new_size) {
    if (!ptr) {
        return kmalloc(new_size);
    }
    
    if (new_size == 0) {
        kfree(ptr);
        return NULL;
    }
    
    // Get the old size
    kmalloc_header_t* header = (kmalloc_header_t*)((uint8_t*)ptr - HEADER_SIZE);
    if (header->magic != KMALLOC_MAGIC) {
        return NULL;
    }
    
    size_t old_size = header->size;
    
    // If new size fits in the same block, just update the size
    size_t new_order = size_to_order(new_size);
    if (new_order == header->order) {
        header->size = new_size;
        return ptr;
    }
    
    // Allocate new block
    void* new_ptr = kmalloc(new_size);
    if (!new_ptr) {
        return NULL;
    }
    
    // Copy data from old to new
    size_t copy_size = old_size < new_size ? old_size : new_size;
    uint8_t* src = (uint8_t*)ptr;
    uint8_t* dst = (uint8_t*)new_ptr;
    
    for (size_t i = 0; i < copy_size; i++) {
        dst[i] = src[i];
    }
    
    // Free old block
    kfree(ptr);
    
    return new_ptr;
}