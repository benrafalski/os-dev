#include "print/print.h"


void main(){
    set_color(WHITE_FGD, BLUE_BGD);
    clear_screen();
    kputs("Hello from the kernel!");
}


// Phase 0: Introduction
// 1. Welcome to Operating Systems Development
// 2. Building the latest GCC

// Phase I: Beginning
// In this phase we will set up a toolchain and create a basic kernel that will become the core of the new operating system.
// 1. Setting up a Cross-Toolchain
// 2. Creating a Hello World kernel
// 3. Setting up a Project
// 4. Calling Global Constructors
// 5. Terminal Support
// 6. Stack Smash Protector
// 7. Multiboot
// 8. Global Descriptor Table
// 9. Memory Management
// 10. Interrupts
// 11. Multithreaded Kernel
// 12. Keyboard
// 13. Internal Kernel Debugger
// 14. Filesystem Support

// Phase II: User-Space
// In this phase we'll expand your operating system into user-space and add support for programs, which is enough for your project to qualify as a small operating system. You need to work on system calls, program loading, memory management and rework parts of your system early in this phase.
// 1. User-Space
// 2. Program Loading
// 3. System Calls
// 4. OS Specific Toolchain
// 5. Creating a C Library
// 6. Fork and Execute
// 7. Shell

// Phase III: Extending your Operating System
// With these basic features in place, you can now start writing your operating system and all its wonderful features. You'll add games, editors, test programs, command line utilties, drivers and whatever you can imagine. Your skill and imagination is the limit here. You can delay many of these features until later in your operating system and make then in almost any order.
// 1. Time
// 2. Threads
// 3. Thread Local Storage
// 4. Symmetric Multiprocessing
// 5. Secondary Storage
// 6. Real Filesystems
// 7. Graphics
// 8. User Interface
// 9. Networking
// 10. Sound
// 11. Universal Serial Bus

// Phase IV: Bootstrapping
// You now have your basic operating system in place and you are ready to move onto the next level. In this phase, we will start porting software onto your operating system such that you can become self-hosting. You already begun your effort toward being self-hosting when you set up your OS-specific toolchain and it pays off now.
// 1. Porting Software
// 2. Porting GCC
// 3. Compiling your OS under your OS
// 4. Fully Self-hosting

// Phase V: Profit
// You have now successfully created a real operating system that is fully self-hosting and the envy of the entire operating systems development community. You have ported quake, have OpenGL programs, a working browser, a thriving community of contributors, and much success. You can now start over and develop the next operating system from your own operating system.