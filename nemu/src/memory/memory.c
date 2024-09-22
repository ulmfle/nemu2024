#include "common.h"
#include "cpu/reg.h"
#include "memory/memory.h"

uint32_t cache_read(hwaddr_t, size_t, bool *);
void cache_replace(hwaddr_t, size_t);
void cache_write(hwaddr_t, uint32_t, size_t);
uint32_t dram_read(hwaddr_t, size_t);
void dram_write(hwaddr_t, size_t, uint32_t);
lnaddr_t seg_translate(swaddr_t, size_t, uint8_t);
void load_desc(uint8_t sreg);
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
	return hwaddr_read(addr, len);
}

void lnaddr_write(lnaddr_t addr, size_t len, uint32_t data) {
	Log("addr %08x len : %u data : %08x", addr, (unsigned)len, data);
	hwaddr_write(addr, len, data);
}

uint32_t swaddr_read(swaddr_t addr, size_t len, uint8_t sreg) {
#ifdef DEBUG
	assert(len == 1 || len == 2 || len == 4);
#endif
	lnaddr_t lnaddr = cpu.cr0.protect_enable ? seg_translate(addr, len, sreg) : addr;
	return lnaddr_read(lnaddr, len);
}

void swaddr_write(swaddr_t addr, size_t len, uint8_t sreg, uint32_t data) {
#ifdef DEBUG
	assert(len == 1 || len == 2 || len == 4);
#endif
	lnaddr_t lnaddr = cpu.cr0.protect_enable ? seg_translate(addr, len, sreg) : addr;
	lnaddr_write(lnaddr, len, data);
}

lnaddr_t seg_translate(swaddr_t addr, size_t len, uint8_t sreg) {
	if (!cpu.sr[sreg].hid_desc.seg_present) {
		load_desc(sreg);
		cpu.sr[sreg].hid_desc.seg_present = 1;
	}
	return sr_base(sreg) + addr;
}

hwaddr_t page_translate(lnaddr_t addr) {
	return addr;
}

void load_desc(uint8_t sreg) {
	memcpy((void *)&cpu.sr[sreg].hid_desc, hwa_to_va(page_translate(cpu.gdtr.LBA + sizeof(descriptor)*cpu.sr[sreg].sel.index)), sizeof(descriptor));
}