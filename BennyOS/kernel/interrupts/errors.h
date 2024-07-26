void panic(const char* msg){
    print_exception_msg(msg);
    for(;;) {
        asm("cli;hlt");
    }
}