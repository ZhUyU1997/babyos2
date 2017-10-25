/*
 * guzhoudiaoke@126.com
 * 2017-10-23
 */

#include "kernel.h"
#include "mm.h"
#include "x86.h"
#include "console.h"
#include "string.h"

__attribute__ ((__aligned__(PAGE_SIZE)))
uint8 kernel_stack[KSTACK_SIZE] = {
    0xff,
};

/* pg_dir and pte for entry */
__attribute__ ((__aligned__(PAGE_SIZE)))
pte_t entry_pg_table0[NR_PTE_PER_PAGE] = { 
    [0] = (0) | PTE_P | PTE_W,
};
__attribute__ ((__aligned__(PAGE_SIZE)))
pte_t entry_pg_table_vram[NR_PTE_PER_PAGE] = {
    [0] = (0) | PTE_P | PTE_W,
};

__attribute__ ((__aligned__(PAGE_SIZE)))
pde_t entry_pg_dir[1024] = { 
    [0] = (0) | PTE_P | PTE_W,
};

pde_t *kernel_pg_dir = NULL;

uint8 *mem_start = NULL;
uint8 *mem_end = NULL;

uint32 usable_phy_mem_start = 0;
uint32 usable_phy_mem_end = 0;

extern uint8 data[];    // defined by kernel.ld
extern uint8 end[];     // defined by kernel.ld

void map_pages(pde_t *pg_dir, void *va, uint32 pa, uint32 size, uint32 perm)
{
    uint8 *v = (uint8 *) (((uint32)va) & PAGE_MASK);
    uint8 *e = (uint8 *) (((uint32)va + size) & PAGE_MASK);
    
	pde_t *pde = &pg_dir[PD_INDEX(va)];
	pte_t *pg_table;
	while (v < e) {
        if ((*pde) & PTE_P) {
			pg_table = (pte_t *)(PA2VA((*pde) & PAGE_MASK));
		}
		else {
            pg_table = (pte_t *)mem_start;
			memset(pg_table, 0, PAGE_SIZE);
            mem_start += PAGE_SIZE;
            *pde = (VA2PA(pg_table) | PTE_P | PTE_W);
		}

		pde++;
		for (int i = PT_INDEX(v); i < NR_PTE_PER_PAGE && v < e; i++, v += PAGE_SIZE) {
			pte_t *pte = &pg_table[i];
			if (v < e) {
				*pte = VA2PA(v) | PTE_P | perm;
			}
		}
	}
}

/* FIXME: only used for test now */
void *screen_va = NULL;

void test_page_mapping()
{
	uint32 total = 0;
	for (uint8 *v = (uint8 *)KERNEL_BASE; v < mem_end; v += 1*KB) {
		pde_t *pde = &kernel_pg_dir[PD_INDEX(v)];

        if ((*pde) & PTE_P) {
			pte_t *pg_table = (pte_t *)(PA2VA(((*pde) & PAGE_MASK)));
			pte_t *pte = &pg_table[PT_INDEX(v)];
			if (!((*pte) & PTE_P)) {
				kprintf(WHITE, "page fault: v: 0x%p, *pde: 0x%p, *pte: 0x%p\n", v, *pde, *pte);
				break;
			}
		}
		else {
			kprintf(WHITE, "page fault2: v: 0x%p, *pde: 0x%p\n", v, *pde);
			break;
		}

		uint8 x = *v;
		total += x;
	}
}

void init_paging()
{
    mem_start = (uint8 *)PAGE_ALIGN(mem_start);

    // mem for kernel_pg_dir
    kernel_pg_dir = (pde_t *)mem_start;
    mem_start += PAGE_SIZE;
    memset(kernel_pg_dir, 0, PAGE_SIZE);

    // first 1MB: KERNEL_BASE ~ KERNEL_LOAD -> 0~1M
    map_pages(kernel_pg_dir, (uint8 *)KERNEL_BASE, 0, EXTENED_MEM, PTE_W);

    // kernel text + rodata: KERNEL_LOAD ~ data -> 1M ~ VA2PA(data)
    map_pages(kernel_pg_dir, (uint8 *)KERNEL_LOAD, VA2PA(KERNEL_LOAD), VA2PA(data) - VA2PA(KERNEL_LOAD), 0);

    // kernel data + memory: data ~ KERNEL_BASE+MAX_PHY_MEM -> VA2PA(data) ~ MAX_PHY_MEM
    map_pages(kernel_pg_dir, data, VA2PA(data), VA2PA(mem_end) - VA2PA(data), PTE_W);

	// FIXME: map the video vram mem, just for test now
    kernel_pg_dir[((uint32)screen_va)>>22] = ((uint32)(VA2PA(entry_pg_table_vram)) | (PTE_P | PTE_W));

    set_cr3(VA2PA(kernel_pg_dir));

    // FIXME: debug
	test_page_mapping();
}

#if 0
/* This method can only get Memory less than 64MB */
static uint32 i386_detect_memory()
{
	uint32 npages_base = (nvram_read(NVRAM_BASELOW) * 1024) / PAGE_SIZE;
	uint32 npages_ext = (nvram_read(NVRAM_EXTLOW) * 1024) / PAGE_SIZE;
	uint32 ext_ = (nvram_read(0x30) * 1024) / PAGE_SIZE;

	uint32 npages = 0;
	if (npages_ext != 0) {
		npages = (EXTENED_MEM / PAGE_SIZE) + npages_ext;
	}
	else {
		npages = npages_base;
	}

	kprintf(WHITE, "npages_base: %u, npages_ext: %u, ext_: %u, npages: %u\n", npages_base, npages_ext, ext_, npages);
	return npages * PAGE_SIZE;
}
#endif

void init_mem_range()
{
    boot_memory_info_t *info = (boot_memory_info_t *) PA2VA(MEMORY_INFO_ADDR);
	kprintf(WHITE, "the memory info from int 0x15, eax=0xe820:\n");
	kprintf(WHITE, "type\t\taddress\t\tlength\n");
	for (uint32 i = 0; i < info->num_of_range; i++) {
		kprintf(RED, "%x\t%x\t%x\n", 
                info->ranges[i].type,
                info->ranges[i].base_addr_low,
                info->ranges[i].base_addr_low + info->ranges[i].length_low);

        /* I am not sure how to get the total memory, and I don't know why when I set the machine with 128MB
         * memory, this method get a range 1MB ~ 127MB with type 1 (Usable (normal) RAM, 
         * and 127MB~128MB with type 2(Reserved - unusable)
         * for now, get the range of address >= 1M and type == 1 (Usable (normal) RAM */
        if (info->ranges[i].type == 1 && info->ranges[i].base_addr_low >= 1*MB) {
            usable_phy_mem_start = info->ranges[i].base_addr_low;
            usable_phy_mem_end = info->ranges[i].base_addr_low + info->ranges[i].length_low;
        }
	}

    kprintf(WHITE, "usable memory above 1MB: from %uMB, to %dMB\n", 
            usable_phy_mem_start / (1*MB), usable_phy_mem_end / (1*MB));

    mem_start = end;
    mem_end = (uint8 *)PA2VA(usable_phy_mem_end);
	kprintf(WHITE, "mem_start: 0x%x, mem_end: 0x%x\n", mem_start, mem_end);
}

void init_free_area()
{
}

void init_mm()
{
    init_mem_range();
    init_paging();
    init_free_area();
}

/* FIXME: map the video vram mem, just for test now */
void kmap_device(void *va)
{
	screen_va = va;

    entry_pg_dir[((uint32)va)>>22] = ((uint32)(VA2PA(entry_pg_table_vram)) | (PTE_P | PTE_W));
    uint32 entry = ((uint32) va) | (PTE_P | PTE_W);
    for (uint32 i = 0; i < NR_PTE_PER_PAGE; i++) {
        entry_pg_table_vram[i] = entry;
        entry += PAGE_SIZE;
    }
}
