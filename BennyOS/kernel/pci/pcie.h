#ifndef PCIE_H
#define PCIE_H

// #include <stdint.h>
// #include <stdbool.h>
// #include "../print/print.h"
// #include "../memory/mmu.h"

// Configuration Access Method 1
#define PCI_CONFIG_ADDRESS  0xCF8
#define PCI_CONFIG_DATA     0xCFC

// Configuration Space Registers
#define PCI_VENDOR_ID       0x00
#define PCI_DEVICE_ID       0x02
#define PCI_COMMAND         0x04
#define PCI_STATUS          0x06
#define PCI_REVISION_ID     0x08
#define PCI_PROG_IF         0x09
#define PCI_SUBCLASS        0x0A
#define PCI_CLASS_CODE      0x0B
#define PCI_HEADER_TYPE     0x0E
#define PCI_BAR0            0x10
#define PCI_BAR1            0x14
#define PCI_BAR2            0x18
#define PCI_BAR3            0x1C
#define PCI_BAR4            0x20
#define PCI_BAR5            0x24
#define PCI_INTERRUPT_LINE  0x3C
#define PCI_INTERRUPT_PIN   0x3D

// Device Classes
#define PCI_CLASS_STORAGE      0x01
#define PCI_CLASS_NETWORK      0x02
#define PCI_CLASS_DISPLAY      0x03
#define PCI_CLASS_BRIDGE       0x06
#define PCI_CLASS_SERIAL_BUS   0x0C

// Storage Subclasses
#define PCI_SUBCLASS_IDE       0x01
#define PCI_SUBCLASS_SATA      0x06
#define PCI_SUBCLASS_NVME      0x08

// Command Register Bits
#define PCI_COMMAND_IO_SPACE     0x01
#define PCI_COMMAND_MEMORY_SPACE 0x02
#define PCI_COMMAND_BUS_MASTER   0x04

#define MAX_PCI_DEVICES 128

typedef struct pci_device {
    uint16_t vendor_id;
    uint16_t device_id;
    uint8_t bus;
    uint8_t device;
    uint8_t function;
    uint8_t class_code;
    uint8_t subclass;
    uint8_t prog_if;
    uint8_t header_type;
    uint32_t bar[6];
    uint8_t interrupt_line;
    uint8_t interrupt_pin;
    struct pci_device* next;
} pci_device_t;

// Global device storage
static pci_device_t pci_devices[MAX_PCI_DEVICES];
static int pci_device_count = 0;
static pci_device_t* pci_device_list = NULL;

// Port I/O for PCI access
static inline void pci_outl(uint16_t port, uint32_t value) {
    asm volatile("outl %0, %1" : : "a"(value), "Nd"(port));
}

static inline uint32_t pci_inl(uint16_t port) {
    uint32_t value;
    asm volatile("inl %1, %0" : "=a"(value) : "Nd"(port));
    return value;
}

static inline void pci_outw(uint16_t port, uint16_t value) {
    asm volatile("outw %0, %1" : : "a"(value), "Nd"(port));
}

static inline uint16_t pci_inw(uint16_t port) {
    uint16_t value;
    asm volatile("inw %1, %0" : "=a"(value) : "Nd"(port));
    return value;
}

static inline void pci_outb(uint16_t port, uint8_t value) {
    asm volatile("outb %0, %1" : : "a"(value), "Nd"(port));
}

static inline uint8_t pci_inb(uint16_t port) {
    uint8_t value;
    asm volatile("inb %1, %0" : "=a"(value) : "Nd"(port));
    return value;
}

// Create configuration address
static uint32_t pci_make_address(uint8_t bus, uint8_t device, uint8_t function, uint8_t offset) {
    return (1 << 31) |              // Enable bit
           (bus << 16) |            // Bus number
           (device << 11) |         // Device number  
           (function << 8) |        // Function number
           (offset & 0xFC);         // Register offset (aligned to 4 bytes)
}

// Read configuration registers
static uint32_t pci_config_read32(uint8_t bus, uint8_t device, uint8_t function, uint8_t offset) {
    uint32_t address = pci_make_address(bus, device, function, offset);
    pci_outl(PCI_CONFIG_ADDRESS, address);
    return pci_inl(PCI_CONFIG_DATA);
}

static uint16_t pci_config_read16(uint8_t bus, uint8_t device, uint8_t function, uint8_t offset) {
    uint32_t address = pci_make_address(bus, device, function, offset);
    pci_outl(PCI_CONFIG_ADDRESS, address);
    return pci_inw(PCI_CONFIG_DATA + (offset & 2));
}

static uint8_t pci_config_read8(uint8_t bus, uint8_t device, uint8_t function, uint8_t offset) {
    uint32_t address = pci_make_address(bus, device, function, offset);
    pci_outl(PCI_CONFIG_ADDRESS, address);
    return pci_inb(PCI_CONFIG_DATA + (offset & 3));
}

// Write configuration registers
static void pci_config_write32(uint8_t bus, uint8_t device, uint8_t function, uint8_t offset, uint32_t value) {
    uint32_t address = pci_make_address(bus, device, function, offset);
    pci_outl(PCI_CONFIG_ADDRESS, address);
    pci_outl(PCI_CONFIG_DATA, value);
}

static void pci_config_write16(uint8_t bus, uint8_t device, uint8_t function, uint8_t offset, uint16_t value) {
    uint32_t address = pci_make_address(bus, device, function, offset);
    pci_outl(PCI_CONFIG_ADDRESS, address);
    pci_outw(PCI_CONFIG_DATA + (offset & 2), value);
}

// Get device class name
static const char* pci_get_class_name(uint8_t class_code) {
    switch(class_code) {
        case PCI_CLASS_STORAGE: return "Storage";
        case PCI_CLASS_NETWORK: return "Network";
        case PCI_CLASS_DISPLAY: return "Display";
        case PCI_CLASS_BRIDGE: return "Bridge";
        case PCI_CLASS_SERIAL_BUS: return "Serial Bus";
        default: return "Unknown";
    }
}

static const char* pci_get_storage_subclass_name(uint8_t subclass) {
    switch(subclass) {
        case PCI_SUBCLASS_IDE: return "IDE";
        case PCI_SUBCLASS_SATA: return "SATA";
        case PCI_SUBCLASS_NVME: return "NVMe";
        default: return "Other";
    }
}

// Check if device exists
static bool pci_device_exists(uint8_t bus, uint8_t device, uint8_t function) {
    uint16_t vendor_id = pci_config_read16(bus, device, function, PCI_VENDOR_ID);
    return vendor_id != 0xFFFF;
}

// Add device to our list
static void pci_add_device(uint8_t bus, uint8_t device, uint8_t function) {
    if (pci_device_count >= MAX_PCI_DEVICES) {
        kprintf("Warning: Maximum PCI devices reached!\n");
        return;
    }
    
    pci_device_t* dev = &pci_devices[pci_device_count];
    
    // Read basic device info
    dev->bus = bus;
    dev->device = device;
    dev->function = function;
    dev->vendor_id = pci_config_read16(bus, device, function, PCI_VENDOR_ID);
    dev->device_id = pci_config_read16(bus, device, function, PCI_DEVICE_ID);
    dev->class_code = pci_config_read8(bus, device, function, PCI_CLASS_CODE);
    dev->subclass = pci_config_read8(bus, device, function, PCI_SUBCLASS);
    dev->prog_if = pci_config_read8(bus, device, function, PCI_PROG_IF);
    dev->header_type = pci_config_read8(bus, device, function, PCI_HEADER_TYPE);
    dev->interrupt_line = pci_config_read8(bus, device, function, PCI_INTERRUPT_LINE);
    dev->interrupt_pin = pci_config_read8(bus, device, function, PCI_INTERRUPT_PIN);
    
    // Read BARs
    for (int i = 0; i < 6; i++) {
        dev->bar[i] = pci_config_read32(bus, device, function, PCI_BAR0 + (i * 4));
    }
    
    // Link to list
    dev->next = pci_device_list;
    pci_device_list = dev;
    
    pci_device_count++;
}

// Scan for devices on a specific bus
static void pci_scan_bus(uint8_t bus) {
    for (uint8_t device = 0; device < 32; device++) {
        if (pci_device_exists(bus, device, 0)) {
            pci_add_device(bus, device, 0);
            
            // Check for multi-function device
            uint8_t header_type = pci_config_read8(bus, device, 0, PCI_HEADER_TYPE);
            if (header_type & 0x80) {  // Multi-function device
                for (uint8_t function = 1; function < 8; function++) {
                    if (pci_device_exists(bus, device, function)) {
                        pci_add_device(bus, device, function);
                    }
                }
            }
        }
    }
}

// Initialize PCI subsystem and enumerate devices
static void pci_init(void) {
    // kprintf("Initializing PCI subsystem...\n");
    
    pci_device_count = 0;
    pci_device_list = NULL;
    
    // Scan bus 0 (and potentially others)
    pci_scan_bus(0);
    
    kprintf("PCI enumeration complete. Found %d devices.\n", pci_device_count);
}

// Print all discovered devices
static void pci_list_devices(void) {
    kprintf("Found %d PCI devices:\n", pci_device_count);
    
    pci_device_t* current = pci_device_list;
    int device_num = 0;
    
    while (current != NULL) {
        kprintf(" Device %d: %d:%d.%d (VID:0x%x, DID:0x%x)\n  Class:0x%x(%s); Sub:0x%x", 
                device_num++, current->bus, current->device, current->function,
                current->vendor_id, current->device_id, current->class_code,
                pci_get_class_name(current->class_code), current->subclass);
        
        if (current->class_code == PCI_CLASS_STORAGE) {
            kprintf("(%s)", pci_get_storage_subclass_name(current->subclass));
        }

        kprintf("\n");
        
        // Show non-zero BARs on same line
        bool has_bars = false;
        for (int i = 0; i < 6; i++) {
            if (current->bar[i] != 0) {
                if (!has_bars) {
                    kprintf("  BARs:");
                    has_bars = true;
                }
                kprintf(" %d:0x%x;", i, current->bar[i]);
            }
        }

        if(has_bars) {
            kprintf("\n");
        }
        
        if (current->interrupt_pin != 0) {
            kprintf("  INT:PIN%d/Line%d\n", current->interrupt_pin, current->interrupt_line);
        }
        
        // kprintf("\n");
        current = current->next;
    }
    
    kprintf("========================\n\n");
}

// Find devices by class/subclass
static pci_device_t* pci_find_device_by_class(uint8_t class_code, uint8_t subclass) {
    pci_device_t* current = pci_device_list;
    
    while (current != NULL) {
        if (current->class_code == class_code && current->subclass == subclass) {
            return current;
        }
        current = current->next;
    }
    
    return NULL;
}

// Find devices by vendor/device ID
static pci_device_t* pci_find_device_by_id(uint16_t vendor_id, uint16_t device_id) {
    pci_device_t* current = pci_device_list;
    
    while (current != NULL) {
        if (current->vendor_id == vendor_id && current->device_id == device_id) {
            return current;
        }
        current = current->next;
    }
    
    return NULL;
}

// Enable PCI device (set command register bits)
static void pci_enable_device(pci_device_t* device) {
    if (!device) return;
    
    uint16_t command = pci_config_read16(device->bus, device->device, device->function, PCI_COMMAND);
    command |= PCI_COMMAND_IO_SPACE | PCI_COMMAND_MEMORY_SPACE | PCI_COMMAND_BUS_MASTER;
    pci_config_write16(device->bus, device->device, device->function, PCI_COMMAND, command);
    
    kprintf("Enabled PCI device %d:%d.%d\n", device->bus, device->device, device->function);
}

// Get BAR size (useful for memory mapping)
static uint32_t pci_get_bar_size(pci_device_t* device, int bar_num) {
    if (!device || bar_num >= 6) return 0;
    
    uint8_t bar_offset = PCI_BAR0 + (bar_num * 4);
    uint32_t original_value = pci_config_read32(device->bus, device->device, device->function, bar_offset);
    
    // Write all 1s to the BAR
    pci_config_write32(device->bus, device->device, device->function, bar_offset, 0xFFFFFFFF);
    
    // Read back to get size mask
    uint32_t size_mask = pci_config_read32(device->bus, device->device, device->function, bar_offset);
    
    // Restore original value
    pci_config_write32(device->bus, device->device, device->function, bar_offset, original_value);
    
    // Calculate size
    if (size_mask == 0) return 0;
    
    return ~size_mask + 1;
}

#endif // PCIE_H