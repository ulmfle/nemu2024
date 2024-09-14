#include "cache.h"

#ifdef DEBUG
void show_all(uint8_t *data, size_t len) {
    while (len--) printf("%d ", *data++); printf("\n");
}
#endif

static CB_L1 cl1_block[NR_CL1_BLOCK];

Cache_L1 cache_l1;

static int random_rep(void *_cb_pool) {
    srand((unsigned)time(0));
    return rand() % ASSOC_CL1;
}

static uint32_t cb_l1_read(void *this, uint8_t off, size_t len) {
    return *(uint32_t *)((((CB_L1 *)this)->buf) + off) & (~((~0u) << (8*len)));
}

static void cb_l1_write(void *this, uint8_t off, uint8_t *data, size_t len) {
    uint8_t *dst = (uint8_t *)(((CB_L1 *)this)->buf);
    int idx;
    for (idx = off; idx < off + len; ++idx) {
        dst[idx] = data[idx];
    }
}

static void *check_l1_hit(void *this, hwaddr_t addr) {
    int idx;
    CB_L1 *p_cb = ((Cache_L1 *)this)->assoc[GET_CI_L1(addr)];
    for (idx = 0; idx < ASSOC_CL1; ++idx) {
        if (p_cb[idx].valid && p_cb[idx].tag == GET_CT_L1(addr)) return (void *)(p_cb + idx);
    }
    return NULL;
}

static void *l1_replace(void *this, hwaddr_t addr) {
    Log("idx:%d, tag:%d", GET_CI_L1(addr), GET_CT_L1(addr));
    int dst = random_rep(((Cache_L1 *)this)->cb_pool);
    CB_L1 *dst_cb = &(((Cache_L1 *)this)->assoc[GET_CI_L1(addr)][dst]);
    dst_cb->tag = GET_CT_L1(addr);
    dst_cb->valid = 1;
    dst_cb->write(dst_cb, 0, hwa_to_va(addr - GET_CO_L1(addr)), CB_SIZE);   //attention
    return dst_cb;
}

static uint32_t l1_read(void *this, hwaddr_t addr, size_t len, bool *hit) {
    CB_L1 *cb = (CB_L1 *)(((Cache_L1 *)this)->check_hit(&cache_l1, addr));
    if (cb != NULL) {
        *hit = true;
        return cb->read(cb, GET_CO_L1(addr), len);
    }
    *hit = false;
    return 0;

}

static void l1_write(void *this, hwaddr_t addr, uint32_t data, size_t len, bool *hit) {
    CB_L1 *cb = (CB_L1 *)(((Cache_L1 *)this)->check_hit(this, addr));
    if (cb != NULL) {
        *hit = true;
        cb->write(cb, GET_CO_L1(addr), (uint8_t *)&data, len);
    } else {
        *hit = false;
    }
}

static void l1_init(void *this) {
    ((Cache_L1 *)this)->cb_pool = (void *)cl1_block;
    ((Cache_L1 *)this)->check_hit = check_l1_hit;
    ((Cache_L1 *)this)->replace = l1_replace;
    ((Cache_L1 *)this)->read = l1_read;
    ((Cache_L1 *)this)->write = l1_write;
    ((Cache_L1 *)this)->assoc = (CB_L1 (*)[8])(((Cache_L1 *)this)->cb_pool);
    int idx;
    for (idx = 0; idx < NR_CL1_BLOCK; ++idx) {
        cl1_block[idx].valid = 0;
        cl1_block[idx].read = cb_l1_read;
        cl1_block[idx].write = cb_l1_write;
    }
}

void init_cache() {
    cache_l1.init = l1_init;
    cache_l1.init(&cache_l1);
}