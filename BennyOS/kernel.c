void main(){
    const char *string = "BEN";
    volatile char *video = (volatile char*)0xb8000;
    while( *string != 0 )
    {
        *video++ = *string++;
        *video++ = 0x1f;
    }
}