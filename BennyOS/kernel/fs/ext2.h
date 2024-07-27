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


