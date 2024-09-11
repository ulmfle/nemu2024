#include "common.h"
#include "memory.h"
#include <string.h>
#include <elf.h>

#define ELF_OFFSET_IN_DISK 0

#ifdef HAS_DEVICE
void ide_read(uint8_t *, uint32_t, uint32_t);
#else
void ramdisk_read(uint8_t *, uint32_t, uint32_t);
void ramdisk_write(uint8_t *, uint32_t, uint32_t);
#endif

#define STACK_SIZE (1 << 20)

void create_video_mapping();
uint32_t get_ucr3();

uint32_t loader() {
	Elf32_Ehdr *elf;
	Elf32_Phdr *ph = NULL;

	uint8_t buf[4096];

#ifdef HAS_DEVICE
	ide_read(buf, ELF_OFFSET_IN_DISK, 4096);
#else
	ramdisk_read(buf, ELF_OFFSET_IN_DISK, 4096);
#endif

	elf = (void*)buf;

	/* TODO: fix the magic number with the correct one */
	const uint32_t elf_magic = 0x464c457f;
	uint32_t *p_magic = (void *)buf;
	nemu_assert(*p_magic == elf_magic);

	volatile uint32_t entry = elf->e_entry;

	/* Load each program segment */
	int nr_ph = elf->e_phnum;
	int phe_size = elf->e_phentsize;

#ifdef HAS_DEVICE
	ide_read(buf, ELF_OFFSET_IN_DISK + elf->e_phoff, nr_ph * phe_size);
#else
	ramdisk_read(buf, ELF_OFFSET_IN_DISK + elf->e_phoff, nr_ph * phe_size);
#endif

	ph = (void *)buf;
	int ph_idx;
	for (ph_idx = 0; ph_idx < nr_ph; ++ph_idx) {
		/* Scan the program header table, load each segment into memory */
		if(ph[ph_idx].p_type == PT_LOAD) {
			/* TODO: read the content of the segment from the ELF file
			 * to the memory region [VirtAddr, VirtAddr + FileSiz)
			 */
#ifdef HAS_DEVICE
			ide_write((uint8_t*)buf + ph[ph_idx].p_offset, ph[ph_idx].p_vaddr, ph[ph_idx].p_filesz);
#else
			ramdisk_write((uint8_t*)buf + ph[ph_idx].p_offset, ph[ph_idx].p_vaddr, ph[ph_idx].p_filesz);
#endif
			/* TODO: zero the memory region
			 * [VirtAddr + FileSiz, VirtAddr + MemSiz)
			 */
			memset((void *)ph[ph_idx].p_vaddr + ph[ph_idx].p_filesz, 0, ph[ph_idx].p_memsz - ph[ph_idx].p_filesz);

#ifdef IA32_PAGE
			/* Record the program break for future use. */
			extern uint32_t cur_brk, max_brk;
			uint32_t new_brk = ph->p_vaddr + ph->p_memsz - 1;
			if(cur_brk < new_brk) { max_brk = cur_brk = new_brk; }
#endif
		}
	}

	// volatile uint32_t entry = elf->e_entry;

#ifdef IA32_PAGE
	mm_malloc(KOFFSET - STACK_SIZE, STACK_SIZE);

#ifdef HAS_DEVICE
	create_video_mapping();
#endif

	write_cr3(get_ucr3());
#endif

	return entry;
}
