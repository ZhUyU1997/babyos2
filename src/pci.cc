/*
 * guzhoudiaoke@126.com
 * 2018-03-01
 */

#include "babyos.h"
#include "x86.h"

#define PCI_CONFIG_ADDR                 0xCF8
#define PCI_CONFIG_DATA                 0xCFC

#define PCI_CONFIG_VENDOR               0x00
#define PCI_CONFIG_COMMAND              0x04
#define PCI_CONFIG_CLASS_REV            0x08
#define PCI_CONFIG_HDR_TYPE             0x0C
#define PCI_CONFIG_BASE_ADDR0           0x10
#define PCI_CONFIG_INTR                 0x3C

#define PCI_BASE_ADDR_MEM_MASK           (~0x0FUL)
#define PCI_BASE_ADDR_IO_MASK            (~0x03UL)

void pci_device_bar_t::init(uint32 addr_reg_val, uint32 len_reg_val)
{
    if (addr_reg_val == 0xffffffff) {
        addr_reg_val = 0;
    }

    if (addr_reg_val & 1) {
        m_type = TYPE_IO;
        m_base_addr = addr_reg_val  & PCI_BASE_ADDR_IO_MASK;
        m_length    = ~(len_reg_val & PCI_BASE_ADDR_IO_MASK) + 1;
    }
    else {
        m_type = TYPE_MEM;
        m_base_addr = addr_reg_val  & PCI_BASE_ADDR_MEM_MASK;
        m_length    = ~(len_reg_val & PCI_BASE_ADDR_MEM_MASK) + 1;
    }
}

void pci_device_bar_t::dump()
{
    console()->kprintf(PINK, "type: %s\t", m_type == TYPE_IO ? "io base address" : "mem base address");
    console()->kprintf(PINK, "base address: %x\t", m_base_addr);
    console()->kprintf(PINK, "len: %x\n", m_length);
}

void pci_device_t::init(uint16 vendor_id, uint16 device_id, uint32 class_code, uint8 revision, bool multi_function)
{
    m_vendor_id = vendor_id;
    m_device_id = device_id;
    m_multi_function = multi_function;
    m_class_code = class_code;
    m_revision = revision;

    for (int i = 0; i < 6; i++) {
        m_bar[i].m_type = pci_device_bar_t::TYPE_INVALID;
    }
    m_interrupt_line = -1;
}

void pci_device_t::dump()
{
    console()->kprintf(CYAN, "vendor id:      %x\n", m_vendor_id);
    console()->kprintf(CYAN, "device id:      %x\n", m_device_id);
    console()->kprintf(CYAN, "class code:     %x\n", m_class_code);
    console()->kprintf(CYAN, "revision:       %x\n", m_revision);
    console()->kprintf(CYAN, "multi function: %u\n", m_multi_function);
    console()->kprintf(CYAN, "interrupt line: %u\n", m_interrupt_line);
    for (int i = 0; i < 6; i++) {
        if (m_bar[i].m_type != pci_device_bar_t::TYPE_INVALID) {
            console()->kprintf(CYAN, "bar %u: \n", i);
            m_bar[i].dump();
        }
    }
}

/*****************************************************************/

void pci_t::init()
{
    console()->kprintf(GREEN, "\n");
    console()->kprintf(GREEN, "********************** init pci **********************\n");
    enum_buses();
    console()->kprintf(GREEN, "********************** init pci **********************\n");
    console()->kprintf(GREEN, "\n");
}

void pci_t::enum_buses()
{
    for (uint16 bus = 0; bus < 256; bus++) {
        for (uint8 device = 0; device < 32; device++) {
            check_device(bus, device);
        }
    }
}

void pci_t::check_device(uint8 bus, uint8 device)
{
    uint8 function = 0;

    uint32 val = read(bus, device, function, PCI_CONFIG_VENDOR);
    uint16 vendor_id = val & 0xffff;
    uint16 device_id = val >> 16;
    if (vendor_id == 0xffff) {
        return;
    }

    val = read(bus, device, function, PCI_CONFIG_HDR_TYPE);
    uint8 header_type = ((val >> 16));

    val = read(bus, device, function, PCI_CONFIG_COMMAND);
    uint16 command = val & 0xffff;
    console()->kprintf(BLUE, "command: %x\n", command);

    val = read(bus, device, function, PCI_CONFIG_CLASS_REV);
    uint32 classcode = val >> 8;
    uint8 revision = val & 0xff;

    pci_device_t pci_device;
    pci_device.init(vendor_id, device_id, classcode, revision, (header_type & 0x80));

    for (int bar = 0; bar < 6; bar++) {
        int reg = PCI_CONFIG_BASE_ADDR0 + (bar*4);
        val = read(bus, device, function, reg);
        write(bus, device, function, reg, 0xffffffff);
        uint32 len = read(bus, device, function, reg);
        write(bus, device, function, reg, val);

        if (len != 0 && len != 0xffffffff) {
            pci_device.m_bar[bar].init(val, len);
        }
    }

    val = read(bus, device, function, PCI_CONFIG_INTR);
    if ((val & 0xff) > 0 && (val & 0xff) < 32) {
        uint32 irq = val & 0xff;
        pci_device.m_interrupt_line = irq;
    }

    console()->kprintf(YELLOW, "pci device at bus: %u, device: %u\n", bus, device);
    pci_device.dump();
}

uint32 pci_t::read(uint32 bus, uint32 device, uint32 function, uint32 addr)
{
    outl(PCI_CONFIG_ADDR, ((uint32) 0x80000000 | (bus << 16) | (device << 11) | (function << 8) | addr));
    return inl(PCI_CONFIG_DATA);
}

void pci_t::write(uint32 bus, uint32 device, uint32 function, uint32 addr, uint32 val)
{
    outl(PCI_CONFIG_ADDR, ((uint32) 0x80000000 | (bus << 16) | (device << 11) | (function << 8) | addr));
    outl(PCI_CONFIG_DATA, val);
}

