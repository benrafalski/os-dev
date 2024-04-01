#define MASTER_PIC_CMD 0x20
#define MASTER_PIC_DATA 0x21
#define SLAVE_PIC_CMD 0xA0
#define SLAVE_PIC_DATA 0xA1
#define PIC_EOI 0x20

#define ICW1_ICW4 0x01 /* Indicates that ICW4 will be present */
#define ICW1_INIT 0x10 /* Initialization - required! */
#define ICW4_8086 0x01 /* 8086/88 (MCS-80/85) mode */

// End of interrupt command
//      issued to the PIC chips at the end of the IRQ-based interrupt routine
void pic_send_eoi(uint8_t irq)
{
    if (irq >= 8)
    {
        outb(SLAVE_PIC_CMD, PIC_EOI);
    }
    outb(MASTER_PIC_CMD, PIC_EOI);
}

void pic_remapping(uint8_t offset)
{
    uint8_t master_mask, slave_mask;
    // save masks
    master_mask = inb(MASTER_PIC_DATA);
    slave_mask = inb(SLAVE_PIC_DATA);

    // starts the initialization sequence (in cascade mode)
    outb(MASTER_PIC_CMD, ICW1_INIT | ICW1_ICW4);
    // io_wait();
    outb(SLAVE_PIC_CMD, ICW1_INIT | ICW1_ICW4);
    // io_wait();

    // ICW2
    outb(MASTER_PIC_DATA, offset);
    // io_wait();
    outb(SLAVE_PIC_DATA, offset + 0x08);

    // ICW3:
    //      tell Master PIC that there is a slave PIC at IRQ2 (0000 0100)
    //      tell Slave PIC its cascade identity (0000 0010)
    outb(MASTER_PIC_DATA, 4);
    outb(SLAVE_PIC_DATA, 2);

    // ICW4: have the PICs use 8086 mode (and not 8080 mode)
    outb(MASTER_PIC_DATA, ICW4_8086);
    outb(SLAVE_PIC_DATA, ICW4_8086);

    // restore saved masks.
    outb(MASTER_PIC_DATA, master_mask);
    outb(SLAVE_PIC_DATA, slave_mask);
}

void pic_disable(void) {
    outb(MASTER_PIC_DATA, 0xFF);
    outb(SLAVE_PIC_DATA, 0xFF);
}

void pic_clear_mask(uint8_t irq) {
    uint16_t port;
    uint8_t value;
 
    if(irq < 8) {
        port = MASTER_PIC_DATA;
    } else {
        port = SLAVE_PIC_DATA;
        irq -= 8;
    }
    value = inb(port) & ~(1 << irq);
    outb(port, value);        
}
