#include "colors.h"
#include "portio.h"
#include <stdint.h>

#define VIDEO_MEMORY 0xB8000
#define VGA_HEIGHT 25
#define VGA_WIDTH 80
#define REG_SCREEN_CTRL 0x3D4
#define REG_SCREEN_DATA 0x3D5

uint8_t color = WHITE_FGD | BLUE_BGD;

// A utility function to reverse a string
void reverse(char str[], int length)
{
    int start = 0;
    int end = length - 1;
    while (start < end)
    {
        char temp = str[start];
        str[start] = str[end];
        str[end] = temp;
        end--;
        start++;
    }
}
char *citoa(int num, char *str, int base)
{
    int i = 0;
    int isNegative = 0;

    /* Handle 0 explicitly, otherwise empty string is
     * printed for 0 */
    if (num == 0)
    {
        str[i++] = '0';
        str[i] = '\0';
        return str;
    }

    // In standard itoa(), negative numbers are handled
    // only with base 10. Otherwise numbers are
    // considered unsigned.
    if (num < 0 && base == 10)
    {
        isNegative = 1;
        num = -num;
    }

    // Process individual digits
    while (num != 0)
    {
        int rem = num % base;
        str[i++] = (rem > 9) ? (rem - 10) + 'a' : rem + '0';
        num = num / base;
    }

    // If number is negative, append '-'
    if (isNegative)
        str[i++] = '-';

    str[i] = '\0'; // Append string terminator

    // Reverse the string
    reverse(str, i);

    return str;
}

void set_color(unsigned char foreground, unsigned char background)
{
    color = foreground | background;
}
uint8_t get_color()
{
    return color;
}

int get_screen_offset(int row, int col)
{
    // For example, if I want to set a character at
    // row 3, column 4 of the display, then the
    // character cell of that will be at a (decimal)
    // offset of 488 ((3 * 80 (i.e. the the row width) + 4) * 2 = 488)
    // from the start
    // of video memory.

    int offset = ((row * VGA_WIDTH) + col) * 2;
    return offset;
}

// https://wiki.osdev.org/Text_Mode_Cursor
int get_cursor()
{
    outb(REG_SCREEN_CTRL, 0x0E);
    int offset = inb(REG_SCREEN_DATA);
    outb(REG_SCREEN_CTRL, 0x0F);
    offset += inw(REG_SCREEN_DATA);
    return offset * 2;

}

void set_cursor(int offset)
{
    offset /= 2;
    outb(REG_SCREEN_CTRL, 0x0E);
    outb(REG_SCREEN_DATA, 0);
    outb(REG_SCREEN_CTRL, 0x0F);
    outw(REG_SCREEN_DATA, offset);
}

uint32_t get_cursor_row()
{
    uint32_t offset = get_cursor();
    return (offset / VGA_WIDTH) / 2;
}

uint32_t get_cursor_col()
{
    uint32_t offset = get_cursor();
    return (offset % VGA_WIDTH) / 2;
}

void print_char(unsigned char character, int row, int col)
{
    int offset;
    unsigned char *vidmem = (unsigned char *)VIDEO_MEMORY;
    if (col >= 0 && row >= 0)
    {
        offset = get_screen_offset(row, col);
    }
    else
    {
        offset = get_cursor();
    }

    vidmem[offset] = character;
    vidmem[offset + 1] = color;
    offset += 2;
    // offset = handle_scrolling(offset);
    set_cursor(offset);
}

void print_at(const char *message, int row, int col)
{
    if (col >= 0 && row >= 0)
    {
        set_cursor(get_screen_offset(row, col));
    }

    // char digits[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9' };
    int i = 0;
    while (message[i] != 0)
    {
        if(message[i] == '\n'){
            col = 0;
            row++;

        } else{
            print_char(message[i], row, col);
            col++;
        }

        int off = get_screen_offset(row, col);
        set_cursor(off);
        i++;
        
    }
    // row++;
    // set_cursor(get_screen_offset(row, 0));
}

void kputs(const char *string)
{
    uint32_t row = get_cursor_row();
    uint32_t col = get_cursor_col();

    // print_char('0' + row, 10, 5);
    // print_char('0' + col, 11, 5);

    print_at(string, row, col);
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

    set_cursor(get_screen_offset(0, 0));
}