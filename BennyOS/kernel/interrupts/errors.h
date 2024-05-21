void panic(const char* msg){
    kputs(msg);
    asm("hlt;");
}