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


#define BGD_TABLE_LBA       8
#define BGD_TABLE_SIZE      512


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
}

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
    uint8_t     os_spec2[12]                // OS specific value 2
} __attribute__((packed)) inode_t;


