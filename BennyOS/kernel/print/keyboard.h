#define PS2_RW_DATA 0x60

#define BACKSPACE   0x0E
#define SPACE       0x39
#define ENTER       0x1C
#define KEY_ROW1    "1234567890-=" 
#define KEY_ROW2    "qwertyuiop[]"
#define KEY_ROW3    "asdfghjkl;'`"
#define KEY_ROW4    "\\zxcvbnm,./" 


int buffer = 0;

char scancode_ascii(const char scan_code)
{
    if(scan_code >= 0x2 && scan_code <= 0xD){
        return KEY_ROW1[scan_code - 0x2];
    }
    else if(scan_code >= 0x10 && scan_code <= 0x1B){
        return KEY_ROW2[scan_code - 0x10];
    }
    else if (scan_code >= 0x1E && scan_code <= 0x29){
        return KEY_ROW3[scan_code - 0x1E];
    }
    else if (scan_code >= 0x2B && scan_code <= 0x35){
        return KEY_ROW4[scan_code - 0x2B];
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

void command_ls(){
    kputs("");
    print_dirlist(cwd);
}

void command_cd(char* path){
    kputs("");
    vfs_cd(path);
}

void command_cat(char* path){
    kputs("");
    char* buff = vfs_read_file(path); 
    if(buff){
        kputs(buff);
    }
}

char read_key(void){

    uint16_t pos = get_cursor_position();
    int x = pos % VGA_WIDTH;
    int y = pos / VGA_WIDTH;

    char buff[1000];

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
                uint32_t nargs = strsplit(buff, ' ');

                if(strcmp("clear", buff)) clear_screen();
                // In newer versions of QEMU, you can do shutdown with
                else if(strcmp("exit", buff)) outw(0x604, 0x2000);
                else if(strcmp("ls", buff)) command_ls();
                else if(strcmp("cd", buff)) command_cd(buff + 3);
                else if(strcmp("cat", buff)) command_cat(buff + 4);

                else {
                    kputs("");
                    char msg[1000] = "Unrecognized command: ";
                    strcat(msg, buff);
                    kputs(msg);
                }
reset:                
                for(int i = 0; i < buffer; i++) buff[i] = 0;
                buffer = 0;
                kprintf(">:  ", 0);
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