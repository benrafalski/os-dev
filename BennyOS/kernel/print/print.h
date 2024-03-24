#include "colors.h"
#include "portio.h"

#define VIDEO_MEMORY 0xB8000
#define VGA_HEIGHT 25
#define VGA_WIDTH 80
#define REG_SCREEN_CTRL 0x3D4
#define REG_SCREEN_DATA 0x3D5

unsigned char color = WHITE_FGD | BLUE_BGD;

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

int get_screen_offset(int row, int col)
{
    // For example, if I want to set a character at
    // row 3, column 4 of the display, then the
    // character cell of that will be at a (decimal)
    // offset of 488 ((3 * 80 (i.e. the the row width) + 4) * 2 = 488)
    // from the start
    // of video memory.

    // (r * 80  + c) * 2 = 488
    // r * 80  + c = 244

    int offset = (row * VGA_WIDTH + col) * 2;
    return offset;
}

int get_cursor()
{
    // port_byte_out(REG_SCREEN_CTRL, 0x0E);
    // int offset = port_byte_in(REG_SCREEN_DATA);
    // port_byte_out(REG_SCREEN_CTRL, 0x0F);
    // offset += port_byte_in(REG_SCREEN_DATA);
    // return offset * 2;

    int pos = 0;
    outb(0x3D4, 0x0F);
    pos |= inb(0x3D5);
    outb(0x3D4, 0x0E);
    pos |= ((int)inb(0x3D5)) << 8;
    return pos;

// https://wiki.osdev.org/Text_Mode_Cursor

}

void set_cursor(int offset)
{
    offset /= 2;
    outb(REG_SCREEN_CTRL, 0x0E);
    outb(REG_SCREEN_DATA, 0);
    outb(REG_SCREEN_CTRL, 0x0F);
    outb(REG_SCREEN_DATA, offset);
}

int get_cursor_row()
{
    int offset = get_cursor();
    return (offset / VGA_WIDTH);
}

int get_cursor_col()
{
    int offset = get_cursor();
    return (offset % VGA_WIDTH);
}

void print_char(char character, int col, int row)
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

    if (character == '\n')
    {
        int rows = offset / (2 * VGA_WIDTH);
        offset = get_screen_offset(79, rows);
    }
    else
    {
        vidmem[offset] = character;
        vidmem[offset + 1] = color;
    }

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
    int i = 0;
    while (message[i] != 0)
    {
        print_char(message[i++], col, row);
        col++;
        // set_cursor(get_screen_offset(row, col+1));
    }
}

void puts(const char *string)
{

    set_cursor(get_screen_offset(4, 4));

    int cur = get_cursor();

    int row = get_cursor_row();
    int col = get_cursor_col();
    char str[5];
    print_at(citoa(cur, str, 10), 0, 0);
}

void clear_screen()
{
    int row = 0;
    int col = 0;

    for (row = 0; row < VGA_HEIGHT; row++)
    {
        for (col = 0; col < VGA_WIDTH; col++)
        {
            print_char(' ', col, row);
        }
    }

    set_cursor(get_screen_offset(0, 0));
}