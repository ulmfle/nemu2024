#include "common.h"

uint32_t cache_read(hwaddr_t, size_t, bool *);
void cache_replace(hwaddr_t);
void cache_write(hwaddr_t, uint32_t, size_t);
uint32_t dram_read(hwaddr_t, size_t);
void dram_write(hwaddr_t, size_t, uint32_t);
/* Memory accessing interfaces */

uint32_t hwaddr_read(hwaddr_t addr, size_t len) {
	uint32_t val;
	bool hit;
	val = cache_read(addr, len, &hit);
	if (hit == false) {
		val = dram_read(addr, len) & (~0u >> ((4 - len) << 3));
		cache_replace(addr);
	}
	//Log("final val: 0x%08x", val);
	return val;
}

void hwaddr_write(hwaddr_t addr, size_t len, uint32_t data) {
	Log("addr: 0x%08x val: 0x%08x len:%d",addr, data, (unsigned)len);
	//dram_write(addr, len, data);
	cache_write(addr, data, len);
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

