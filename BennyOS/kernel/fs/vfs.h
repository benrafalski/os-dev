#include "ext2.h"



typedef struct vfs_node {
    char* name;
    uint32_t inode;
    uint8_t type;
    struct vfs_node* parent;
    struct vfs_node* children;
    struct vfs_node* next;
    void* fs_specific; // Points to ext2-specific data
} vfs_node_t;

typedef struct mounted_fs {
    char* mount_point;
    vfs_node_t* root_node;
    // Filesystem-specific operations
    struct fs_operations* ops;
} mounted_fs_t;

#define MAX_MOUNTED_FS 10
mounted_fs_t mounted_filesystems[MAX_MOUNTED_FS];
int num_mounted_fs = 0;
vfs_node_t* vfs_root = NULL;


// Convert ext2 directory structure to VFS nodes (non-recursive)
vfs_node_t* convert_ext2_to_vfs(dir_list_node_t* ext2_dir) {
    if (!ext2_dir) return NULL;
    
    vfs_node_t* vfs_head = NULL;
    vfs_node_t* vfs_current = NULL;
    dir_list_node_t* temp = ext2_dir;
    
    while (temp && temp->entry) {
        // Skip "." and ".." entries for now
        if (strcmp(temp->entry->name, ".") && strcmp(temp->entry->name, "..")) {
            vfs_node_t* node = (vfs_node_t*)kmalloc(sizeof(vfs_node_t));
            node->name = (char*)kmalloc(temp->entry->name_lsb + 1);
            memcpy(temp->entry->name, node->name, temp->entry->name_lsb);
            node->name[temp->entry->name_lsb] = '\0';
            // kprintf("Read node name: %s (inode=%p)\n", node->name, node);
            
            node->inode = temp->entry->inode;
            node->type = temp->entry->type;
            node->children = NULL; // Load on-demand
            node->next = NULL;


            
            if (!vfs_head) {
                vfs_head = node;
                vfs_current = node;
            } else {
                vfs_current->next = node;
                vfs_current = node;
            }
        }
        temp = temp->next;
    }
    
    return vfs_head;
}


int register_mounted_fs(const char* mount_point, vfs_node_t* root_node) {
    // kprintf("Registering filesystem: mount_point=%s, root_node=%p\n", mount_point, root_node);
    
    

    if (num_mounted_fs >= MAX_MOUNTED_FS) {
        kputs("Too many mounted filesystems");
        return -1;
    }
    
    mounted_filesystems[num_mounted_fs].mount_point = (char*)kmalloc(strlen(mount_point) + 1);
    if (!mounted_filesystems[num_mounted_fs].mount_point) {
        kputs("Failed to allocate mount point string");
        return -1;
    }
    
    strcpy(mounted_filesystems[num_mounted_fs].mount_point, mount_point);
    mounted_filesystems[num_mounted_fs].root_node = root_node;
    
    // If this is the root mount, set it as the global root
    if (strcmp(mount_point, "/") == 0) {
        // kprintf("Setting vfs_root to %p\n", root_node);
        vfs_root = root_node;
    }
    
    num_mounted_fs++;
    // kprintf("num_mounted_fs is now %d\n", num_mounted_fs);
    return 0;
}

int vfs_mount_ext2(const char* mount_point, uint32_t root_inode) {
    // kprintf("Mounting ext2 at %s with inode %d\n", mount_point, root_inode);
    
    // 1. Create VFS node for the mount point
    vfs_node_t* mount_node = (vfs_node_t*)kmalloc(sizeof(vfs_node_t));
    if (!mount_node) {
        kputs("Failed to allocate mount_node");
        return -1;
    }
    
    mount_node->name = (char*)kmalloc(strlen(mount_point) + 1);
    if (!mount_node->name) {
        kputs("Failed to allocate mount_node->name");
        kfree(mount_node);
        return -1;
    }
    
    strcpy(mount_node->name, mount_point);
    mount_node->inode = root_inode;
    mount_node->type = D_DIR;
    mount_node->parent = NULL;
    mount_node->children = NULL;
    mount_node->next = NULL;
    mount_node->fs_specific = NULL;
    
    // kprintf("Created mount_node at %p\n", mount_node);
    
    // 2. Load the root directory of the filesystem
    dir_list_node_t* ext2_root = read_directory(root_inode);
    if (!ext2_root) {
        kputs("Failed to read root directory");
        kfree(mount_node->name);
        kfree(mount_node);
        return -1;
    }
    
    // kprintf("Read ext2_root at %p\n", ext2_root);
    
    // 3. Convert ext2 structure to VFS structure
    mount_node->children = convert_ext2_to_vfs(ext2_root);
    // kprintf("Converted children, mount_node->children = %p\n", mount_node->children);
    
    // 4. Register the mounted filesystem
    int result = register_mounted_fs(mount_point, mount_node);
    // kprintf("register_mounted_fs returned %d\n", result);
    // kprintf("vfs_root is now %p\n", vfs_root);
    
    return result;
}

void vfs_load_children(vfs_node_t* dir_node) {
    if (!dir_node || dir_node->type != D_DIR) {
        return;
    }
    
    // If children are already loaded, don't reload
    if (dir_node->children != NULL) {
        return;
    }
    
    // Load the directory contents from the filesystem
    dir_list_node_t* ext2_dir = read_directory(dir_node->inode);
    if (!ext2_dir) {
        kprintf("Failed to read directory with inode %d\n", dir_node->inode);
        return;
    }
    
    // Convert ext2 structure to VFS structure
    dir_node->children = convert_ext2_to_vfs(ext2_dir);
    
    // kprintf("Loaded %s directory children\n", dir_node->name);
}

void vfs_list_directory(vfs_node_t* dir_node) {
    if (!dir_node || dir_node->type != D_DIR) {
        kputs("Not a directory");
        return;
    }
    
    // Load children if not already loaded
    vfs_load_children(dir_node);
    
    vfs_node_t* child = dir_node->children;
    if (!child) {
        kputs("Directory is empty or failed to load");
        return;
    }
    
    kprintf("Contents of directory '%s':\n", dir_node->name);
    while (child) {
        kprintf("  %s (inode: %d, type: %d)\n", child->name, child->inode, child->type);
        child = child->next;
    }
    kputs("");
}

// Updated find function that loads children on-demand
vfs_node_t* vfs_find_node(char* path, vfs_node_t* start_node) {
    if (!start_node) start_node = vfs_root;
    if (!path || !start_node) return NULL;
    
    // Handle root path
    if (strcmp(path, "/") == 0) {
        return vfs_root;
    }
    
    // Skip leading slash
    if (*path == '/') path++;
    
    // Load children if not already loaded
    vfs_load_children(start_node);
    
    // Simple implementation - find immediate child
    vfs_node_t* current = start_node->children;
    while (current) {
        if (strcmp(current->name, path) == 0) {
            return current;
        }
        current = current->next;
    }
    
    return NULL;
}

// Helper function to find a node by name in the root directory
vfs_node_t* vfs_find_child(vfs_node_t* parent, const char* name) {
    if (!parent || !name) return NULL;
    
    vfs_load_children(parent);
    
    vfs_node_t* child = parent->children;
    while (child) {
        if (strcmp(child->name, name) == 0) {
            return child;
        }
        child = child->next;
    }
    return NULL;
}

void init_vfs(){
    // Initialize globals
    num_mounted_fs = 0;
    vfs_root = NULL;
    
    // read the superblock, bgd table, and inode table from disk
    read_superblock();
    read_bgd_table();
    read_inode_table();
    
    // read the root directory, always at inode 2
    if (vfs_mount_ext2("/", 2) == 0) {
        kputs("Successfully mounted ext2 filesystem at /");
    } else {
        kputs("Failed to mount ext2 filesystem");
        return;
    }

    // List root directory
    vfs_list_directory(vfs_root);
    
    // Find and list the "first" directory
    vfs_node_t* first_dir = vfs_find_child(vfs_root, "first");
    if (first_dir) {
        vfs_list_directory(first_dir);
        
        // Find and list the "second" directory inside "first"
        vfs_node_t* second_dir = vfs_find_child(first_dir, "second");
        if (second_dir) {
            vfs_list_directory(second_dir);
        } else {
            kputs("Could not find 'second' directory inside 'first'");
        }
    } else {
        kputs("Could not find 'first' directory");
    }
}



