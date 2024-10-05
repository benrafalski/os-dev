#include "cpuid.h"
#include "print/print.h"
#include "interrupts/init_idt.h"
#include "memory/pmm.h"
#include "memory/vmm.h"
#include "memory/malloc.h"
#include "fs/ext2.h"

#define APIC_MASK 1 << 9

static int check_apic(void){
    uint32_t unused, edx;
    __cpuid(1, unused, unused, unused, edx);
    return edx & APIC_MASK;
}

void interrupt(){
    __asm__ __volatile__("int3;");
}


void main()
{
    // set_color(WHITE_FGD, BLUE_BGD);
    set_color(LIGHT_GREEN_FGD, DARK_GRAY_BGD);
    clear_screen();

    if(check_apic()){
        kputs("+--------------------------------------+");
        kputs("|Hello from the kernel! APIC IS allowed|");
        kputs("+--------------------------------------+\n");
    }else{
        kputs("+--------------------------------------+");
        kputs("Hello from the kernel! APIC NOT allowed");
        kputs("+--------------------------------------+\n");
    }  

    // init terminal
    kprintf(">:  ", 0);

    // remap PIC
    pic_disable();
    pic_remapping(0x20);

    // intialize the IDT
    idt_init();

    // enable keyboard interrupts
    pic_clear_mask(1);
    pic_clear_mask(2);
    idt_set_descriptor(IRQ_KEYBOARD, isr_stub_table[IRQ_KEYBOARD], PRESENT|DPL_0|INT_GATE);

    // maps pages for kernel memory from 
    //      virtual memory  = 0xFFFFFFFFF0142000-0xFFFFFFFFF8000000
    //      physical memory = 0x142000 - 0x8000000
    init_vmm();
    // frees all of physical memory and inits freelist
    init_pmm();

    // Filesystem
    // using ext2

    // read the superblock from disk
    // located at LBA 2
    super_block_t* sb = (super_block_t*)kmalloc(SUPER_BLOCK_SIZE);
    ata_lba_read(SUPER_BLOCK_LBA, SUPER_BLOCK_SIZE/SECTOR_SIZE, (char*)sb);

    if(sb->signature != 0xEF53) 
        panic("Incorrect ext2 signature...");

    if(sb->fs_state != 1)
        panic("Filesystem has errors...");



    // extract the size of each block, 
    // the total number of inodes, 
    // the total number of blocks, 
    // the number of blocks per block group, 
    // and the number of inodes in each block group

    uint32_t block_size = (uint32_t)sb->block_size;
    uint32_t num_inodes = (uint32_t)sb->num_inodes;
    uint32_t num_blocks = (uint32_t)sb->num_blocks;
    uint32_t num_blocks_per_group = (uint32_t)sb->num_blocks_blockgroup;
    uint32_t num_inodes_per_group = (uint32_t)sb->num_inodes_blockgroup;


    // uint32_t    num_inodes;                 // 0x00008000
    // uint32_t    num_blocks;                 // 0x00008000
    // uint32_t    num_reserved_blocks;        // 0x00000666
    // uint32_t    num_unalloc_blocks;         // 0x00007be8
    // uint32_t    num_unalloc_inodes;         // 0x00007ff4
    // uint32_t    superblock_blocknum;        // 0x00000000
    // uint32_t    block_size;                 // 0x00000002
    // uint32_t    fragment_size;              // 0x00000002
    // uint32_t    num_blocks_blockgroup;      // 0x00008000
    // uint32_t    num_frags_blockgroup;       // 0x00008000
    // uint32_t    num_inodes_blockgroup;      // 0x00008000
    // uint32_t    last_mount;                 // 0x66ff2f1f
    // uint32_t    last_write;                 // 0x66ff2f1f
    // uint16_t    mounts_since_cc;            // 0x0001
    // uint16_t    mounts_before_cc;           // 0xffff
    // uint16_t    signature;                  // 0xef53
    // uint16_t    fs_state;                   // 0x0001
    // uint16_t    error;                      // 0x0001
    // uint16_t    ver_minor;                  // 0x0000
    // uint32_t    last_cc;                    // 0x66ff2f1
    // uint32_t    int_force_cc;               // 0x00000000
    // uint32_t    os_id;                      // 0x00000000
    // uint32_t    ver_major;                  // 0x00000001
    // uint16_t    reserve_user;               // 0x00000000
    // uint16_t    reserve_group;              // 0x0000000b


    bgd_table_t* bgd_table = (bgd_table_t*)kmalloc(BGD_TABLE_SIZE);
    ata_lba_read(BGD_TABLE_LBA, BGD_TABLE_SIZE/SECTOR_SIZE, (char*)bgd_table);

    // uint32_t    blk_use_bitmap;             // 0x00000009
    // uint32_t    inode_use_bitmap;           // 0x0000000a
    // uint32_t    inode_table;                // 0x0000000b
    // uint16_t    unalloc_blocks;             // 0x7ff47be8
    // uint16_t    unalloc_inodes;             // 0x00000002
    // uint8_t     unused[14];                 // padding


    uint8_t* ptr = (uint8_t*)kmalloc(512);
    ata_lba_read(0xb*8, 1, (char*)ptr);


    inode_t* inode_table = (inode_t*)kmalloc(0x700);
    // ata_lba_read(91, 1, (char*)inode_table);

    read_sectors_lba(88, 4, (char*)inode_table);

    // inode_t kernel_inode = inode_table[3];

    // uint8_t* kernel = (uint8_t*)kmalloc(512);
    // ata_lba_read(kernel_inode.dptr0*8, 1, (char*)kernel);

    // inode_t test = inode_table[0];

    // uint8_t* test_contents = (uint8_t*)kmalloc(SECTOR_SIZE);
    // ata_lba_read(LBA_4096(test.dptr0), 1, (char*)test_contents);
    // kputs(test_contents);




    inode_t test_inode = inode_table[12];
    uint32_t size = (test_inode.size_lower < SECTOR_SIZE ? SECTOR_SIZE : test_inode.size_lower);
    uint8_t* test_contents = (uint8_t*)kmalloc(size);
    read_sectors_lba(LBA_4096(test_inode.dptr0), size/SECTOR_SIZE, (char*)test_contents);
    kputs(test_contents); // prints 'hello'







    // 0xb300
    // 0000b500
    // 0000b580

    // inode at lba 88 -> nothing
    // inode at lba 88.25 -> root
        // type 41ed
        // sector count 0x00000008
        // osspec 0x00000001
        // dptr0 0x0000040b
    // inode at lba 89.5 -> no clue
        // type 0x8180 -> U_WR|U_RD|FILE
        // sector count = 0x00000040
        // doubly pointer = 0x00000410
    // inode at lba 90.5 -> lost+found
        // type 0x41c0 -> DIR|U_RD|O_RD|G_EX
        // sector count = 0x20
        // dptr0 0x0000040c
        // dptr1 0x0000040d
        // dptr2 0x0000040e
        // dptr3 0x0000040f
    // inode at lba 90.75 -> kernel.bin
        // type 0x81ed -> FILE|U_WR|U_EX|G_RD|G_EX|O_RD|O_EX
        // sector count 0x00000038
        // osspec value 0x00000001
        // dptr0 0x00000411
        // dptr1 0x00000412
        // dptr2 0x00000413
        // dptr3 0x00000414
        // dptr4 0x00000415
        // dptr5 0x00000416
        // dptr6 0x00000417
        // generation number = 0x9d93ebb0
    // inode at lba 91 -> test.txt
        // type 81ed
        // sector count is 1
        // dptr0 0x418

    // start kernel.bin 0040b000


    for(;;) {
        asm("hlt");
    }
}

// Phase 0: Introduction
// DONE
// 1. Welcome to Operating Systems Development
// DONE
// 2. Building the latest GCC

// Phase I: Beginning
// In this phase we will set up a toolchain and create a basic kernel that will become the core of the new operating system.
// DONE
// 1. Setting up a Cross-Toolchain
// DONE
// 2. Creating a Hello World kernel
// DONE
// 3. Setting up a Project
// DONE
// 4. Calling Global Constructors
// DONE
// 5. Terminal Support
// TODO
// 6. Stack Smash Protector
// TODO
// 7. Multiboot
// DONE
// 8. Global Descriptor Table
// DONE
// 9. Memory Management
// DONE
// 10. Interrupts
// TODO
// 11. Multithreaded Kernel
// DONE
// 12. Keyboard
// TODO
// 13. Internal Kernel Debugger
// TODO
// 14. Filesystem Support

// Phase II: User-Space
// In this phase we'll expand your operating system into user-space and add support for programs, which is enough for your project to qualify as a small operating system. You need to work on system calls, program loading, memory management and rework parts of your system early in this phase.
// TODO
// 1. User-Space
// TODO
// 2. Program Loading
// TODO
// 3. System Calls
// TODO
// 4. OS Specific Toolchain
// TODO
// 5. Creating a C Library
// TODO
// 6. Fork and Execute
// TODO
// 7. Shell

// Phase III: Extending your Operating System
// With these basic features in place, you can now start writing your operating system and all its wonderful features. You'll add games, editors, test programs, command line utilties, drivers and whatever you can imagine. Your skill and imagination is the limit here. You can delay many of these features until later in your operating system and make then in almost any order.
// TODO
// 1. Time
// TODO
// 2. Threads
// TODO
// 3. Thread Local Storage
// TODO
// 4. Symmetric Multiprocessing
// TODO
// 5. Secondary Storage
// TODO
// 6. Real Filesystems
// TODO
// 7. Graphics
// TODO
// 8. User Interface
// TODO
// 9. Networking
// TODO
// 10. Sound
// TODO
// 11. Universal Serial Bus

// Phase IV: Bootstrapping
// You now have your basic operating system in place and you are ready to move onto the next level. In this phase, we will start porting software onto your operating system such that you can become self-hosting. You already begun your effort toward being self-hosting when you set up your OS-specific toolchain and it pays off now.
// TODO
// 1. Porting Software
// TODO
// 2. Porting GCC
// TODO
// 3. Compiling your OS under your OS
// TODO
// 4. Fully Self-hosting

// Phase V: Profit
// You have now successfully created a real operating system that is fully self-hosting and the envy of the entire operating systems development community. You have ported quake, have OpenGL programs, a working browser, a thriving community of contributors, and much success. You can now start over and develop the next operating system from your own operating system.