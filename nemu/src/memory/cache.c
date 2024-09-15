#include "cache.h"

uint64_t timer;
static int l1_of;
static CB_L1 l1_block[NR_CL1_BLOCK];
Cache_L1 cache_l1;

static int random_rep(void *cb_pool) {
    srand((unsigned)time(0));
    return rand() % ASSOC_CL1;
}

static CB *find_replace(hwaddr_t addr, int (*method)(void *)) {
    CB *p_cb,*dst_cb;
    p_cb = (CB *)cache_l1.assoc[GET_CI_L1(addr)];

    int dst,idx;
    for (idx = 0; idx < ASSOC_CL1; ++idx) {
        if (p_cb[idx].valid) continue;
        return &p_cb[idx];
    }

    dst = method(cache_l1.cb_pool);
    dst_cb = &p_cb[dst];
    return dst_cb;
}

static uint32_t cb_read(CB *this, uint8_t off, size_t len) {
    return (*(uint32_t *)(this->buf + off)) & ((1 << (len << 3)) - 1);
}

static void cb_write(CB *this, uint8_t off, uint8_t *data, size_t len) {
    memcpy(this->buf + off, data, len);
}

static CB *l1_check_hit(Cache *this, hwaddr_t addr) {
    int idx;
    CB *p_cb = (CB *)(((Cache_L1 *)this)->assoc[GET_CI_L1(addr)]);
    for (idx = 0; idx < ASSOC_CL1; ++idx) {
        if (p_cb[idx].valid && p_cb[idx].tag == GET_CT_L1(addr)) return (p_cb + idx);
    }
    return NULL;
}

static CB *l1_check_read_hit(Cache *this, hwaddr_t addr) {
    return l1_check_hit(this, addr);
}

static CB *l1_check_write_hit(Cache *this, hwaddr_t addr) {
    return l1_check_hit(this, addr);
}

static void l1_replace(Cache *this, hwaddr_t addr) {
    Log("REPLACED");
    CB *dst_cb;
    dst_cb = find_replace(addr, random_rep);
    dst_cb->tag = GET_CT_L1(addr);
    dst_cb->valid = 1;
    dst_cb->write(dst_cb, 0, hwa_to_va((addr & (~CO_L1_MASK))), CB_SIZE);

    l1_of = 0;
}

static void l1_read_replace(Cache *this, hwaddr_t addr) {
    l1_replace(this, addr);
}

static void l1_write_replace(Cache *this, hwaddr_t addr) {
    l1_replace(this, addr);
}

static uint32_t l1_read(Cache *this, hwaddr_t addr, size_t len, bool *hit) {
    uint32_t val;
    l1_of = GET_CO_L1(addr) + len - CB_SIZE + 1;
    l1_of = l1_of > 0 ? l1_of : 0;

    CB *cb,*cb_of = NULL;
    cb = this->check_read_hit((Cache *)&cache_l1, addr);
    if (l1_of) cb_of = this->check_read_hit((Cache *)&cache_l1, addr + len);
    if (((l1_of == 0) && (cb == NULL)) || ((l1_of > 0) && (cb == NULL || cb_of == NULL))) {
        Log("");
        *hit = false;
        return 0;
    }
    *hit = true;
    Log("");
    val = cb->read(cb, GET_CO_L1(addr), len - l1_of);
    if (l1_of) val += (cb_of->read(cb_of, 0, l1_of) << ((len - l1_of) << 3));

    l1_of = 0;
    return val;
}

static void l1_write(Cache *this, hwaddr_t addr, uint32_t data, size_t len, bool *hit) {
    l1_of = GET_CO_L1(addr) + len - CB_SIZE + 1;
    l1_of = l1_of > 0 ? l1_of : 0;
    CB *cb,*cb_of = NULL;
    cb = this->check_write_hit(this, addr);
    if (l1_of) cb_of = this->check_write_hit((Cache *)&cache_l1, addr + len);
    if (((l1_of == 0) && (cb == NULL)) || ((l1_of > 0) && (cb == NULL || cb_of == NULL))) {
        *hit = false;
        return;
    }
    *hit = true;

    uint8_t *of_data = ((uint8_t *)&data) + len - l1_of;
    cb->write(cb, GET_CO_L1(addr), (uint8_t *)&data, len - l1_of);
    if (l1_of) cb_of->write(cb_of, 0, of_data, l1_of);

    l1_of = 0;
}

static void cache_l1_init(Cache *this) {
    CACHE_INIT(1);
    CB_INIT(1);
    ((Cache_L1 *)this)->assoc = (CB_L1 (*)[8])l1_block;
}

void init_cache() {
    cache_l1.init = cache_l1_init;
    cache_l1.init((Cache *)&cache_l1);
}