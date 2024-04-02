#define PS2_RW_DATA 0x60

#define BACKSPACE   0x0E
#define SPACE       0x39
#define ENTER       0x1C


int buffer = 0;

char scancode_ascii(const char scan_code)
{
    if(scan_code >= 0x2 && scan_code <= 0xD){
        char *str = "1234567890-=";
        return str[scan_code - 0x2];
    }
    else if(scan_code >= 0x10 && scan_code <= 0x1B){
        char *str = "qwertyuiop[]";
        return str[scan_code - 0x10];
    }
    else if (scan_code >= 0x1E && scan_code <= 0x29){
        char *str = "asdfghjkl;'`";
        return str[scan_code - 0x1E];
    }
    else if (scan_code >= 0x2B && scan_code <= 0x35){
        char *str = "\\zxcvbnm,./";
        return str[scan_code - 0x2B];
    }
    else if(scan_code == BACKSPACE){
        return BACKSPACE;
    }
    else if(scan_code == ENTER){
        // kputs("here");
        return ENTER;
    }
    else if(scan_code == SPACE){
        return SPACE;
    }
    else{
        return 0;
    }
}

// kernel == 4157984 bytes
//        == 0x3F7220 bytes

char read_key(void){
    uint16_t pos = get_cursor_position();
    int x = pos % VGA_WIDTH;
    int y = pos / VGA_WIDTH;

    char buff[1000];

    unsigned char scan_code = inb(PS2_RW_DATA);
    char key = scancode_ascii(scan_code);
    if(key){
        if(buffer >= 0){
            if(key == BACKSPACE && buffer > 0){
                print_char(' ', x-1, y);
                update_cursor(x-1, y);
                buffer--;
            }
            else if(key == SPACE){
                buff[buffer] = ' ';
                print_char(' ', x, y);
                update_cursor(x+1, y);
                buffer++;
            } 
            else if(key == ENTER){
                kputs("");
                kputs(buff);
                kprintf(">:  ", 0);
                
                for(int i = 0; i < buffer; i++) buff[i] = 0;
                buffer = 0;
            } else if (key != BACKSPACE){
                buff[buffer] = key;
                print_char(key, x, y);
                update_cursor(x+1, y);
                buffer++;
            }
        }
        
        
    }
}