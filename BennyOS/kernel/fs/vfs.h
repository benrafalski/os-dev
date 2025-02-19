#include "ext2.h"


// current working directory
dir_list_node_t* cwd;
dir_list_node_t* root_dir;


// returns inode where "name" is located in "dir"
uint32_t vfs_find(char* name, dir_list_node_t* dir, uint32_t type){
    if(!dir){
        return 0;
    }

    char* filename = name;
    uint8_t isdir = 0;

    if(strends_with(filename, '/') && strstarts_with(filename, '/')){
        // kputs("here");
        isdir = 1;
        // return 0;
    }

    uint32_t n = strsplit(name, '/');
    if(isdir) n--;

    while(*name==0){
        name++;
    }

    if(n == 0) n = 1;

    dir_list_node_t* temp = dir;
    for(int i = 0; i < n; i++){
        if(i < (n - 1)){
            while(temp->next){
                if(temp->entry->type & D_DIR){
                    if(strcmp(name, temp->entry->name)){
                        temp = read_directory(temp->entry->inode);
                        break;
                    }         
                }
                temp = temp->next;
            }
        }else if(i == (n-1)){
            while(temp->next){
                if(temp->entry->type & type){
                    if(strcmp(name, temp->entry->name)){
                        // kprintf("Found at inode %d\n", temp->entry->inode);
                        return temp->entry->inode;
                    }         
                }
                temp = temp->next;
            } 
        }

        while(*name){
            name++;
        }
        name++;
    }

    return 0;
}


void vfs_cd(char* name){
    uint32_t inode;

    if(strcmp(name, "/")){
        cwd = root_dir;
        return;
    }

    if(*name == '/'){
        inode = vfs_find(name, root_dir, D_DIR);
    }else{
        inode = vfs_find(name, cwd, D_DIR);
    }

    if(inode > 0){
        cwd = read_directory(inode);
    }else{
        kputs("Directory doesn't exist...");
    }
}

char* vfs_read_file(char* name){
    dir_list_node_t* search_dir;
    char* buff;
    if(strstarts_with(name, '/')){
        search_dir = root_dir;
    }else{
        search_dir = cwd;
    }

    uint32_t inode = vfs_find(name, search_dir, D_FILE);
    if(inode){
        if(inode < 20){
            uint32_t size = get_inode_size(inode_table[inode-1]);
            buff = (char*)kmalloc(size);
            read_file_inode(inode_table[inode-1], buff);
        }else{
            kputs("File not in inode table");
            return 0;
        }
    }else{
        inode = vfs_find(name, search_dir, D_DIR);
        if(inode){
            kputs("You have provided a directory name.");
        }else{
            kputs("File not found.");
        }
        return 0;
    }
    return buff;
}


dir_list_node_t* mount_dir(uint32_t inode){
    dir_list_node_t* dir = read_directory(inode);
    if(!dir){
        return 0;
    }

    dir_list_node_t* temp = dir;
    while(temp->next){
        if(temp->entry->type & D_FILE){
            // kputs("found a file");
            // kputs(temp->entry->name);
        }else if(temp->entry->type & D_DIR){
            // kputs("found a directory");
            // kputs(temp->entry->name);
            if(!strcmp(".", temp->entry->name) && !strcmp("..", temp->entry->name)){
                temp->children = read_directory(temp->entry->inode); 
            }         
        }
        temp = temp->next;
    }
    return dir;
}

void mount_root_dir(){
    root_dir = mount_dir(2);
}

void init_vfs(){
    // +----------+
    // |Filesystem|
    // |using ext2|
    // +----------+
    // read the superblock, bgd table, and inode table from disk
    read_superblock();
    read_bgd_table();
    read_inode_table();
    // read the root directory, always at inode 2
    mount_root_dir();
    cwd = root_dir;
}



