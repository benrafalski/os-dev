#define VIDEO_ADDRESS 0xb8000
#define MAX_ROWS 25
#define MAX_COLS 80

#define REG_SCREEN_CTRL 0x3d4
#define REG_SCREEN_DATA 0x3d5

#define BLACK_BGD 0x00
#define BLUE_BGD 0x10
#define GREEN_BGD 0x20
#define CYAN_BGD 0x30
#define RED_BGD 0x40
#define PURPLE_BGD 0x50
#define BROWN_BGD 0x60
#define GRAY_BGD 0x70
#define DARK_GRAY_BGD 0x80
#define LIGHT_BLUE_BGD 0x90
#define LIGHT_GREEN_BGD 0xA0
#define LIGHT_CYAN_BGD 0xB0
#define LIGHT_RED_BGD 0xC0
#define LIGHT_PURPLE_BGD 0xD0
#define YELLOW_BGD 0xE0
#define WHITE_BGD 0xF0

// ; foreground colors
#define BLACK_FGD 0x00
#define BLUE_FGD 0x01
#define GREEN_FGD 0x02
#define CYAN_FGD 0x03
#define RED_FGD 0x04
#define PURPLE_FGD 0x05
#define BROWN_FGD 0x06
#define GRAY_FGD 0x07
#define DARK_GRAY_FGD 0x08
#define LIGHT_BLUE_FGD 0x09
#define LIGHT_GREEN_FGD 0x0A
#define LIGHT_CYAN_FGD 0x0B
#define LIGHT_RED_FGD 0x0C
#define LIGHT_PURPLE_FGD 0x0D
#define YELLOW_FGD 0x0E
#define WHITE_FGD 0x0F

#include "./low_level.h"

int get_screen_offset(int row, int col){
    // For example, if I want to set a character at
    // row 3, column 4 of the display, then the 
    // character cell of that will be at a (decimal)
    // offset of 488 ((3 * 80 (i.e. the the row width) + 4) * 2 = 488) 
    // from the start
    // of video memory.

    int offset = (row * MAX_COLS + col) * 2;
    return offset;
}

int get_cursor(){
    port_byte_out(REG_SCREEN_CTRL, 14);
    int offset = port_byte_in(REG_SCREEN_DATA);
    port_byte_out(REG_SCREEN_CTRL, 15);
    offset += port_byte_in(REG_SCREEN_DATA);

    return offset * 2;
}

void set_cursor(int offset){
    offset /= 2;
    port_byte_out(REG_SCREEN_CTRL, 14);
    port_byte_out(REG_SCREEN_DATA, 0);
    port_byte_out(REG_SCREEN_CTRL, 15);
    port_byte_out(REG_SCREEN_DATA, offset);
}

void print_char(char character, int col, int row, char color){
    int offset;
    unsigned char *vidmem = (unsigned char *) VIDEO_ADDRESS;
    if(col >= 0 && row >= 0){
        offset = get_screen_offset(row, col);
    }else{
        offset = get_cursor();
    }

    if(character == '\n'){
        int rows = offset / (2 * MAX_COLS);
        offset = get_screen_offset(79, rows);
    }else{
        vidmem[offset] = character;
        vidmem[offset + 1] = color;
    }

    offset += 2;
    // offset = handle_scrolling(offset);
    set_cursor(offset);

}

void print_at(char* message, int row, int col){
    if(col >= 0 && row >= 0){
        set_cursor(get_screen_offset(row, col));
    }
    int i = 0;
    while(message[i] != 0){
        print_char(message[i++], col, row, WHITE_FGD|BLACK_BGD);
        col++;
        // set_cursor(get_screen_offset(row, col+1));
    }
}

void clear_screen(){
    int row = 0;
    int col = 0;

    for(row = 0; row < MAX_ROWS; row++){
        for(col = 0; col < MAX_COLS; col++){
            print_char(' ', col, row, WHITE_FGD|BLACK_BGD);
        }
    }

    set_cursor(get_screen_offset(0, 0));
}