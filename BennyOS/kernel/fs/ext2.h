/*
https://wiki.osdev.org/Ext2

Second Extended Filesystem (ext2fs) is a rewrite of the original Extended Filesystem and as such, is also based around the concept of "inodes."
It has native support for UNIX ownership / access rights, symbolic- and hard-links, and other properties that are common among UNIX-like operating systems.
Organizationally, it divides disk space up into groups called "block groups."


Block: fs divides the disk space into logical blocks of contiguous space
Block groups: contiguous groups of blocks, some of the blocks are used for metadata
Inode: structure on the disk that represents a file, dir, or sym-link; not an actual file but link to block with the actual file
*/


#define FILESYSTEM_START_SECTOR 100
#define SUPER_BLOCK_LBA     (FILESYSTEM_START_SECTOR + 2)   // Block 1 of filesystem
#define BGD_TABLE_LBA       (FILESYSTEM_START_SECTOR + 8)   // Block 2 of filesystem  
#define SUPER_BLOCK_SIZE    512
#define BGD_TABLE_SIZE      512
#define SECTOR_SIZE         512
#define SECTORS_PER_DPTR    4


#define INODE_TABLE_SIZE    20
#define INODE_SIZE          128

#define ROOT_INODE          2

// EXT2 types
enum {
  FIFO                  = 0x1000,       // FIFO
  CHAR_DEV              = 0x2000,       // Character Device
  DIR                   = 0x4000,       // Directory
  BLOCK_DEV             = 0x6000,       // Block Device
  FILE                  = 0x8000,       // Regular File
  SYM_LINK              = 0xA000,       // Symbolic Link
  UNIX_SOCK             = 0xC000        // Unix Socket
};


// EXT2 permissions
enum {
  O_EX                  = 0x001,        // Other - execute permission
  O_WR                  = 0x002,        // Other - write permission
  O_RD                  = 0x004,        // Other - read permission
  G_EX                  = 0x008,        // Group - execute permission
  G_WR                  = 0x010,        // Group - write permission
  G_RD                  = 0x020,        // Group - read permission
  U_EX                  = 0x040,        // User - execute permission
  U_WR                  = 0x080,        // User - write permission
  U_RD                  = 0x100,        // User - read permission
  STICKY                = 0x200,        // Sticky Bit
  SGID                  = 0x400,        // Set group ID
  SUID                  = 0x800         // Set user ID
};

// EXT2 Inode flags
enum {
    SEC_DEL             = 0x00000001,   // Secure deletion (not used)
    COPY_DEL            = 0x00000002,   // Keep a copy of data when deleted (not used)
    FILE_COMP           = 0x00000004,   // File compression (not used)
    SYNC_UPDATES        = 0x00000008,   // Synchronous updates—new data is written immediately to disk
    IMMUT_FILE          = 0x00000010,   // Immutable file (content cannot be changed)
    APPEND_ONLY         = 0x00000020,   // Append only
    NO_INC_DUMP         = 0x00000040,   // File is not included in 'dump' command
    NO_UPDATE_LACC      = 0x00000080,   // Last accessed time should not updated
    HASH_IDX_DIR        = 0x00010000,   // Hash indexed directory
    AFS_DIR             = 0x00020000,   // AFS directory
    JOURNAL_DATA        = 0x00040000    // Journal file data
};

// Directory Entry Type Indicators
enum {
    D_UNK               = 0,            // Unknown type
    D_FILE              = 1,            // Regular file
    D_DIR               = 2,            // Directory
    D_CHARDEV           = 3,            // Character device
    D_BLOCKDEV          = 4,            // Block device
    D_FIFO              = 5,            // FIFO
    D_SOCKET            = 6,            // Socket
    D_SYM_LINK          = 7             // Symbolic link (soft link)

};

typedef struct {
    uint16_t cylinder;
    uint8_t head;
    uint8_t sector;
} __attribute__((packed)) chs_t;


extern void ata_chs_read(chs_t chs, uint16_t num_sectors, char* buff_addr);
extern void ata_lba_read(uint32_t lba, uint16_t num_sectors, char* buff_addr);
extern void ata_lba_write(uint32_t lba, uint16_t num_sectors, char* buff_addr);


typedef struct {
    uint32_t    num_inodes;                 // total number of inodes in the file system
    uint32_t    num_blocks;                 // total number of blocks in the file system
    uint32_t    num_reserved_blocks;        // number of blocks reserved for the superuser, see "reserve_user" field
    uint32_t    num_unalloc_blocks;         // total number of unallocated blocks
    uint32_t    num_unalloc_inodes;         // total number of unallocated inodes
    uint32_t    superblock_blocknum;        // block number of the superblock
    uint32_t    block_size;                 // real block size = 1024 << (block_size)
    uint32_t    fragment_size;              // read fragment size = 1024 << (fragment_size)
    uint32_t    num_blocks_blockgroup;      // number of blocks in each group
    uint32_t    num_frags_blockgroup;       // number of fragements in each group
    uint32_t    num_inodes_blockgroup;      // number of inodes in each block group
    uint32_t    last_mount;                 // last mount time (POSIX time)
    uint32_t    last_write;                 // last written time (POSIX time)
    uint16_t    mounts_since_cc;            // number of times the volume had been mounted since last consistency check
    uint16_t    mounts_before_cc;           // number of mounts allowed before a consistency check must be done
    uint16_t    signature;                  // ext2 signature (0xEF53), used to help confirm presence of ext2
    uint16_t    fs_state;                   // file system state; 1 = file system is clean, 2 = file system has errors
    uint16_t    error;                      // what to do on error; 1 = ignore, 2 = remount as read-only, 3 kernel panic
    uint16_t    ver_minor;                  // minor portion of the version
    uint32_t    last_cc;                    // POSIX time of last consistency check
    uint32_t    int_force_cc;               // interval between forced consistency check
    uint32_t    os_id;                      // OS ID of which the filesystem on this volume was created
    uint32_t    ver_major;                  // major portion of version
    uint16_t    reserve_user;               // user ID that can use reserved blocks
    uint16_t    reserve_group;              // group ID that can use reserved blocks
    uint32_t    first_nonrsvd_inode;        // First non-reserved inode in file system. (In versions < 1.0, this is fixed as 11)
    uint16_t    inode_size;                 // Size of each inode structure in bytes. (In versions < 1.0, this is fixed as 128)

} __attribute__((packed)) super_block_t;


typedef struct {
    uint32_t    blk_use_bitmap;             // block number of block usage bitmap
    uint32_t    inode_use_bitmap;           // block number of inode usage bitmap
    uint32_t    inode_table;                // block number of the inode table
    uint16_t    unalloc_blocks;             // number of unallocated blocks
    uint16_t    unalloc_inodes;             // number of unallocated inodes
    uint8_t     unused[14];                 // padding

} __attribute__((packed)) bgd_table_t;


typedef struct {
    uint16_t    type_perms;                 // type and permissions
    uint16_t    uid;                        // user id
    uint32_t    size_lower;                 // lower 32 bits of size in bytes
    uint32_t    lacc_time;                  // last access time
    uint32_t    create_time;                // creation time
    uint32_t    lmod_time;                  // last modification time
    uint32_t    delete_time;                // deletion time
    uint16_t    gid;                        // group id
    uint16_t    hard_links;                 // count of hard links to this inode, when 0 data blocks are marked unallocated
    uint32_t    sector_count;               // count if disk sectors in use by inode
    uint32_t    flags;                      // flags
    uint32_t    os_spec1;                   // OS specific value 1
    uint32_t    dptr0;                      // Direct Block Pointer 0, 12 × 4096 = 49,152 bytes (48 KB)
    uint32_t    dptr1;                      // Direct Block Pointer 1
    uint32_t    dptr2;                      // Direct Block Pointer 2
    uint32_t    dptr3;                      // Direct Block Pointer 3
    uint32_t    dptr4;                      // Direct Block Pointer 4
    uint32_t    dptr5;                      // Direct Block Pointer 5
    uint32_t    dptr6;                      // Direct Block Pointer 6
    uint32_t    dptr7;                      // Direct Block Pointer 7
    uint32_t    dptr8;                      // Direct Block Pointer 8
    uint32_t    dptr9;                      // Direct Block Pointer 9
    uint32_t    dptr10;                     // Direct Block Pointer 10
    uint32_t    dptr11;                     // Direct Block Pointer 11
    uint32_t    sing_idptr;                 // singly indirect block pointer, 1024 × 4096 = 4,194,304 bytes (4 MB)
    uint32_t    doub_idptr;                 // doubly indirect block pointer, 1024 × 1024 × 4096 = 4,294,967,296 bytes (4 GB)
    uint32_t    trip_idptr;                 // triply indirect block pointer, 1024 × 1024 × 1024 × 4096 = 4,398,046,511,104 bytes (4 TB)
    uint32_t    gen_number;                 // generation number
    uint32_t    ext_attr;                   // In version >= 1, extended attribute block
    uint32_t    size_upper;                 // In version >= 1, upper 32 bits of file size
    uint32_t    frag_addr;                  // block address of fragment
    uint8_t     os_spec2[12];               // OS specific value 2
} __attribute__((packed)) inode_t;


typedef struct {
    uint32_t    inode;                      // Inode
    uint16_t    size;                       // Total size of this entry (Including all subfields)
    uint8_t     name_lsb;                   // Name Length least-significant 8 bits
    uint8_t     type;                       // Type indicator (only if the feature bit for "directory entries have file type byte" is set, else this is the most-significant 8 bits of the Name Length)
    uint8_t*    name;                       // Name characters
} __attribute__((packed)) dir_entry_t;


typedef struct dir_list_node_t{
    dir_entry_t* entry;
    struct dir_list_node_t* next;
    struct dir_list_node_t* children;
} __attribute__((packed)) dir_list_node_t;


super_block_t superblock = {0};
bgd_table_t bgd_table = {0};
inode_t inode_table[INODE_TABLE_SIZE] = {0};
uint32_t ext2_block_size = 0;

#define GET_BLOCK_SIZE() (ext2_block_size ? ext2_block_size : (ext2_block_size = (1024 << superblock.block_size)))
#define GET_SECTORS_PER_BLOCK() (GET_BLOCK_SIZE() / SECTOR_SIZE)
#define GET_POINTERS_PER_BLOCK() (GET_BLOCK_SIZE() / 4)
#define BLOCK_TO_LBA(b) ((b * GET_SECTORS_PER_BLOCK()) + FILESYSTEM_START_SECTOR)


void read_superblock(){
    ata_lba_read(SUPER_BLOCK_LBA, SUPER_BLOCK_SIZE/SECTOR_SIZE, (char*)&superblock);
    if(superblock.signature != 0xEF53) {
        panic("Incorrect ext2 signature...");
    }

    if(superblock.fs_state != 1){
        panic("Filesystem has errors...");
    }

    ext2_block_size = 1024 << superblock.block_size;
}

void print_superblock(){
    kprintf("Superblock:\n");
    kprintf("  Inodes: %d\n", superblock.num_inodes);
    kprintf("  Blocks: %d\n", superblock.num_blocks);
    kprintf("  Reserved Blocks: %d\n", superblock.num_reserved_blocks);
    kprintf("  Unallocated Blocks: %d\n", superblock.num_unalloc_blocks);
    kprintf("  Unallocated Inodes: %d\n", superblock.num_unalloc_inodes);
    kprintf("  Block Size: %d\n", GET_BLOCK_SIZE());
    kprintf("  Fragment Size: %d\n", superblock.fragment_size);
    kprintf("  Blocks per Group: %d\n", superblock.num_blocks_blockgroup);
    kprintf("  Fragments per Group: %d\n", superblock.num_frags_blockgroup);
    kprintf("  Inodes per Group: %d\n", superblock.num_inodes_blockgroup);
    kprintf("  Last Mount Time: %d\n", superblock.last_mount);
    kprintf("  Last Write Time: %d\n", superblock.last_write);
    kprintf("  Mounts since last check: %d\n", superblock.mounts_since_cc);
    kprintf("  Mounts before check: %d\n", superblock.mounts_before_cc);
    kprintf("  Filesystem State: %s\n", (superblock.fs_state == 1) ? "Clean" : "Errors");
    kprintf("  Error Handling: %d\n", superblock.error);
    kprintf("  Last Check Time: %d\n", superblock.last_cc);
    kprintf("  Check Interval: %d\n", superblock.int_force_cc);
    kprintf("  OS ID: %d\n", superblock.os_id);
    kprintf("  Major Version: %d\n", superblock.ver_major);
    kprintf("  Minor Version: %d\n", superblock.ver_minor);
    kprintf("  Reserved User ID: %d\n", superblock.reserve_user);
    kprintf("  Reserved Group ID: %d\n", superblock.reserve_group);
    kprintf("  First Non-Reserved Inode: %d\n", superblock.first_nonrsvd_inode);
    kprintf("  Inode Size: %d\n", superblock.inode_size);
}

void read_bgd_table(){
    ata_lba_read(BGD_TABLE_LBA, BGD_TABLE_SIZE/SECTOR_SIZE, (char*)&bgd_table);
}

void print_bgd_table(){
    kprintf("Block Group Descriptor Table:\n");
    kprintf("  Block Usage Bitmap: %d\n", bgd_table.blk_use_bitmap);
    kprintf("  Inode Usage Bitmap: %d\n", bgd_table.inode_use_bitmap);
    kprintf("  Inode Table: %d\n", bgd_table.inode_table);
    kprintf("  Unallocated Blocks: %d\n", bgd_table.unalloc_blocks);
    kprintf("  Unallocated Inodes: %d\n", bgd_table.unalloc_inodes);
}


// reads the specified number of lba sectors
void read_sectors_lba(uint32_t lba, uint16_t num_sectors, char* buff_addr){
    char* start = buff_addr;
    uint32_t lba_num = lba;
    for(uint16_t i = 0; i < num_sectors; i++){
        ata_lba_read(lba_num, 1, (char*)start);
        lba_num++;
        start+=SECTOR_SIZE;
    }
}


void read_inode_table(){
    if(bgd_table.inode_table == 0){
        panic("ext.h (read_inode_table): Cannot read inode_table before bgd_table...");
    }
    read_sectors_lba(BLOCK_TO_LBA(bgd_table.inode_table), (INODE_TABLE_SIZE*INODE_SIZE)/SECTOR_SIZE, (char*)&inode_table);
    // kputs("Inode table loaded.");
}

void print_inode_table(){
    kputs("Inode Table:");
    for(int i = 0; i < INODE_TABLE_SIZE; i++){
        inode_t inode = inode_table[i];
        // if(inode.type_perms != 0){
            kprintf(" Inode %d: type_perms=0x%x, uid=%d, size=%d, gid=%d, hard_links=%d, dptr0=%d, dptr1=%d\n", 
                    i+1, inode.type_perms, inode.uid, inode.size_lower, inode.gid, inode.hard_links, inode.dptr0, inode.dptr1);
        // }
    }
}

uint32_t get_inode_size(inode_t inode){
    return (inode.size_lower < SECTOR_SIZE ? SECTOR_SIZE : inode.size_lower);
}

void read_block(uint32_t block_num, char* buffer) {
    if (block_num == 0) return; // No block allocated
    read_sectors_lba(BLOCK_TO_LBA(block_num), GET_SECTORS_PER_BLOCK(), buffer);
}

void read_inode(uint32_t inode_num, inode_t* inode_struct) {
    if (inode_num <= INODE_TABLE_SIZE) {
        // Use cached inode
        *inode_struct = inode_table[inode_num - 1];
        return;
    }
    
    // Calculate the correct block and offset for this inode
    uint32_t inode_index = inode_num - 1;  // Inodes are 1-indexed, array is 0-indexed
    uint32_t inode_block = bgd_table.inode_table + (inode_index * INODE_SIZE) / GET_BLOCK_SIZE();
    uint32_t inode_offset = (inode_index * INODE_SIZE) % GET_BLOCK_SIZE();
    
    // Read the entire block containing this inode
    char* block_buffer = (char*)kmalloc(GET_BLOCK_SIZE());
    read_block(inode_block, block_buffer);
    
    // Extract the inode from the block
    memcpy((char*)inode_struct, block_buffer + inode_offset, INODE_SIZE);
    
    kfree(block_buffer);
}

// Helper function to read a block of data


// Helper function to read indirect blocks
void read_indirect_blocks(uint32_t indirect_block, char* buffer, uint32_t* buffer_offset, uint32_t max_size) {
    if (indirect_block == 0) return;
    
    uint32_t block_size = GET_BLOCK_SIZE();
    uint32_t pointers_per_block = GET_POINTERS_PER_BLOCK();
    
    // Read the indirect block (contains block pointers)
    uint32_t* block_pointers = (uint32_t*)kmalloc(block_size);
    read_block(indirect_block, (char*)block_pointers);
    
    // Read each data block pointed to by the indirect block
    for (uint32_t i = 0; i < pointers_per_block && *buffer_offset < max_size; i++) {
        if (block_pointers[i] != 0) {
            uint32_t bytes_to_read = (max_size - *buffer_offset > block_size) ? block_size : (max_size - *buffer_offset);
            read_block(block_pointers[i], buffer + *buffer_offset);
            *buffer_offset += bytes_to_read;
        }
    }
    
    kfree(block_pointers);
}

// Helper function to read doubly indirect blocks
void read_doubly_indirect_blocks(uint32_t doubly_indirect_block, char* buffer, uint32_t* buffer_offset, uint32_t max_size) {
    if (doubly_indirect_block == 0) return;
    
    uint32_t block_size = GET_BLOCK_SIZE();
    uint32_t pointers_per_block = GET_POINTERS_PER_BLOCK();
    
    // Read the doubly indirect block (contains pointers to singly indirect blocks)
    uint32_t* indirect_pointers = (uint32_t*)kmalloc(block_size);
    read_block(doubly_indirect_block, (char*)indirect_pointers);
    
    // Read each singly indirect block
    for (uint32_t i = 0; i < pointers_per_block && *buffer_offset < max_size; i++) {
        if (indirect_pointers[i] != 0) {
            read_indirect_blocks(indirect_pointers[i], buffer, buffer_offset, max_size);
        }
    }
    
    kfree(indirect_pointers);
}

// Helper function to read triply indirect blocks
void read_triply_indirect_blocks(uint32_t triply_indirect_block, char* buffer, uint32_t* buffer_offset, uint32_t max_size) {
    if (triply_indirect_block == 0) return;
    
    uint32_t block_size = GET_BLOCK_SIZE();
    uint32_t pointers_per_block = GET_POINTERS_PER_BLOCK();
    
    // Read the triply indirect block (contains pointers to doubly indirect blocks)
    uint32_t* doubly_indirect_pointers = (uint32_t*)kmalloc(block_size);
    read_block(triply_indirect_block, (char*)doubly_indirect_pointers);
    
    // Read each doubly indirect block
    for (uint32_t i = 0; i < pointers_per_block && *buffer_offset < max_size; i++) {
        if (doubly_indirect_pointers[i] != 0) {
            read_doubly_indirect_blocks(doubly_indirect_pointers[i], buffer, buffer_offset, max_size);
        }
    }
    
    kfree(doubly_indirect_pointers);
}

// Enhanced function to read all blocks from an inode
void read_inode_data(inode_t inode, char* buff_addr) {
    uint32_t file_size = inode.size_lower;
    uint32_t buffer_offset = 0;
    uint32_t block_size = GET_BLOCK_SIZE();
    
    // Read direct blocks (dptr0 - dptr11)
    for (int i = 0; i < 12 && buffer_offset < file_size; i++) {
        uint32_t* dptr = &inode.dptr0 + i; // Get pointer to dptr[i]
        if (*dptr != 0) {
            uint32_t bytes_to_read = (file_size - buffer_offset > block_size) ? block_size : (file_size - buffer_offset);
            read_block(*dptr, buff_addr + buffer_offset);
            buffer_offset += bytes_to_read;
        }
    }
    
    // Read singly indirect blocks
    if (inode.sing_idptr != 0 && buffer_offset < file_size) {
        read_indirect_blocks(inode.sing_idptr, buff_addr, &buffer_offset, file_size);
    }
    
    // Read doubly indirect blocks
    if (inode.doub_idptr != 0 && buffer_offset < file_size) {
        read_doubly_indirect_blocks(inode.doub_idptr, buff_addr, &buffer_offset, file_size);
    }
    
    // Read triply indirect blocks
    if (inode.trip_idptr != 0 && buffer_offset < file_size) {
        read_triply_indirect_blocks(inode.trip_idptr, buff_addr, &buffer_offset, file_size);
    }
}


void print_inode(inode_t inode) {
    kprintf("inode: type_perms=0x%x, uid=%d, size=%d, lacc_time=%d, create_time=%d, lmod_time=%d, delete_time=%d, gid=%d, hard_links=%d, sector_count=%d\n", 
            inode.type_perms, inode.uid, inode.size_lower, inode.lacc_time, inode.create_time, inode.lmod_time, inode.delete_time,
            inode.gid, inode.hard_links, inode.sector_count);
    kprintf("       flags=0x%x, dptr0=%d, dptr1=%d, dptr2=%d, dptr3=%d, dptr4=%d, dptr5=%d\n", 
            inode.flags, inode.dptr0, inode.dptr1, inode.dptr2, inode.dptr3, inode.dptr4, inode.dptr5);
    kprintf("       dptr6=%d, dptr7=%d, dptr8=%d, dptr9=%d, dptr10=%d, dptr11=%d\n", 
            inode.dptr6, inode.dptr7, inode.dptr8, inode.dptr9, inode.dptr10, inode.dptr11);
    kprintf("       sing_idptr=%d, doub_idptr=%d, trip_idptr=%d\n", 
            inode.sing_idptr, inode.doub_idptr, inode.trip_idptr);
}


// Updated read_dir_inode function
void read_dir_inode(inode_t inode, char* buff_addr){
    if(!(inode.type_perms & DIR)){
        print_inode(inode);
        panic("ext.h (read_dir_inode): cannot read contents unless type is DIR");
    }
    
    // Use the enhanced function to read all blocks
    read_inode_data(inode, buff_addr);
}

// Updated read_file_inode function
void read_file_inode(inode_t inode, char* buff_addr){
    if(!(inode.type_perms & FILE)){
        panic("ext.h (read_file_inode): cannot read contents unless type is FILE");
    }
    
    // Use the enhanced function to read all blocks
    read_inode_data(inode, buff_addr);
}


void read_dir_entry(dir_entry_t* dir_entry, char* buff_addr){
    // memcpy(buff_addr, (char*)dir_entry, 8);
    memcpy((char*)dir_entry, buff_addr, 8);
    dir_entry->name = (char*)kmalloc(dir_entry->name_lsb + 1);
    // memcpy(buff_addr + 8, dir_entry->name, dir_entry->name_lsb);
    memcpy(dir_entry->name, buff_addr + 8, dir_entry->name_lsb);
    dir_entry->name[dir_entry->name_lsb] = 0;
}

void print_dirlist(dir_list_node_t* dir_list_head){
    if(!dir_list_head){
        return;
    }

    dir_list_node_t* temp = dir_list_head;
    while(temp->next){
        kputs(temp->entry->name);
        temp = temp->next;
    }
}

// takes inode number of directory location (note inode table starts at index 1)
dir_list_node_t* read_directory(uint32_t inode_num){ // 0x951e

    // while(1);
    uint8_t* dir_contents;
    if(inode_num < 20){
        dir_contents = (uint8_t*)kmalloc(inode_table[inode_num - 1].size_lower);
        read_dir_inode(inode_table[inode_num - 1], dir_contents);
    }else{
        inode_t *tmp_inode = (inode_t*)kmalloc(sizeof(inode_t));
        read_sectors_lba(BLOCK_TO_LBA(inode_num), INODE_SIZE/SECTOR_SIZE, (char*)tmp_inode);
        dir_contents = (uint8_t*)kmalloc(tmp_inode->size_lower);
        read_dir_inode(*tmp_inode, dir_contents);
    }    

    dir_list_node_t* dir_list = (dir_list_node_t*)kmalloc(sizeof(dir_list_node_t*));
    dir_list_node_t* dir_list_head = dir_list;

    while(((dir_entry_t*)dir_contents)->inode != 0){ // 0x96b9
        // while(1);
        
        dir_entry_t* dir_entry = (dir_entry_t*)kmalloc(sizeof(dir_entry_t*)); // 0x95fa:      call   0x91db
        
        read_dir_entry(dir_entry, dir_contents);
        
        dir_contents += dir_entry->size;
        dir_list->entry = dir_entry;
        dir_list->next = (dir_list_node_t*)kmalloc(sizeof(dir_list_node_t*)); // call   0x91db
        dir_list = dir_list->next;   
        
        
    }


    
    // kfree(dir_list);
    // dir_list = 0; 

    return dir_list_head;
}


// Helper function to write sectors using LBA
void write_sectors_lba(uint32_t lba, uint16_t num_sectors, char* buff_addr){
    char* start = buff_addr;
    uint32_t lba_num = lba;
    for(uint16_t i = 0; i < num_sectors; i++){
        ata_lba_write(lba_num, 1, (char*)start);
        lba_num++;
        start += SECTOR_SIZE;
    }
}

void write_block(uint32_t block_num, char* buffer) {
    if (block_num == 0) return; // No block allocated
    write_sectors_lba(BLOCK_TO_LBA(block_num), GET_SECTORS_PER_BLOCK(), buffer);
}



// Write superblock back to disk
void write_superblock() {
    ata_lba_write(SUPER_BLOCK_LBA, SUPER_BLOCK_SIZE/SECTOR_SIZE, (char*)&superblock);
}

// Write block group descriptor table back to disk
void write_bgd_table() {
    ata_lba_write(BGD_TABLE_LBA, BGD_TABLE_SIZE/SECTOR_SIZE, (char*)&bgd_table);
}

// Write inode table back to disk
void write_inode_table() {
    write_sectors_lba(BLOCK_TO_LBA(bgd_table.inode_table), (INODE_TABLE_SIZE*INODE_SIZE)/SECTOR_SIZE, (char*)&inode_table);
}

// Write a specific inode back to disk
void write_inode(uint32_t inode_num, inode_t* inode_struct) {
    if (inode_num <= INODE_TABLE_SIZE) {
        // Update in-memory inode table
        inode_table[inode_num - 1] = *inode_struct;
        // Write entire inode table back (simple approach)
        write_inode_table();
        kprintf("write_inode: Updated cached inode %d, size=%d\n", inode_num, inode_struct->size_lower);
    } else {
        // Calculate the correct block and offset for this inode
        uint32_t inode_index = inode_num - 1;  // Inodes are 1-indexed, array is 0-indexed
        uint32_t inode_block = bgd_table.inode_table + (inode_index * INODE_SIZE) / GET_BLOCK_SIZE();
        uint32_t inode_offset = (inode_index * INODE_SIZE) % GET_BLOCK_SIZE();
        
        kprintf("write_inode: Writing inode %d to block %d, offset %d\n", inode_num, inode_block, inode_offset);
        
        // Read the entire block containing this inode
        char* block_buffer = (char*)kmalloc(GET_BLOCK_SIZE());
        read_block(inode_block, block_buffer);
        
        // Update the inode within the block
        memcpy(block_buffer + inode_offset, (char*)inode_struct, INODE_SIZE);
        
        // Write the block back to disk
        write_block(inode_block, block_buffer);
        
        kfree(block_buffer);
        kprintf("write_inode: Wrote inode %d to disk, size=%d\n", inode_num, inode_struct->size_lower);
    }
}

// Simple block allocation (finds first free block)
uint32_t allocate_block() {
    // TODO: Implement proper block bitmap checking
    // For now, return a hardcoded free block
    static uint32_t next_free_block = 100; // Start after metadata blocks
    return next_free_block++;
}

// Write data to inode blocks (handles direct blocks only for simplicity)
int write_inode_data(inode_t* inode, char* data, uint32_t size) {
    uint32_t block_size = GET_BLOCK_SIZE();
    uint32_t blocks_needed = (size + block_size - 1) / block_size; // Round up
    uint32_t data_offset = 0;
    
    // For simplicity, only handle direct blocks (up to 12 blocks)
    if (blocks_needed > 12) {
        kprintf("write_inode_data: File too large, need indirect blocks (not implemented)\n");
        return -1;
    }
    
    // Allocate and write direct blocks
    for (uint32_t i = 0; i < blocks_needed; i++) {
        uint32_t* dptr = &inode->dptr0 + i;
        
        // Allocate new block if not already allocated
        if (*dptr == 0) {
            *dptr = allocate_block();
            kprintf("Allocated block %d for direct pointer %d\n", *dptr, i);
        }
        
        // Prepare block data
        char* block_buffer = (char*)kmalloc(block_size);
        uint32_t bytes_to_write = (size - data_offset > block_size) ? block_size : (size - data_offset);
        
        // Copy data to block buffer
        for (uint32_t j = 0; j < bytes_to_write; j++) {
            block_buffer[j] = data[data_offset + j];
        }
        
        // Zero-fill remainder of block
        for (uint32_t j = bytes_to_write; j < block_size; j++) {
            block_buffer[j] = 0;
        }
        
        // Write block to disk
        write_block(*dptr, block_buffer);
        kprintf("Wrote %d bytes to block %d\n", bytes_to_write, *dptr);
        
        kfree(block_buffer);
        data_offset += bytes_to_write;
    }
    
    // Update inode size
    inode->size_lower = size;
    
    return 0;
}

uint32_t allocate_inode() {
    // TODO: Implement proper inode bitmap checking
    // Start after known existing files - test.txt is at inode 15
    static uint32_t next_free_inode = 25; // Start well after existing files
    return next_free_inode++;
}

// Add directory entry to a directory
int ext2_add_dir_entry(uint32_t dir_inode, uint32_t file_inode, const char* name, uint8_t type) {
    // Get directory inode
    inode_t directory;
    if (dir_inode <= INODE_TABLE_SIZE) {
        directory = inode_table[dir_inode - 1];
    } else {
        read_inode(dir_inode, &directory);
    }
    
    // Calculate new entry size
    uint8_t name_len = strlen(name);
    uint16_t entry_size = 8 + name_len;  // Basic entry + name
    if (entry_size % 4 != 0) {
        entry_size += 4 - (entry_size % 4);  // Align to 4 bytes
    }
    
    // For simplicity, just append to the first directory block
    // In a real implementation, you'd properly manage directory block space
    if (directory.dptr0 == 0) {
        // Allocate first directory block
        directory.dptr0 = allocate_block();
        directory.size_lower = GET_BLOCK_SIZE();
        
        // Create initial directory content with "." and ".." entries
        char* dir_block = (char*)kmalloc(GET_BLOCK_SIZE());
        memset(dir_block, 0, GET_BLOCK_SIZE());
        
        // Add "." entry
        dir_entry_t* dot_entry = (dir_entry_t*)dir_block;
        dot_entry->inode = dir_inode;
        dot_entry->size = 12;  // Minimum size for "." entry
        dot_entry->name_lsb = 1;
        dot_entry->type = D_DIR;
        dir_block[8] = '.';
        
        // Add ".." entry  
        dir_entry_t* dotdot_entry = (dir_entry_t*)(dir_block + 12);
        dotdot_entry->inode = dir_inode;  // TODO: Should be parent inode
        dotdot_entry->size = GET_BLOCK_SIZE() - 12 - entry_size;  // Rest of block minus our new entry
        dotdot_entry->name_lsb = 2;
        dotdot_entry->type = D_DIR;
        dir_block[20] = '.';
        dir_block[21] = '.';
        
        // Add new file entry at the end
        uint32_t offset = 12 + dotdot_entry->size;
        dir_entry_t* new_entry = (dir_entry_t*)(dir_block + offset);
        new_entry->inode = file_inode;
        new_entry->size = entry_size;
        new_entry->name_lsb = name_len;
        new_entry->type = type;
        memcpy(dir_block + offset + 8, name, name_len);
        
        // Adjust previous entry size
        dotdot_entry->size = offset - 12;
        
        write_block(directory.dptr0, dir_block);
        kfree(dir_block);
    } else {
        // Directory block exists, append to it
        // This is a simplified implementation - normally you'd need to parse existing entries
        char* dir_block = (char*)kmalloc(GET_BLOCK_SIZE());
        read_block(directory.dptr0, dir_block);
        
        // Find end of existing entries (simplified - assumes we can append at the end)
        uint32_t offset = directory.size_lower % GET_BLOCK_SIZE();
        if (offset + entry_size > GET_BLOCK_SIZE()) {
            kputs("ext2_add_dir_entry: Directory block full (need to allocate new block)");
            kfree(dir_block);
            return -1;
        }
        
        // Add new entry
        dir_entry_t* new_entry = (dir_entry_t*)(dir_block + offset);
        new_entry->inode = file_inode;
        new_entry->size = entry_size;
        new_entry->name_lsb = name_len;
        new_entry->type = type;
        memcpy(dir_block + offset + 8, name, name_len);
        
        write_block(directory.dptr0, dir_block);
        kfree(dir_block);
    }
    
    // Update directory inode
    write_inode(dir_inode, &directory);
    
    return 0;
}


// Create a new file in ext2 filesystem
uint32_t ext2_create_file(uint32_t parent_inode, const char* filename, uint32_t mode) {
    // Allocate new inode
    uint32_t new_inode_num = allocate_inode();
    
    // Create inode structure
    inode_t new_inode = {0};
    new_inode.type_perms = FILE | U_RD | U_WR | G_RD | O_RD; // Regular file with 644 permissions
    new_inode.uid = 0;
    new_inode.size_lower = 0;  // Empty file initially
    new_inode.lacc_time = 0;   // TODO: Add proper timestamp
    new_inode.create_time = 0;
    new_inode.lmod_time = 0;
    new_inode.delete_time = 0;
    new_inode.gid = 0;
    new_inode.hard_links = 1;
    new_inode.sector_count = 0;
    new_inode.flags = 0;
    new_inode.os_spec1 = 0;
    
    // Initialize all block pointers to 0 (no blocks allocated yet)
    new_inode.dptr0 = 0;
    new_inode.dptr1 = 0;
    new_inode.dptr2 = 0;
    new_inode.dptr3 = 0;
    new_inode.dptr4 = 0;
    new_inode.dptr5 = 0;
    new_inode.dptr6 = 0;
    new_inode.dptr7 = 0;
    new_inode.dptr8 = 0;
    new_inode.dptr9 = 0;
    new_inode.dptr10 = 0;
    new_inode.dptr11 = 0;
    new_inode.sing_idptr = 0;
    new_inode.doub_idptr = 0;
    new_inode.trip_idptr = 0;
    
    new_inode.gen_number = 0;
    new_inode.ext_attr = 0;
    new_inode.size_upper = 0;
    new_inode.frag_addr = 0;
    
    // Write inode to disk
    write_inode(new_inode_num, &new_inode);
    
    // Add directory entry to parent directory
    if (ext2_add_dir_entry(parent_inode, new_inode_num, filename, D_FILE) != 0) {
        kputs("ext2_create_file: Failed to add directory entry");
        return 0;
    }
    
    kprintf("ext2_create_file: Created file '%s' with inode %d\n", filename, new_inode_num);
    return new_inode_num;
}



// Create directory
uint32_t ext2_create_dir(uint32_t parent_inode, const char* dirname, uint32_t mode) {
    // Allocate new inode
    uint32_t new_inode_num = allocate_inode();
    
    // Create directory inode
    inode_t new_inode = {0};
    new_inode.type_perms = DIR | U_RD | U_WR | U_EX | G_RD | G_EX | O_RD | O_EX; // Directory with 755 permissions
    new_inode.uid = 0;
    new_inode.size_lower = GET_BLOCK_SIZE();  // One block for directory
    new_inode.lacc_time = 0;
    new_inode.create_time = 0;
    new_inode.lmod_time = 0;
    new_inode.delete_time = 0;
    new_inode.gid = 0;
    new_inode.hard_links = 2;  // "." and parent reference
    new_inode.sector_count = GET_SECTORS_PER_BLOCK();
    new_inode.flags = 0;
    
    // Allocate directory block
    new_inode.dptr0 = allocate_block();
    
    // Initialize all other pointers to 0
    new_inode.dptr1 = 0;
    // ... (initialize rest)
    
    // Write inode to disk
    write_inode(new_inode_num, &new_inode);
    
    // Create initial directory content
    char* dir_block = (char*)kmalloc(GET_BLOCK_SIZE());
    memset(dir_block, 0, GET_BLOCK_SIZE());
    
    // Add "." entry
    dir_entry_t* dot_entry = (dir_entry_t*)dir_block;
    dot_entry->inode = new_inode_num;
    dot_entry->size = 12;
    dot_entry->name_lsb = 1;
    dot_entry->type = D_DIR;
    dir_block[8] = '.';
    
    // Add ".." entry
    dir_entry_t* dotdot_entry = (dir_entry_t*)(dir_block + 12);
    dotdot_entry->inode = parent_inode;
    dotdot_entry->size = GET_BLOCK_SIZE() - 12;  // Rest of block
    dotdot_entry->name_lsb = 2;
    dotdot_entry->type = D_DIR;
    dir_block[20] = '.';
    dir_block[21] = '.';
    
    write_block(new_inode.dptr0, dir_block);
    kfree(dir_block);
    
    // Add directory entry to parent
    if (ext2_add_dir_entry(parent_inode, new_inode_num, dirname, D_DIR) != 0) {
        kputs("ext2_create_dir: Failed to add directory entry");
        return 0;
    }
    
    return new_inode_num;
}


// TODO
// This is a simplified implementation that:
// 1. Only handles direct blocks (files up to 48KB)
// 2. Uses simple block allocation without bitmap checking
// 3. Doesn't handle block deallocation
// 4. Doesn't update block usage bitmaps
// For Future:
// 1. Block Bitmap Management: Read/write block allocation bitmaps
// 2. Indirect Block Support: Handle singly/doubly/triply indirect blocks
// 3. Proper Error Handling: Handle disk write failures
// 4. Block Deallocation: Free unused blocks when files shrink
// 5. Directory Updates: Update directory entries for new files
// 6. Journaling: Add transaction support for reliability



