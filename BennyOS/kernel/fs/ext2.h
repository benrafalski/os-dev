/*
https://wiki.osdev.org/Ext2

Second Extended Filesystem (ext2fs) is a rewrite of the original Extended Filesystem and as such, is also based around the concept of "inodes."
It has native support for UNIX ownership / access rights, symbolic- and hard-links, and other properties that are common among UNIX-like operating systems.
Organizationally, it divides disk space up into groups called "block groups."


Block: fs divides the disk space into logical blocks of contiguous space
Block groups: contiguous groups of blocks, some of the blocks are used for metadata
Inode: structure on the disk that represents a file, dir, or sym-link; not an actual file but link to block with the actual file



*/

#define SUPER_BLOCK_LBA     2
#define SUPER_BLOCK_SIZE    512
#define SECTOR_SIZE         512
#define SECTORS_PER_DPTR    4


#define BGD_TABLE_LBA       8
#define BGD_TABLE_SIZE      512

#define INODE_TABLE_SIZE    20
#define INODE_SIZE          128

#define LBA_4096(n) (n * 8)


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
    uint32_t    dptr0;                      // Direct Block Pointer 0
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
    uint32_t    sing_idptr;                 // singly indirect block pointer
    uint32_t    doub_idptr;                 // doubly indirect block pointer
    uint32_t    trip_idptr;                 // triply indirect block pointer
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
    // uint32_t    num_inodes;                 // 0x00008000
    // uint32_t    num_blocks;                 // 0x00008000
    // uint32_t    num_reserved_blocks;        // 0x00000666
    // uint32_t    num_unalloc_blocks;         // 0x00007be8
    // uint32_t    num_unalloc_inodes;         // 0x00007ff4
    // uint32_t    superblock_blocknum;        // 0x00000000
    // uint32_t    block_size;                 // 0x00000002 -> 4096 is block size
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
bgd_table_t bgd_table = {0};
    // uint32_t    blk_use_bitmap;             // 0x00000009
    // uint32_t    inode_use_bitmap;           // 0x0000000a
    // uint32_t    inode_table;                // 0x0000000b
    // uint16_t    unalloc_blocks;             // 0x7ff47be8
    // uint16_t    unalloc_inodes;             // 0x00000002
    // uint8_t     unused[14];                 // padding

inode_t inode_table[INODE_TABLE_SIZE] = {0};
    // 0xb300
    // 0000b500
    // 0000b580

    // inode[0] at lba 88 -> nothing address is 0x100e0 in memory
    // inode[1] at lba 88.25 -> root
        // type 41ed -> DIR, U_RD, G_RD, U_EX, U_WR, G_EX, O_RD, O_EX  
        // sector count 0x00000008
        // osspec 0x00000002
        // dptr0 0x0000040b
    // inode[2] at lba 89.5 -> no clue
        // type 0x8180 -> U_WR|U_RD|FILE
        // sector count = 0x00000040
        // doubly pointer = 0x00000410
    // inode[3] at lba 90.5 -> lost+found
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

// root
// entry 1
// inode = 0x00000002
// size = 0x000c
// name lsb = 0x02
// 


// extract the size of each block, 
// the total number of inodes, 
// the total number of blocks, 
// the number of blocks per block group, 
// and the number of inodes in each block group

void read_superblock(){
    ata_lba_read(SUPER_BLOCK_LBA, SUPER_BLOCK_SIZE/SECTOR_SIZE, (char*)&superblock);
    if(superblock.signature != 0xEF53) {
        panic("Incorrect ext2 signature...");
    }

    if(superblock.fs_state != 1){
        panic("Filesystem has errors...");
    }

    // kprintf("inodesize: %d\n", superblock.inode_size);
}

void read_bgd_table(){
    ata_lba_read(BGD_TABLE_LBA, BGD_TABLE_SIZE/SECTOR_SIZE, (char*)&bgd_table);
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
    read_sectors_lba(LBA_4096(bgd_table.inode_table), (INODE_TABLE_SIZE*INODE_SIZE)/SECTOR_SIZE, (char*)&inode_table);
}


uint32_t get_inode_size(inode_t inode){
    return (inode.size_lower < SECTOR_SIZE ? SECTOR_SIZE : inode.size_lower);
}

void read_inode(uint32_t inode, inode_t* inode_struct){
    read_sectors_lba(LBA_4096(inode), INODE_SIZE/SECTOR_SIZE, (char*)inode_struct);
}


void read_file_inode(inode_t inode, char* buff_addr){
    if(!(inode.type_perms & FILE)){
        panic("ext.h (read_file_inode): cannot read contents unless type is FILE");
    }

    uint32_t size = get_inode_size(inode);

    if(inode.dptr0){
        read_sectors_lba(LBA_4096(inode.dptr0), size/SECTOR_SIZE, (char*)buff_addr);
    }

}

void read_dir_inode(inode_t inode, char* buff_addr){
    if(!(inode.type_perms & DIR)){
        panic("ext.h (read_dir_inode): cannot read contents unless type is DIR");
    }

    if(inode.dptr0){
        read_sectors_lba(LBA_4096(inode.dptr0), 2, (char*)buff_addr);
    }

}

void read_dir_entry(dir_entry_t* dir_entry, char* buff_addr){
    memcpy(buff_addr, (char*)dir_entry, 8);
    dir_entry->name = (char*)kmalloc(dir_entry->name_lsb + 1);
    memcpy(buff_addr + 8, dir_entry->name, dir_entry->name_lsb);
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
        read_sectors_lba(LBA_4096(inode_num), INODE_SIZE/SECTOR_SIZE, (char*)tmp_inode);
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



void ext2_read(){

}





