/*
Second Extended Filesystem (ext2fs) is a rewrite of the original Extended Filesystem and as such, is also based around the concept of "inodes."
It has native support for UNIX ownership / access rights, symbolic- and hard-links, and other properties that are common among UNIX-like operating systems.
Organizationally, it divides disk space up into groups called "block groups."


Block: fs divides the disk space into logical blocks of contiguous space
Block groups: contiguous groups of blocks, some of the blocks are used for metadata
Inode: structure on the disk that represents a file, dir, or sym-link; not an actual file but link to block with the actual file



*/

#define SUPER_BLOCK_LBA     1
#define SUPER_BLOCK_SIZE    512
#define BLOCK_SIZE          512


typedef struct {
    uint16_t cylinder;
    uint8_t head;
    uint8_t sector;
} __attribute__((packed)) chs_t;


extern void ata_chs_read(chs_t chs, uint16_t num_sectors, char* buff_addr);


typedef struct {
    uint32_t num_inodes;
    uint32_t num_blocks;
    uint32_t num_reserved_blocks;
    uint32_t num_unalloc_blocks;
    uint32_t num_unalloc_inodes;
    uint32_t superblock_blocknum;
    uint32_t block_size;
    uint32_t fragment_size;
    uint32_t num_blocks_blockgroup;
    uint32_t num_frags_blockgroup;
    uint32_t num_inodes_blockgroup;
    uint32_t last_mount;
    uint32_t last_write;
    uint16_t mounts_since_cc;
    uint16_t mounts_before_cc;
    uint16_t signature;
    uint16_t fs_state;
    uint16_t error;
    uint16_t ver_minor;
    uint32_t last_cc;
    uint32_t int_force_cc;
    uint32_t os_id;
    uint32_t ver_major;
    uint16_t reserve_user;
    uint16_t reserve_group;

} __attribute__((packed)) super_block;


