#define PS2_RW_DATA 0x60

#define BACKSPACE   0x0E
#define SPACE       0x39
#define ENTER       0x1C
#define LSHIFT      0x2A  // Left shift press
#define RSHIFT      0x36  // Right shift press
#define LSHIFT_REL  0xAA  // Left shift release
#define RSHIFT_REL  0xB6  // Right shift release

#define KEY_ROW1    "1234567890-=" 
#define KEY_ROW2    "qwertyuiop[]"
#define KEY_ROW3    "asdfghjkl;'`"
#define KEY_ROW4    "\\zxcvbnm,./" 

// Shifted versions
#define KEY_ROW1_SHIFT "!@#$%^&*()_+"
#define KEY_ROW2_SHIFT "QWERTYUIOP{}"
#define KEY_ROW3_SHIFT "ASDFGHJKL:\"~"
#define KEY_ROW4_SHIFT "|ZXCVBNM<>?"

char buff[1000] = {0};
int buffer = 0;
int shift_pressed = 0;  // Track shift state

char scancode_ascii(const char scan_code)
{

    if (scan_code & 0x80) {
        unsigned char press_code = scan_code & 0x7F;
        if (press_code == LSHIFT || press_code == RSHIFT) {
            shift_pressed = 0;
        }
        return 0;
    }
    
    if(scan_code == LSHIFT || scan_code == RSHIFT) {
        shift_pressed = 1;
        return 0;  
    }

    if(scan_code >= 0x2 && scan_code <= 0xD){
        return shift_pressed ? KEY_ROW1_SHIFT[scan_code - 0x2] : KEY_ROW1[scan_code - 0x2];
    }
    else if(scan_code >= 0x10 && scan_code <= 0x1B){
        return shift_pressed ? KEY_ROW2_SHIFT[scan_code - 0x10] : KEY_ROW2[scan_code - 0x10];
    }
    else if (scan_code >= 0x1E && scan_code <= 0x29){
        return shift_pressed ? KEY_ROW3_SHIFT[scan_code - 0x1E] : KEY_ROW3[scan_code - 0x1E];
    }
    else if (scan_code >= 0x2B && scan_code <= 0x35){
        return shift_pressed ? KEY_ROW4_SHIFT[scan_code - 0x2B] : KEY_ROW4[scan_code - 0x2B];
    }
    else if(scan_code == BACKSPACE){
        return BACKSPACE;
    }
    else if(scan_code == ENTER){
        return ENTER;
    }
    else if(scan_code == SPACE){
        return SPACE;
    }
    else{
        return 0;
    }
}

char read_key(void){
    uint16_t pos = get_cursor_position();
    int x = pos % VGA_WIDTH;
    int y = pos / VGA_WIDTH;

    

    unsigned char scan_code = inb(PS2_RW_DATA);
    char key = scancode_ascii(scan_code);

    if(scan_code == 0xa){
        // TODO
        // fix '9' bug
        buff[buffer] = '9';
        print_char('9', x, y);
        update_cursor(x+1, y);
        buffer++;
    }
    else if(key){
        if(buffer >= 0){
            if(key == BACKSPACE && buffer > 0){
                print_char(' ', x-1, y);
                update_cursor(x-1, y);
                buffer--;
                buff[buffer] = 0;
            }
            else if(key == SPACE){
                buff[buffer] = ' ';
                print_char(' ', x, y);
                update_cursor(x+1, y);
                buffer++;
            } 
            else if(key == ENTER){
                buff[buffer] = '\0';  // Null terminate
                // uint32_t nargs = strsplit(buff, ' ');

                if(strcmp("clear", buff) == 0) {
                    clear_screen();
                }
                else if(strcmp("exit", buff) == 0) {
                    outw(0x604, 0x2000);
                }
                else if(strncmp("touch ", buff, 6) == 0) {
                    // Create new file: touch filename
                    kputs("");
                    char* filename = buff + 6;  // Skip "touch "
                    int fd = vfs_open(filename, VFS_O_CREAT | VFS_O_WRONLY, 0644);
                    if (fd >= 0) {
                        vfs_close(fd);
                        kprintf("\nCreated file: %s", filename);
                    } else {
                        kprintf("\nFailed to create file: %s", filename);
                    }
                }
                else if(strncmp("mkdir ", buff, 6) == 0) {
                    // Create directory: mkdir dirname
                    kputs("");
                    char* dirname = buff + 6;  // Skip "mkdir "
                    if (vfs_mkdir(dirname, 0755) == 0) {
                        kprintf("\nCreated directory: %s", dirname);
                    } else {
                        kprintf("\nFailed to create directory: %s", dirname);
                    }
                }
                else if(strncmp("echo ", buff, 5) == 0) {
                    // Write to file: echo "text" > filename
                    char* rest = buff + 5;  // Skip "echo "
                    char* gt_pos = strstr(rest, " > ");
                    if (gt_pos) {
                        *gt_pos = '\0';  // Terminate text part
                        char* filename = gt_pos + 3;  // Skip " > "
                        
                        int fd = vfs_open(filename, VFS_O_CREAT | VFS_O_WRONLY | VFS_O_TRUNC, 0644);
                        if (fd >= 0) {
                            vfs_write(fd, rest, strlen(rest));
                            vfs_close(fd);
                            kprintf("\nWrote to file: %s", filename);
                        } else {
                            kprintf("\nFailed to write to file: %s", filename);
                        }
                    } else {
                        kprintf("\n%s", rest);  // Just echo the text
                    }
                }
                else if(strncmp("cat ", buff, 4) == 0) {
                    // Read file: cat filename
                    kputs("");
                    char* filename = buff + 4;  // Skip "cat "
                    int fd = vfs_open(filename, VFS_O_RDONLY, 0);
                    if (fd >= 0) {
                        char read_buf[512];
                        int bytes_read = vfs_read(fd, read_buf, sizeof(read_buf) - 1);
                        if (bytes_read > 0) {
                            read_buf[bytes_read] = '\0';
                            kprintf("\n%s", read_buf);
                        }
                        kputs("");
                        vfs_close(fd);
                    } else {
                        kprintf("\nFailed to read file: %s", filename);
                    }
                }
                else if(strcmp("ls", buff) == 0) {
                    // List directory contents
                    kputs("");
                    vfs_list_directory(vfs_root);
                }
                else if(strcmp("lsmem", buff) == 0) {
                    kputs("");
                    print_memory_map();
                }
                else if(strcmp("lspci", buff) == 0) {
                    kputs("");
                    pci_list_devices();
                }
                else {
                    if(buffer > 0) {
                        kprintf("\nCommand not recognized: '%s'", buff);
                        kputs("\nAvailable commands: clear, exit, touch, mkdir, echo, cat, ls");
                    }
                }
reset:                
                for(int i = 0; i < buffer; i++) buff[i] = 0;
                buffer = 0;
                kprintf("\n>: ");
            } else if (key != BACKSPACE){
                buff[buffer] = key;
                print_char(key, x, y);
                update_cursor(x+1, y);
                buffer++;
            }
        }
    }

exit:
    return key;
}