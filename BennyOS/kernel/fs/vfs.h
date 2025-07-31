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

// File descriptor structure
typedef struct vfs_file {
    vfs_node_t* node;           // Pointer to the VFS node
    uint32_t position;          // Current file position
    uint32_t flags;             // Open flags
    uint32_t mode;              // File mode
    uint8_t* buffer;            // File content buffer (for caching)
    uint32_t buffer_size;       // Size of loaded buffer
    uint8_t is_open;            // File open status
} vfs_file_t;

// File open flags
#define VFS_O_RDONLY    0x00    // Read only
#define VFS_O_WRONLY    0x01    // Write only
#define VFS_O_RDWR      0x02    // Read/Write
#define VFS_O_CREAT     0x04    // Create if doesn't exist
#define VFS_O_TRUNC     0x08    // Truncate to zero length
#define VFS_O_APPEND    0x10    // Append mode

// Maximum open files
#define MAX_OPEN_FILES 32
vfs_file_t open_files[MAX_OPEN_FILES];

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
            // memcpy(temp->entry->name, node->name, temp->entry->name_lsb);
            memcpy(node->name, temp->entry->name, temp->entry->name_lsb);
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

// Initialize file descriptor table
void vfs_init_file_table() {
    for (int i = 0; i < MAX_OPEN_FILES; i++) {
        open_files[i].node = NULL;
        open_files[i].position = 0;
        open_files[i].flags = 0;
        open_files[i].mode = 0;
        open_files[i].buffer = NULL;
        open_files[i].buffer_size = 0;
        open_files[i].is_open = 0;
    }
}

// Find an available file descriptor
int vfs_alloc_fd() {
    for (int i = 0; i < MAX_OPEN_FILES; i++) {
        if (!open_files[i].is_open) {
            return i;
        }
    }
    return -1; // No available file descriptors
}

// Helper function to resolve a full path to a VFS node
vfs_node_t* vfs_resolve_path(const char* path) {
    if (!path || !vfs_root) {
        return NULL;
    }
    
    // Handle root path
    if (strcmp(path, "/") == 0) {
        return vfs_root;
    }
    
    // Make a copy of the path for tokenization
    char* path_copy = (char*)kmalloc(strlen(path) + 1);
    strcpy(path_copy, path);
    
    vfs_node_t* current = vfs_root;
    char* token = strtok(path_copy, "/");
    
    while (token && current) {
        // Load children if it's a directory
        if (current->type == D_DIR) {
            vfs_load_children(current);
        }
        
        // Find the next component
        vfs_node_t* child = current->children;
        vfs_node_t* found = NULL;
        
        while (child) {
            if (strcmp(child->name, token) == 0) {
                found = child;
                break;
            }
            child = child->next;
        }
        
        if (!found) {
            kfree(path_copy);
            return NULL; // Path component not found
        }
        
        current = found;
        token = strtok(NULL, "/");
    }
    
    kfree(path_copy);
    return current;
}

// Helper function to load file content from ext2
int vfs_load_file_content(vfs_node_t* file_node, uint8_t** buffer, uint32_t* size) {
    if (!file_node || file_node->type != D_FILE) {
        return -1;
    }

    // Get inode information - Fix the boundary check!
    inode_t file_inode;
    if (file_node->inode <= INODE_TABLE_SIZE) {  // Changed from < 20 to <= INODE_TABLE_SIZE
        file_inode = inode_table[file_node->inode - 1];
        kprintf("vfs_load_file_content: Using cached inode %d, size=%d\n", 
                file_node->inode, file_inode.size_lower);
    } else {
        read_inode(file_node->inode, &file_inode);
        kprintf("vfs_load_file_content: Read fresh inode %d from disk, size=%d\n", 
                file_node->inode, file_inode.size_lower);
    }
    
    *size = file_inode.size_lower;
    
    if (*size == 0) {
        kprintf("vfs_load_file_content: File has zero size, allocating minimal buffer\n");
        *buffer = (uint8_t*)kmalloc(1);
        return 0;
    }
    
    *buffer = (uint8_t*)kmalloc(*size);
    
    if (!*buffer) {
        kprintf("vfs_load_file_content: Failed to allocate buffer for %d bytes\n", *size);
        return -1;
    }
    
    // Read file content using ext2 function
    read_file_inode(file_inode, (char*)*buffer);
    
    kprintf("vfs_load_file_content: Successfully loaded %d bytes\n", *size);
    return 0;
}


// File creation function
int vfs_create(const char* path, uint32_t mode) {
    if (!path) {
        kputs("vfs_create: Invalid path");
        return -1;
    }
    
    // Parse the path to get directory and filename
    char* path_copy = (char*)kmalloc(strlen(path) + 1);
    strcpy(path_copy, path);
    
    // Find the last '/' to separate directory from filename
    char* filename = NULL;
    char* last_slash = NULL;
    
    for (int i = strlen(path_copy) - 1; i >= 0; i--) {
        if (path_copy[i] == '/') {
            last_slash = &path_copy[i];
            break;
        }
    }
    
    vfs_node_t* parent_dir;
    
    if (last_slash) {
        *last_slash = '\0';  // Terminate directory path
        filename = last_slash + 1;
        parent_dir = vfs_resolve_path(path_copy[0] ? path_copy : "/");
    } else {
        filename = path_copy;
        parent_dir = vfs_root;  // Current directory is root
    }
    
    if (!parent_dir || parent_dir->type != D_DIR) {
        kputs("vfs_create: Parent directory not found");
        kfree(path_copy);
        return -1;
    }
    
    // Check if file already exists
    vfs_load_children(parent_dir);
    vfs_node_t* existing = vfs_find_child(parent_dir, filename);
    if (existing) {
        kputs("vfs_create: File already exists");
        kfree(path_copy);
        return -1;
    }
    
    // Create new inode and file on disk
    uint32_t new_inode = ext2_create_file(parent_dir->inode, filename, mode);
    if (new_inode == 0) {
        kputs("vfs_create: Failed to create file on disk");
        kfree(path_copy);
        return -1;
    }
    
    // Create VFS node
    vfs_node_t* new_node = (vfs_node_t*)kmalloc(sizeof(vfs_node_t));
    new_node->name = (char*)kmalloc(strlen(filename) + 1);
    strcpy(new_node->name, filename);
    new_node->inode = new_inode;
    new_node->type = D_FILE;
    new_node->parent = parent_dir;
    new_node->children = NULL;
    new_node->next = parent_dir->children;  // Add to front of children list
    new_node->fs_specific = NULL;
    
    parent_dir->children = new_node;
    
    kprintf("vfs_create: Created file %s with inode %d\n", path, new_inode);
    kfree(path_copy);
    return 0;
}

// VFS open function
int vfs_open(const char* path, uint32_t flags, uint32_t mode) {
    if (!path) {
        kputs("vfs_open: Invalid path");
        return -1;
    }
    
    // Find available file descriptor
    int fd = vfs_alloc_fd();
    if (fd == -1) {
        kputs("vfs_open: No available file descriptors");
        return -1;
    }
    
    // Parse the path to find the file
    vfs_node_t* file_node = vfs_resolve_path(path);
    
    if (!file_node) {
        if (flags & VFS_O_CREAT) {
            // Create the file
            if (vfs_create(path, mode) != 0) {
                kprintf("vfs_open: Failed to create file: %s\n", path);
                return -1;
            }
            
            // Try to resolve the path again
            file_node = vfs_resolve_path(path);
            if (!file_node) {
                kprintf("vfs_open: Created file but failed to resolve: %s\n", path);
                return -1;
            }
        } else {
            kprintf("vfs_open: File not found: %s\n", path);
            return -1;
        }
    }
    
    // Check if it's a file (not a directory)
    if (file_node->type == D_DIR) {
        kputs("vfs_open: Cannot open directory as file");
        return -1;
    }
    
    // Load file content into buffer (or create empty buffer for new files)
    uint8_t* file_buffer = NULL;
    uint32_t file_size = 0;
    
    if (flags & VFS_O_TRUNC) {
        // Truncate existing file to zero
        file_buffer = (uint8_t*)kmalloc(1);
        file_size = 0;
    } else {
        if (vfs_load_file_content(file_node, &file_buffer, &file_size) != 0) {
            // If loading fails for a new file, create empty buffer
            file_buffer = (uint8_t*)kmalloc(1);
            file_size = 0;
        }
    }
    
    // Initialize file descriptor
    open_files[fd].node = file_node;
    open_files[fd].position = (flags & VFS_O_APPEND) ? file_size : 0;
    open_files[fd].flags = flags;
    open_files[fd].mode = mode;
    open_files[fd].buffer = file_buffer;
    open_files[fd].buffer_size = file_size;
    open_files[fd].is_open = 1;
    
    kprintf("vfs_open: Opened file %s (fd=%d, size=%d)\n", path, fd, file_size);
    return fd;
}

// VFS read function
int vfs_read(int fd, void* buffer, size_t count) {
    // Validate file descriptor
    if (fd < 0 || fd >= MAX_OPEN_FILES) {
        kputs("vfs_read: Invalid file descriptor");
        return -1;
    }
    
    // Check if file is actually open
    if (!open_files[fd].is_open) {
        kputs("vfs_read: File descriptor not open");
        return -1;
    }
    
    // Check if file is opened for reading
    if (open_files[fd].flags == VFS_O_WRONLY) {
        kputs("vfs_read: File not opened for reading");
        return -1;
    }
    
    // Validate buffer
    if (!buffer) {
        kputs("vfs_read: Invalid buffer");
        return -1;
    }
    
    // Get current position and file size
    uint32_t position = open_files[fd].position;
    uint32_t file_size = open_files[fd].buffer_size;
    
    // Check if we're at end of file
    if (position >= file_size) {
        return 0; // EOF
    }
    
    // Calculate how much we can actually read
    size_t bytes_to_read = count;
    if (position + bytes_to_read > file_size) {
        bytes_to_read = file_size - position;
    }
    
    // Copy data from file buffer to user buffer
    uint8_t* src = open_files[fd].buffer + position;
    uint8_t* dest = (uint8_t*)buffer;
    
    memcpy(dest, src, bytes_to_read);
    
    // Update file position
    open_files[fd].position += bytes_to_read;
    
    kprintf("vfs_read: Read %d bytes from fd=%d\n", bytes_to_read, fd);
    return (int)bytes_to_read;
}

// VFS seek function (useful for positioning within files)
int vfs_seek(int fd, uint32_t offset, int whence) {
    // Validate file descriptor
    if (fd < 0 || fd >= MAX_OPEN_FILES) {
        kputs("vfs_seek: Invalid file descriptor");
        return -1;
    }
    
    // Check if file is actually open
    if (!open_files[fd].is_open) {
        kputs("vfs_seek: File descriptor not open");
        return -1;
    }
    
    uint32_t new_position;
    uint32_t file_size = open_files[fd].buffer_size;
    
    // Calculate new position based on whence
    switch (whence) {
        case 0: // SEEK_SET - absolute position
            new_position = offset;
            break;
        case 1: // SEEK_CUR - relative to current position
            new_position = open_files[fd].position + offset;
            break;
        case 2: // SEEK_END - relative to end of file
            new_position = file_size + offset;
            break;
        default:
            kputs("vfs_seek: Invalid whence parameter");
            return -1;
    }
    
    // Validate new position (don't allow seeking beyond file)
    if (new_position > file_size) {
        new_position = file_size;
    }
    
    // Update position
    open_files[fd].position = new_position;
    
    kprintf("vfs_seek: Set position to %d for fd=%d\n", new_position, fd);
    return new_position;
}

// Helper function to get file size
uint32_t vfs_get_file_size(int fd) {
    if (fd < 0 || fd >= MAX_OPEN_FILES || !open_files[fd].is_open) {
        return 0;
    }
    return open_files[fd].buffer_size;
}

// Helper function to get current file position
uint32_t vfs_get_position(int fd) {
    if (fd < 0 || fd >= MAX_OPEN_FILES || !open_files[fd].is_open) {
        return 0;
    }
    return open_files[fd].position;
}

// VFS write function
int vfs_write(int fd, const void* buffer, size_t count) {
    // Validate file descriptor
    if (fd < 0 || fd >= MAX_OPEN_FILES) {
        kputs("vfs_write: Invalid file descriptor");
        return -1;
    }
    
    // Check if file is actually open
    if (!open_files[fd].is_open) {
        kputs("vfs_write: File descriptor not open");
        return -1;
    }
    
    // Check if file is opened for writing
    if (open_files[fd].flags == VFS_O_RDONLY) {
        kputs("vfs_write: File not opened for writing");
        return -1;
    }
    
    // Validate buffer
    if (!buffer || count == 0) {
        kputs("vfs_write: Invalid buffer or count");
        return -1;
    }
    
    // Get current position and file size
    uint32_t position = open_files[fd].position;
    uint32_t current_size = open_files[fd].buffer_size;
    uint32_t new_end_position = position + count;
    
    // Check if we need to expand the buffer
    if (new_end_position > current_size) {
        // Reallocate buffer to accommodate new data
        uint8_t* new_buffer = (uint8_t*)kmalloc(new_end_position);
        if (!new_buffer) {
            kputs("vfs_write: Failed to allocate larger buffer");
            return -1;
        }
        
        // Copy existing data to new buffer
        if (open_files[fd].buffer && current_size > 0) {
            memcpy(new_buffer, open_files[fd].buffer, current_size);
            kfree(open_files[fd].buffer);
        }
        
        // Update file descriptor with new buffer
        open_files[fd].buffer = new_buffer;
        open_files[fd].buffer_size = new_end_position;
        
        kprintf("vfs_write: Expanded buffer from %d to %d bytes\n", current_size, new_end_position);
    }
    
    // Copy data from user buffer to file buffer
    const uint8_t* src = (const uint8_t*)buffer;
    uint8_t* dest = open_files[fd].buffer + position;
    
    
    memcpy(dest, src, count);
    
    // Update file position
    open_files[fd].position += count;
    
    // Update the VFS node's inode data (in-memory only for now)
    if (open_files[fd].node) {
        // Note: This doesn't persist to disk in this implementation
        // You would need to implement write-back to ext2 for persistence
        kprintf("vfs_write: File size updated in memory (persistence not implemented)\n");
    }
    
    kprintf("vfs_write: Wrote %d bytes to fd=%d at position %d\n", count, fd, position);
    return (int)count;
}

// VFS truncate function (useful for truncating files)
int vfs_truncate(int fd, uint32_t length) {
    // Validate file descriptor
    if (fd < 0 || fd >= MAX_OPEN_FILES) {
        kputs("vfs_truncate: Invalid file descriptor");
        return -1;
    }
    
    // Check if file is actually open
    if (!open_files[fd].is_open) {
        kputs("vfs_truncate: File descriptor not open");
        return -1;
    }
    
    // Check if file is opened for writing
    if ((open_files[fd].flags & VFS_O_RDONLY) == VFS_O_RDONLY) {
        kputs("vfs_truncate: File not opened for writing");
        return -1;
    }
    
    uint32_t current_size = open_files[fd].buffer_size;
    
    if (length == current_size) {
        return 0; // No change needed
    }
    
    if (length < current_size) {
        // Shrinking the file - just update the size
        open_files[fd].buffer_size = length;
        
        // Reset position if it's beyond the new end
        if (open_files[fd].position > length) {
            open_files[fd].position = length;
        }
        
        kprintf("vfs_truncate: Truncated file to %d bytes\n", length);
    } else {
        // Expanding the file - reallocate and zero-fill
        uint8_t* new_buffer = (uint8_t*)kmalloc(length);
        if (!new_buffer) {
            kputs("vfs_truncate: Failed to allocate larger buffer");
            return -1;
        }
        
        // Copy existing data
        if (open_files[fd].buffer && current_size > 0) {
            memcpy(new_buffer, open_files[fd].buffer, current_size);
            kfree(open_files[fd].buffer);
        }
        
        // Zero-fill the new space
        for (uint32_t i = current_size; i < length; i++) {
            new_buffer[i] = 0;
        }
        
        open_files[fd].buffer = new_buffer;
        open_files[fd].buffer_size = length;
        
        kprintf("vfs_truncate: Expanded file to %d bytes\n", length);
    }
    
    return 0;
}

int vfs_flush(int fd) {
    // Validate file descriptor
    if (fd < 0 || fd >= MAX_OPEN_FILES) {
        kputs("vfs_flush: Invalid file descriptor");
        return -1;
    }
    
    // Check if file is actually open
    if (!open_files[fd].is_open) {
        kputs("vfs_flush: File descriptor not open");
        return -1;
    }
    
    vfs_node_t* node = open_files[fd].node;
    if (!node) {
        kputs("vfs_flush: No VFS node associated with file descriptor");
        return -1;
    }
    
    // Get the inode
    inode_t file_inode;
    if (node->inode <= INODE_TABLE_SIZE) {
        file_inode = inode_table[node->inode - 1];
    } else {
        read_inode(node->inode, &file_inode);
    }
    
    // Write the file data to disk
    uint32_t file_size = open_files[fd].buffer_size;
    char* file_data = (char*)open_files[fd].buffer;
    
    if (write_inode_data(&file_inode, file_data, file_size) == 0) {
        // Write the updated inode back to disk
        write_inode(node->inode, &file_inode);
        
        // Update filesystem metadata
        // Note: In a full implementation, you'd also update block bitmaps
        
        kprintf("vfs_flush: Successfully wrote %d bytes to disk for inode %d\n", file_size, node->inode);
        return 0;
    } else {
        kputs("vfs_flush: Failed to write file data to disk");
        return -1;
    }
}

// Add automatic flush option to vfs_close
int vfs_close(int fd) {
    // Validate file descriptor
    if (fd < 0 || fd >= MAX_OPEN_FILES) {
        kputs("vfs_close: Invalid file descriptor");
        return -1;
    }
    
    // Check if file is actually open
    if (!open_files[fd].is_open) {
        kputs("vfs_close: File descriptor not open");
        return -1;
    }
    
    // Flush changes to disk if file was opened for writing
    if (open_files[fd].flags != VFS_O_RDONLY) {
        kputs("vfs_close: Flushing changes to disk...");
        if (vfs_flush(fd) != 0) {
            kputs("vfs_close: Warning - failed to flush changes to disk");
        }
    }
    
    // Free the file buffer if allocated
    if (open_files[fd].buffer) {
        kfree(open_files[fd].buffer);
        open_files[fd].buffer = NULL;
    }
    
    // Clear the file descriptor entry
    open_files[fd].node = NULL;
    open_files[fd].position = 0;
    open_files[fd].flags = 0;
    open_files[fd].mode = 0;
    open_files[fd].buffer_size = 0;
    open_files[fd].is_open = 0;
    
    kprintf("vfs_close: Closed file descriptor %d\n", fd);
    return 0;
}


int vfs_mkdir(const char* path, uint32_t mode) {
    if (!path) {
        kputs("vfs_mkdir: Invalid path");
        return -1;
    }
    
    // Parse the path to get directory and dirname
    char* path_copy = (char*)kmalloc(strlen(path) + 1);
    strcpy(path_copy, path);
    
    char* dirname = NULL;
    char* last_slash = NULL;
    
    for (int i = strlen(path_copy) - 1; i >= 0; i--) {
        if (path_copy[i] == '/') {
            last_slash = &path_copy[i];
            break;
        }
    }
    
    vfs_node_t* parent_dir;
    
    if (last_slash) {
        *last_slash = '\0';
        dirname = last_slash + 1;
        parent_dir = vfs_resolve_path(path_copy[0] ? path_copy : "/");
    } else {
        dirname = path_copy;
        parent_dir = vfs_root;
    }
    
    if (!parent_dir || parent_dir->type != D_DIR) {
        kputs("vfs_mkdir: Parent directory not found");
        kfree(path_copy);
        return -1;
    }
    
    // Check if directory already exists
    vfs_load_children(parent_dir);
    vfs_node_t* existing = vfs_find_child(parent_dir, dirname);
    if (existing) {
        kputs("vfs_mkdir: Directory already exists");
        kfree(path_copy);
        return -1;
    }
    
    // Create directory on disk
    uint32_t new_inode = ext2_create_dir(parent_dir->inode, dirname, mode);
    if (new_inode == 0) {
        kputs("vfs_mkdir: Failed to create directory on disk");
        kfree(path_copy);
        return -1;
    }
    
    // Create VFS node
    vfs_node_t* new_node = (vfs_node_t*)kmalloc(sizeof(vfs_node_t));
    new_node->name = (char*)kmalloc(strlen(dirname) + 1);
    strcpy(new_node->name, dirname);
    new_node->inode = new_inode;
    new_node->type = D_DIR;
    new_node->parent = parent_dir;
    new_node->children = NULL;
    new_node->next = parent_dir->children;
    new_node->fs_specific = NULL;
    
    parent_dir->children = new_node;
    
    kprintf("vfs_mkdir: Created directory %s with inode %d\n", path, new_inode);
    kfree(path_copy);
    return 0;
}



void init_vfs(){
    // Initialize globals
    num_mounted_fs = 0;
    vfs_root = NULL;

    // Initialize file descriptor table
    vfs_init_file_table();
    
    // Read the superblock, bgd table, and inode table from disk
    read_superblock();
    read_bgd_table();
    read_inode_table();
    
    // read the root directory, always at inode 2
    if (vfs_mount_ext2("/", 2) == 0) {
        kprintf("Successfully mounted ext2 filesystem at / (vfs_root = %p)\n", vfs_root);
    } else {
        kputs("Failed to mount ext2 filesystem");
        return;
    }
}



