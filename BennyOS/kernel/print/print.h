#include "colors.h"
#include "portio.h"
#include "string.h"
#include <stdint.h>

#define VIDEO_MEMORY 0xB8000
#define VGA_HEIGHT 25
#define VGA_WIDTH 80
#define REG_SCREEN_CTRL 0x3D4
#define REG_SCREEN_DATA 0x3D5

uint8_t color = WHITE_FGD | BLUE_BGD;

void set_color(unsigned char foreground, unsigned char background)
{
    color = foreground | background;
}
uint8_t get_color()
{
    return color;
}

void enable_cursor(uint8_t cursor_start, uint8_t cursor_end)
{
	outb(REG_SCREEN_CTRL, 0x0A);
	outb(REG_SCREEN_DATA, (inb(REG_SCREEN_DATA) & 0xC0) | cursor_start);
 
	outb(REG_SCREEN_CTRL, 0x0B);
	outb(REG_SCREEN_DATA, (inb(REG_SCREEN_DATA) & 0xE0) | cursor_end);
}

void disable_cursor()
{
	outb(REG_SCREEN_CTRL, 0x0A);
	outb(REG_SCREEN_DATA, 0x20);
}

void update_cursor(int x, int y)
{
	uint16_t pos = y * VGA_WIDTH + x;
 
	outb(REG_SCREEN_CTRL, 0x0F);
	outb(REG_SCREEN_DATA, (uint8_t) (pos & 0xFF));
	outb(REG_SCREEN_CTRL, 0x0E);
	outb(REG_SCREEN_DATA, (uint8_t) ((pos >> 8) & 0xFF));
}

uint16_t get_cursor_position(void)
{
    uint16_t pos = 0;
    outb(REG_SCREEN_CTRL, 0x0F);
    pos |= inb(REG_SCREEN_DATA);
    outb(REG_SCREEN_CTRL, 0x0E);
    pos |= ((uint16_t)inb(REG_SCREEN_DATA)) << 8;
    return pos;
}

void print_char(uint8_t c, int x, int y){
    volatile uint16_t * where;
    where = (volatile uint16_t *)VIDEO_MEMORY + (y * VGA_WIDTH + x) ;
    *where = c | (color << 8);
}


void kputs(const char *str)
{
    int len = strlen(str);
    uint16_t pos = get_cursor_position();
    int x = pos % VGA_WIDTH;
    int y = pos / VGA_WIDTH;

    while(*str != '\0'){

        if(*str == '\n'){
            y++;
            x = 0;
            str++;
            continue;
        }

        print_char(*str, x, y);
        x++;
        str++;
    }
    y += (len / VGA_WIDTH) + 1;
    update_cursor(0, y);
}

void kprintf(const char* str, int arg){
    int len = strlen(str) - 2;
    uint16_t pos = get_cursor_position();
    int x = pos % VGA_WIDTH;
    int y = pos / VGA_WIDTH;

    while(*str != '\0'){
        if(*str == '%'){
            char *trash;
            char *num;
            
            str++;
            if(*str == 'd'){
                // print int
                num = citoa(arg, trash, 10);
                // len += strlen(num);
                
            }
            else if(*str == 'x'){
                // print hex
                num = citoa(arg, trash, 16);
                // len += strlen(num);
            }

            while(*num != '\0'){
                print_char(*num, x, y);
                x++;
                num++;
            }
            str++;
        }

        if(*str == '\n'){
            y++;
            x = 0;
            str++;
            continue;
        }

        print_char(*str, x, y);
        x++;
        str++;
    }  

    update_cursor(x-1, y);
}

void clear_screen()
{
    uint16_t row = 0;
    uint16_t col = 0;

    for (row = 0; row < VGA_HEIGHT; row++)
    {
        for (col = 0; col < VGA_WIDTH; col++)
        {
            print_char(' ', row, col);
        }
    }

    update_cursor(0, 0);
}