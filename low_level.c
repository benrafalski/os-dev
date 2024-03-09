// instructions in and out are used to 
// read and write data to the I/O addresses 
// that are mapped to specific controller registers

unsigned char port_byte_in(unsigned short port){
    unsigned char result;
    // in: reads contents of port to al
    // load eax into result (=a)
    // load edx into port (d)
    __asm__("in %%dx, %%al" : "=a" (result) : "d" (port));
    return result;
}

void port_byte_out(unsigned short port, unsigned char data){
    // out: updates contents of port to al
    // al=data
    // dx=port
    __asm__("out %%al, %%dx" : : "a" (data), "d" (port));
}

unsigned char port_word_in(unsigned short port){
    unsigned short result;
    __asm__("in %%dx, %%ax" : "=a" (result) : "d" (port));
    return result;
}

void port_word_out(unsigned short port, unsigned short data){
    __asm__("out %%ax, %%dx" : : "a" (data), "d" (port));
}