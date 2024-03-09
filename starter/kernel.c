#include "./screen.h"


// A utility function to reverse a string
void reverse(char str[], int length)
{
    int start = 0;
    int end = length - 1;
    while (start < end) {
        char temp = str[start];
        str[start] = str[end];
        str[end] = temp;
        end--;
        start++;
    }
}
char* citoa(int num, char* str, int base)
{
    int i = 0;
    int isNegative = 0;
 
    /* Handle 0 explicitly, otherwise empty string is
     * printed for 0 */
    if (num == 0) {
        str[i++] = '0';
        str[i] = '\0';
        return str;
    }
 
    // In standard itoa(), negative numbers are handled
    // only with base 10. Otherwise numbers are
    // considered unsigned.
    if (num < 0 && base == 10) {
        isNegative = 1;
        num = -num;
    }
 
    // Process individual digits
    while (num != 0) {
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

void write_string(int color, const char *string)
{
    volatile char *video = (volatile char*)0xb8000;
    while( *string != 0 )
    {
        *video++ = *string++;
        *video++ = color;
    }
}


void main() {
    // write_string(WHITE_FGD | CYAN_BGD, "X");
    // print_char('P', 0, 0, WHITE_FGD|GREEN_BGD);
    clear_screen();
    print_at("message1", 0, 0);
    print_at("messag2", 1, 0);

    // set_cursor(get_screen_offset(1, 1));

}