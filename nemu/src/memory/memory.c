#include "common.h"
#include "cpu/reg.h"
#include "memory/memory.h"

uint32_t cache_read(hwaddr_t, size_t, bool *);
void cache_replace(hwaddr_t, size_t);
void cache_write(hwaddr_t, uint32_t, size_t);
uint32_t dram_read(hwaddr_t, size_t);
void dram_write(hwaddr_t, size_t, uint32_t);
lnaddr_t seg_translate(swaddr_t, uint8_t);
hwaddr_t page_translate(lnaddr_t);
void load_desc(uint8_t, uint16_t);
/* Memory accessing interfaces */

uint32_t hwaddr_read(hwaddr_t addr, size_t len) {
	uint32_t val;
	bool hit;
	val = cache_read(addr, len, &hit);

	if (hit == false) {
		val = dram_read(addr, len) & (~0u >> ((4 - len) << 3));
		cache_replace(addr, len);
	}
	return val;
}

void hwaddr_write(hwaddr_t addr, size_t len, uint32_t data) {
	// dram_write(addr, len, data);
	cache_write(addr, data, len);
}

uint32_t lnaddr_read(lnaddr_t addr, size_t len) {
	if ((addr + len - 1) % PAGE_SIZE < addr % PAGE_SIZE) assert(0);
	return hwaddr_read(page_translate(addr), len);
}

void lnaddr_write(lnaddr_t addr, size_t len, uint32_t data) {
	if ((addr + len - 1) % PAGE_SIZE < addr % PAGE_SIZE) assert(0);
	hwaddr_write(page_translate(addr), len, data);
}

uint32_t swaddr_read(swaddr_t addr, size_t len, uint8_t sreg) {
#ifdef DEBUG
	assert(len == 1 || len == 2 || len == 4);
#endif
	return lnaddr_read(seg_translate(addr, sreg), len);
}

void swaddr_write(swaddr_t addr, size_t len, uint8_t sreg, uint32_t data) {
#ifdef DEBUG
	assert(len == 1 || len == 2 || len == 4);
#endif
	lnaddr_write(seg_translate(addr, sreg), len, data);
}

lnaddr_t seg_translate(swaddr_t addr, uint8_t sreg) {
	return cpu.cr0.protect_enable ? sr_base(sreg) + addr : addr;
}

hwaddr_t page_translate(lnaddr_t addr) {
	if (!(cpu.cr0.protect_enable && cpu.cr0.paging)) return addr;
	PDE pde; PTE pte;
	Log("base: %08x", cpu.cr3.page_directory_base << 12);
	memcpy((void *)&pde, hwa_to_va(cpu.cr3.page_directory_base + sizeof(uint32_t)*((addr & (~(~0u >> 10))) >> 22)), sizeof(uint32_t));
	assert(pde.present);
	memcpy((void *)&pte, hwa_to_va(pde.page_frame + sizeof(uint32_t)*((addr >> 12) & ~(~0u << 10))), sizeof(uint32_t));
	assert(pte.present);
	return pte.page_frame + (addr & PAGE_MASK);
}

void load_desc(uint8_t sreg, uint16_t _sel) {
	cpu.sr[sreg].sel.val = _sel;
	memcpy((void *)&(cpu.sr[sreg].hid_desc), hwa_to_va(page_translate(cpu.gdtr.LBA + sizeof(uint64_t)*cpu.sr[sreg].sel.index)), sizeof(uint64_t));
}