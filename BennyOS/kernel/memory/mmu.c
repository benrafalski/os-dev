// mmu.c - Memory manager implementation

#include "mmu.h"
#include "../print/print.h"

memory_manager_t g_memory_manager = {0};

// Forward declarations for internal functions
static void buddy_init(void);
static void* buddy_alloc(size_t order);
static void buddy_free(void* ptr, size_t order);

static inline uintptr_t align_up(uintptr_t addr, size_t alignment) {
    return (addr + alignment - 1) & ~(alignment - 1);
}

static inline uintptr_t align_down(uintptr_t addr, size_t alignment) {
    return addr & ~(alignment - 1);
}

static inline uintptr_t buddy_addr(uintptr_t addr, size_t order) {
    return addr ^ (PAGE_SIZE << order);
}

static size_t size_to_order(size_t size) {
    size_t order = 0;
    size_t page_count = (size + PAGE_SIZE - 1) >> PAGE_SHIFT;
    
    while ((1UL << order) < page_count && order <= MAX_ORDER) {
        order++;
    }
    return order <= MAX_ORDER ? order : MAX_ORDER;
}

void parse_memory_map(void) {
    uint16_t entry_count = *(uint16_t*)MEMORY_MAP_ADDR;
    memory_map_entry_t* entries = (memory_map_entry_t*)(MEMORY_MAP_ADDR + 4);
    
    kprintf("=== Parsing Memory Map ===\n");
    kprintf("Found %d memory regions:\n", entry_count);
    
    g_memory_manager.region_count = 0;
    g_memory_manager.total_memory = 0;
    g_memory_manager.available_memory = 0;
    
    for (int i = 0; i < entry_count && g_memory_manager.region_count < MAX_MEMORY_REGIONS; i++) {
        memory_region_t* region = &g_memory_manager.regions[g_memory_manager.region_count];
        
        region->start_addr = entries[i].base_addr;
        region->end_addr = entries[i].base_addr + entries[i].length;
        region->size = entries[i].length;
        region->available = (entries[i].type == MEMORY_TYPE_AVAILABLE);
        region->next = (g_memory_manager.region_count + 1 < MAX_MEMORY_REGIONS) ? 
                       &g_memory_manager.regions[g_memory_manager.region_count + 1] : NULL;
        
        g_memory_manager.total_memory += entries[i].length;
        
        if (region->available) {
            g_memory_manager.available_memory += entries[i].length;
            kprintf("  Available: 0x%x - 0x%x (size: 0x%x)\n", 
                   region->start_addr, region->end_addr, region->size);
        } else {
            kprintf("  Reserved:  0x%x - 0x%x (size: 0x%x)\n", 
                   region->start_addr, region->end_addr, region->size);
        }
        
        g_memory_manager.region_count++;
    }
    
    kprintf("Total memory: %d MB\n", g_memory_manager.total_memory / (1024 * 1024));
    kprintf("Available: %d MB\n", g_memory_manager.available_memory / (1024 * 1024));
    kprintf("============================\n\n");
}

static void buddy_init_region(uint64_t start, uint64_t end) {
    // Skip low memory and kernel area
    if (start < 0x100000) {  // Skip first 1MB
        start = 0x100000;
    }
    
    // Skip kernel area (assuming kernel is at 0x8000-0x100000)
    if (start < 0x200000 && end > 0x8000) {
        if (start < 0x200000) start = 0x200000;  // Skip to 2MB
    }
    
    start = align_up(start, PAGE_SIZE);
    end = align_down(end, PAGE_SIZE);
    
    if (start >= end) return;
    
    kprintf("Initializing buddy allocator for region: 0x%x - 0x%x\n", start, end);
    
    while (start + PAGE_SIZE <= end) {
        size_t order = MAX_ORDER;
        
        // Find largest block that fits
        while (order > 0) {
            size_t block_size = PAGE_SIZE << order;
            if ((start % block_size) == 0 && start + block_size <= end) {
                break;
            }
            order--;
        }
        
        // Add block to free list
        buddy_block_t* block = (buddy_block_t*)start;
        block->order = order;
        block->next = g_memory_manager.free_lists[order];
        g_memory_manager.free_lists[order] = block;
        
        start += (PAGE_SIZE << order);
    }
}

void buddy_init(void) {
    // Clear free lists
    for (int i = 0; i <= MAX_ORDER; i++) {
        g_memory_manager.free_lists[i] = NULL;
    }
    
    // Initialize buddy allocator for each available region
    for (int i = 0; i < g_memory_manager.region_count; i++) {
        memory_region_t* region = &g_memory_manager.regions[i];
        if (region->available && region->size >= PAGE_SIZE) {
            buddy_init_region(region->start_addr, region->end_addr);
        }
    }
}

void* buddy_alloc(size_t order) {
    if (order > MAX_ORDER) return NULL;
    
    // Find a free block
    for (size_t current_order = order; current_order <= MAX_ORDER; current_order++) {
        if (g_memory_manager.free_lists[current_order]) {
            // Split blocks down to required order
            while (current_order > order) {
                buddy_block_t* block = g_memory_manager.free_lists[current_order];
                g_memory_manager.free_lists[current_order] = block->next;
                current_order--;
                
                uintptr_t addr = (uintptr_t)block;
                size_t half_size = PAGE_SIZE << current_order;
                
                buddy_block_t* buddy1 = (buddy_block_t*)addr;
                buddy_block_t* buddy2 = (buddy_block_t*)(addr + half_size);
                
                buddy1->order = current_order;
                buddy2->order = current_order;
                
                buddy1->next = g_memory_manager.free_lists[current_order];
                buddy2->next = buddy1;
                g_memory_manager.free_lists[current_order] = buddy2;
            }
            
            // Allocate the block
            buddy_block_t* result = g_memory_manager.free_lists[order];
            g_memory_manager.free_lists[order] = result->next;
            g_memory_manager.allocated_memory += PAGE_SIZE << order;
            
            // Clear the allocated memory
            uint8_t* mem = (uint8_t*)result;
            for (size_t i = 0; i < (PAGE_SIZE << order); i++) {
                mem[i] = 0;
            }
            
            return result;
        }
    }
    
    return NULL;
}

void buddy_free(void* ptr, size_t order) {
    if (!ptr || order > MAX_ORDER) return;
    
    uintptr_t base = (uintptr_t)ptr;
    g_memory_manager.allocated_memory -= PAGE_SIZE << order;
    
    // Try to merge with buddy
    while (order < MAX_ORDER) {
        uintptr_t buddy = buddy_addr(base, order);
        
        buddy_block_t** prev = &g_memory_manager.free_lists[order];
        buddy_block_t* curr = g_memory_manager.free_lists[order];
        
        while (curr) {
            if ((uintptr_t)curr == buddy && curr->order == order) {
                // Found buddy, merge
                *prev = curr->next;
                base = base < buddy ? base : buddy;
                order++;
                goto continue_merge;
            }
            prev = &curr->next;
            curr = curr->next;
        }
        break;  // No buddy found
        
    continue_merge:
        continue;
    }
    
    // Add merged block to free list
    buddy_block_t* block = (buddy_block_t*)base;
    block->order = order;
    block->next = g_memory_manager.free_lists[order];
    g_memory_manager.free_lists[order] = block;
}

// High-level allocation functions
void* kmalloc(size_t size) {
    if (size == 0) return NULL;
    
    size_t order = size_to_order(size);
    return buddy_alloc(order);
}

void kfree(void* ptr) {
    if (!ptr) return;
    
    // For simplicity, we'll track the order in the block header
    // In a real implementation, you'd maintain a separate allocation table
    buddy_block_t* block = (buddy_block_t*)ptr;
    buddy_free(ptr, block->order);
}

void* alloc_page(void) {
    return buddy_alloc(0);
}

void free_page(void* page) {
    buddy_free(page, 0);
}

void* alloc_pages(int order) {
    return buddy_alloc(order);
}

void free_pages(void* pages, int order) {
    buddy_free(pages, order);
}

void vmm_map_page(uint64_t* pml4, uintptr_t virt, uintptr_t phys, uint64_t flags) {
    int pml4_i = (virt >> 39) & 0x1FF;
    int pdpt_i = (virt >> 30) & 0x1FF;
    int pd_i   = (virt >> 21) & 0x1FF;
    int pt_i   = (virt >> 12) & 0x1FF;

    // Walk or create page table hierarchy
    if (!(pml4[pml4_i] & PAGE_PRESENT)) {
        uintptr_t new_pdpt = (uintptr_t)alloc_page();
        if (!new_pdpt) return;
        pml4[pml4_i] = new_pdpt | flags | PAGE_PRESENT;
    }

    uint64_t* pdpt = (uint64_t*)(pml4[pml4_i] & ~0xFFF);

    if (!(pdpt[pdpt_i] & PAGE_PRESENT)) {
        uintptr_t new_pd = (uintptr_t)alloc_page();
        if (!new_pd) return;
        pdpt[pdpt_i] = new_pd | flags | PAGE_PRESENT;
    }

    uint64_t* pd = (uint64_t*)(pdpt[pdpt_i] & ~0xFFF);

    if (!(pd[pd_i] & PAGE_PRESENT)) {
        uintptr_t new_pt = (uintptr_t)alloc_page();
        if (!new_pt) return;
        pd[pd_i] = new_pt | flags | PAGE_PRESENT;
    }

    uint64_t* pt = (uint64_t*)(pd[pd_i] & ~0xFFF);
    pt[pt_i] = phys | flags | PAGE_PRESENT;
}
// Add this function to maintain compatibility with keyboard driver
void print_memory_map(void) {
    // First print the raw memory map from bootloader
    uint16_t entry_count = *(uint16_t*)MEMORY_MAP_ADDR;
    memory_map_entry_t* entries = (memory_map_entry_t*)(MEMORY_MAP_ADDR + 4);
    
    kprintf("\n=== Memory Map ===\n");
    kprintf("Found %d memory regions:\n", entry_count);
    
    for (int i = 0; i < entry_count; i++) {
        const char* type_str;
        switch(entries[i].type) {
            case MEMORY_TYPE_AVAILABLE: type_str = "Available"; break;
            case MEMORY_TYPE_RESERVED: type_str = "Reserved"; break;
            case MEMORY_TYPE_ACPI_RECLAIMABLE: type_str = "ACPI Reclaimable"; break;
            case MEMORY_TYPE_ACPI_NVS: type_str = "ACPI NVS"; break;
            case MEMORY_TYPE_BAD: type_str = "Bad Memory"; break;
            default: type_str = "Unknown"; break;
        }
        
        kprintf("Region %d:\n", i);
        kprintf("  Base: 0x%x\n", entries[i].base_addr);
        kprintf("  Length: 0x%x\n", entries[i].length);
        kprintf("  Type: %s\n", type_str);
        kprintf("  Attrs: 0x%x\n\n", entries[i].extended_attrs);
    }
    
    // Then print memory manager status
    // print_memory_info();
}
void print_memory_info(void) {
    kprintf("\n=== Memory Manager Status ===\n");
    kprintf("Total Memory: %d MB\n", g_memory_manager.total_memory / (1024 * 1024));
    kprintf("Available: %d MB\n", g_memory_manager.available_memory / (1024 * 1024));
    kprintf("Allocated: %d KB\n", g_memory_manager.allocated_memory / 1024);
    kprintf("Free Lists:\n");
    
    for (int i = 0; i <= MAX_ORDER; i++) {
        int count = 0;
        buddy_block_t* block = g_memory_manager.free_lists[i];
        while (block) {
            count++;
            block = block->next;
        }
        if (count > 0) {
            kprintf("  Order %d (%d KB blocks): %d free\n", 
                   i, (4 << i), count);
        }
    }
    kprintf("=============================\n\n");
}

void memory_init(void) {
    kprintf("Initializing memory manager...\n");
    
    // Parse memory map from bootloader
    parse_memory_map();
    
    // Initialize buddy allocator
    buddy_init();
    
    // Set up page tables (your existing code)
    g_memory_manager.pml4 = (uint64_t*)0x2000;
    
    // Map kernel to higher half
    for (uintptr_t offset = 0; offset < 0x100000; offset += PAGE_SIZE) {
        uintptr_t virt = KERNEL_VIRT_BASE + offset;
        uintptr_t phys = 0x8000 + offset;  // Kernel physical base
        vmm_map_page(g_memory_manager.pml4, virt, phys, PAGE_WRITE);
    }
    
    print_memory_info();
    kprintf("Memory manager initialized successfully!\n\n");
}