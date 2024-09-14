#include "common.h"
#include "cache.h"

uint32_t dram_read(hwaddr_t, size_t);
void dram_write(hwaddr_t, size_t, uint32_t);
/* Memory accessing interfaces */

uint32_t hwaddr_read(hwaddr_t addr, size_t len) {
	uint32_t val;
	int of = GET_CO_L1(addr) + len - CB_SIZE + 1;
	of = of > 0 ? of : 0;
	bool hit_l = false, hit_r = true;

	val = cache_l1.read(&cache_l1, addr, len - of, &hit_l) << of;
	if (of) val += cache_l1.read(&cache_l1, addr + len - of + 1, of, &hit_r) & ((1 << of) - 1);
	if (hit_l == false || hit_r == false) {
		val = dram_read(addr, len) & (~0u >> ((4 - len) << 3));
		cache_l1.replace(&cache_l1, addr);
		if (of) cache_l1.replace(&cache_l1, addr + len);
	}
	return val;
}

void hwaddr_write(hwaddr_t addr, size_t len, uint32_t data) {
	int of = GET_CO_L1(addr) + len - CB_SIZE + 1;
	of = of > 0 ? of : 0;

	bool hit_l, hit_r;
	cache_l1.write(&cache_l1, addr, data, len - of, &hit_l);
	if (of) cache_l1.write(&cache_l1, addr + len - of + 1, *(&data + len - of), of, &hit_r);

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

