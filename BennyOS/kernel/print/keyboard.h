#define PS2_RW_DATA 0x60

#define BACKSPACE 0x0E

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
    // else if(scan_code == BACKSPACE){
    //     // return ' ';
    // }
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

    unsigned char scan_code = inb(PS2_RW_DATA);
    char key = scancode_ascii(scan_code);
    if(key){
        if(key == BACKSPACE){
            // print_char(key, x-1, y);
            // update_cursor(x, y);
            // int i = 100;
            // int j = 100;
            // i = 0x68;
            // i = 0x68;
            // i = 0x68;
            // i = 0x68;kkkkk
        }else{
            print_char(key, x, y);
            update_cursor(x+1, y);
        }
        
    }
}