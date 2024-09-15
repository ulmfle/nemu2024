#include "common.h"
#include "cache.h"

uint32_t dram_read(hwaddr_t, size_t);
void dram_write(hwaddr_t, size_t, uint32_t);
/* Memory accessing interfaces */

uint32_t hwaddr_read(hwaddr_t addr, size_t len) {
	uint32_t val;
	bool hit_l1=0;
	val = cache_l1.read((Cache *)&cache_l1, addr, len, &hit_l1);

	if (hit_l1 == 0) {
		val = dram_read(addr, len) & (~0u >> ((4 - len) << 3));
		cache_l1.read_replace((Cache *)&cache_l1, addr);
		if (l1_of) cache_l1.read_replace((Cache *)&cache_l1, addr + len);
	}
	return val;
}

void hwaddr_write(hwaddr_t addr, size_t len, uint32_t data) {
	bool hit_l1;
	cache_l1.write((Cache *)&cache_l1, addr, data, len, &hit_l1);
	dram_write(addr, len, data);	//write through and not write allocate
}

uint32_t lnaddr_read(lnaddr_t addr, size_t len) {
	return hwaddr_read(addr, len);
}

void lnaddr_write(lnaddr_t addr, size_t len, uint32_t data) {
	hwaddr_write(addr, len, data);
}

uint32_t swaddr_read(swaddr_t addr, size_t len) {
#ifdef DEBUG
	assert(len == 1 || len == 2 || len == 4);
#endif
	return lnaddr_read(addr, len);
}

void swaddr_write(swaddr_t addr, size_t len, uint32_t data) {
#ifdef DEBUG
	assert(len == 1 || len == 2 || len == 4);
#endif
	lnaddr_write(addr, len, data);
}

