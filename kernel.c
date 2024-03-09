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

// void write_string(int color, const char *string)
// {
//     volatile char *video = (volatile char*)0xb8000;
//     while( *string != 0 )
//     {
//         *video++ = *string++;
//         *video++ = color;
//     }
// }


void main() {
    char * video_memory = ( char *) 0xb8000;
    *video_memory = 'X';
}