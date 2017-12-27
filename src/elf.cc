/*
 * guzhoudiaoke@126.com
 * 2017-12-23
 */

#include "elf.h"
#include "babyos.h"
#include "ide.h"
#include "string.h"

static uint32 log(int32 x, int32 n)
{
    int32 ret = 0, num = 1;
    while (num < n) {
        num *= x;
        ret++;
    }

    return ret;
}

static int32 read_hard_disk(uint8* buffer, uint32 lba, uint32 sector_num)
{

    for (uint8* b = buffer; b < buffer + SECT_SIZE*sector_num; b += SECT_SIZE, lba++) {
        io_clb_t clb_elf;
        clb_elf.init(0, 1, lba);

        memset(clb_elf.buffer, 0, SECT_SIZE);
        os()->get_ide()->request(&clb_elf);
        memcpy(b, clb_elf.buffer, SECT_SIZE);
    }

    return 0;
}

int32 load_elf_segments(elf_hdr_t* elf)
{
    pde_t* pg_dir = current->m_vmm.get_pg_dir();
    uint8 *base = (uint8 *) elf;

    prog_hdr_t *ph = (prog_hdr_t *)(base + elf->phoff);
    prog_hdr_t *end_ph = ph + elf->phnum;
    for (; ph < end_ph; ph++) {
        if (ph->type != PT_LOAD) {
            continue;
        }
        void* vaddr = (void*) (ph->vaddr & PAGE_MASK);
        uint32 offset = ph->vaddr - (uint32)vaddr;
        uint32 len = PAGE_ALIGN(ph->memsz + ((uint32)vaddr & (PAGE_SIZE-1)));
        uint32 pagenum = len / PAGE_SIZE;

        //console()->kprintf(WHITE, "lba: %u, ph->vaddr: %p, vaddr: %p, offset: %x, len: %p, msize: %x, fsize: %x\n", 
        //        lba, ph->vaddr, vaddr, offset, len, ph->memsz, ph->filesz);

        int32 ret = current->m_vmm.do_mmap((uint32) vaddr, len, PROT_READ | PROT_WRITE | PROT_EXEC, MAP_FIXED);
        if (ret < 0) {
            console()->kprintf(RED, "BUG on do_mmap!\n");
            return -1;
        }
        void* mem = os()->get_mm()->alloc_pages(log(2, pagenum));
        os()->get_mm()->map_pages(pg_dir, vaddr, VA2PA(mem), len, PTE_W | PTE_U);
        memcpy(mem+offset, base+ph->off, ph->filesz);
        if (ph->memsz > ph->filesz) {
            memset(mem+offset+ph->filesz, 0, ph->memsz - ph->filesz);
        }
    }

    return 0;
}

int32 elf_t::load(trap_frame_t* frame)
{
    uint32 lba = frame->ebx;
    uint32 sector_num = frame->ecx;

    // read elf from hard disk
    uint32 order = log(2, PAGE_ALIGN(sector_num * SECT_SIZE) / PAGE_SIZE);
    uint8* buffer = (uint8*) os()->get_mm()->alloc_pages(order);
    read_hard_disk(buffer, lba, sector_num);
    elf_hdr_t *elf = (elf_hdr_t *) (buffer);

    // 3. check if it's a valid elf file
    if (elf->magic != ELF_MAGIC) {
        console()->kprintf(RED, "BUG on elf format!\n");
        return -1;
    }

    // load program segments
    if (load_elf_segments(elf) != 0) {
        console()->kprintf(RED, "BUG on load elf program segments!\n");
        return -1;
    }

    // eip
    frame->eip = (uint32)(elf->entry);

    // free buffer pages
    os()->get_mm()->free_pages(buffer, order);

    return 0;
}

