#include "common.h"
#include "cpu/reg.h"

uint32_t cache_read(hwaddr_t, size_t, bool *);
void cache_replace(hwaddr_t, size_t);
void cache_write(hwaddr_t, uint32_t, size_t);
uint32_t dram_read(hwaddr_t, size_t);
void dram_write(hwaddr_t, size_t, uint32_t);
lnaddr_t seg_translate(swaddr_t, size_t, uint8_t);
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
	Log();
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
	uint8_t temp[4];
	uint64_t l = lnaddr_read(cpu.gdtr.LBA + sizeof(descriptor)*cpu.sr[sreg].sel.index, 4);
	memcpy((void *)temp, &l, 4);

	descriptor *desc = (void *)temp;
	
	return desc->seg_base + addr;
}