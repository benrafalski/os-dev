#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "ext2.h"

super_block sb = {0};

int main(){

    FILE *IMAGE_PTR = fopen("../bin/fs.bin", "wb");
    // make superblock
    sb.num_inodes = 0;
    sb.num_blocks = 0;
    sb.num_reserved_blocks = 0;
    sb.num_unalloc_blocks = 0;
    sb.num_unalloc_inodes = 0;
    sb.superblock_blocknum = 0;
    sb.block_size = 0;
    sb.fragment_size = 0;
    sb.num_blocks_blockgroup = 0;
    sb.num_frags_blockgroup = 0;
    sb.num_inodes_blockgroup = 0;
    sb.last_mount = 0;
    sb.last_write = 0;
    sb.mounts_since_cc = 0;
    sb.mounts_before_cc = 0;
    sb.signature = 0xEF53;
    sb.fs_state = 0;
    sb.error = 0;
    sb.ver_minor = 0;
    sb.last_cc = 0;
    sb.int_force_cc = 0;
    sb.os_id = 0;
    sb.ver_major = 0;
    sb.reserve_user = 0;
    sb.reserve_group = 0;
    fwrite(&sb, sizeof(super_block), 1, IMAGE_PTR);
    char* null[512];
    memset(null, '\0', sizeof(null));
    fwrite(&null, BLOCK_SIZE - sizeof(super_block), 1, IMAGE_PTR);

    

    return 0;
}