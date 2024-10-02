#include "common.h"
#include "memory/memory.h"
#include <stdlib.h>
#include <time.h>       //for random

#define CB_SIZE_WIDTH 6
#define NR_CL1_BLOCK_WIDTH 10
#define NR_CL2_BLOCK_WIDTH 12
#define ASSOC_CL1_WIDTH 3
#define ASSOC_CL2_WIDTH 4
#define TAG_CL1_WIDTH (32 - CB_SIZE_WIDTH - NR_CL1_BLOCK_WIDTH + ASSOC_CL1_WIDTH)
#define TAG_CL2_WIDTH (32 - CB_SIZE_WIDTH - NR_CL2_BLOCK_WIDTH + ASSOC_CL2_WIDTH)
//#define TAG_WIDTH(level) (32 - CB_SIZE_WIDTH - concat3(NR_CL, level, _BLOCK_WIDTH) + concat3(ASSOC_CL, level, _WIDTH))
#define TAG_WIDTH(level) concat3(TAG_CL, level, _WIDTH)

#define CB_SIZE (1 << CB_SIZE_WIDTH)
#define NR_CL1_BLOCK (1 << NR_CL1_BLOCK_WIDTH)
#define NR_CL2_BLOCK (1 << NR_CL2_BLOCK_WIDTH)
#define ASSOC_CL1 (1 << ASSOC_CL1_WIDTH)
#define ASSOC_CL2 (1 << ASSOC_CL2_WIDTH)

#define CT_MASK(level) (~0u << (32 - TAG_WIDTH(level)))
#define CO_MASK (CB_SIZE - 1)
#define CI_MASK(level) (((~0u) ^ (CT_MASK(level))) ^ (CO_MASK))

#define GET_CT(addr, level) (((addr) & CT_MASK(level)) >> (32 - TAG_WIDTH(level)))
#define GET_CI(addr, level) (((addr) & CI_MASK(level)) >> CB_SIZE_WIDTH)
#define GET_CO(addr) ((addr) & CO_MASK)

#define ASSOC(level) ((CB (*)[concat(ASSOC_CL, level)])(this->cb_pool))

typedef struct CB {
    uint8_t buf[CB_SIZE];
    uint32_t tag;
    bool valid;
    bool dirty;
    uint32_t (*read)(struct CB *, uint8_t, size_t);
    void (*write)(struct CB *, uint8_t, uint8_t *, size_t);
} CB;

typedef struct Cache {
    void *cb_pool;
    
    CB *(*check_read_hit)(struct Cache *, hwaddr_t);
    CB *(*check_write_hit)(struct Cache *, hwaddr_t);
    void (*read_replace)(struct Cache *, hwaddr_t);
    void (*write_replace)(struct Cache *, hwaddr_t);
    uint32_t (*read)(struct Cache *, hwaddr_t, size_t, bool *);
    void (*write)(struct Cache *, hwaddr_t, uint32_t, size_t, bool *);
} Cache;

static CB l1_block[NR_CL1_BLOCK];
static CB l2_block[NR_CL2_BLOCK];
Cache l1, l2;

//stand-alone
static CB *normal_check_hit(CB *cb_lst, size_t len, uint32_t _tag) {
    int idx;
    for (idx = 0; idx < len; ++idx) {
        if (cb_lst[idx].valid && cb_lst[idx].tag == _tag) return (cb_lst + idx);
    }
    return NULL;
}

//stand-alone
static CB *normal_find_replace(CB *cb_lst, size_t len) {
    CB *dst_cb = NULL;
    int idx;
    for (idx = 0; idx < len; ++idx) {
        if (cb_lst[idx].valid) continue;
        dst_cb = cb_lst + idx;
        break;
    }
    if (dst_cb == NULL) {
        srand((unsigned)time(NULL));
        dst_cb = cb_lst + (rand() % len);
    }
    return dst_cb;
}

//base
static uint32_t cbread(CB *this, uint8_t off, size_t len) {
    return (*(uint32_t *)(this->buf + off)) & (~0u >> ((4 - len) << 3));
}

//base
static void cbwrite(CB *this, uint8_t off, uint8_t *data, size_t len) {
    memcpy(this->buf + off, data, (unsigned)len);
}

//base
static uint32_t cread(Cache *this, hwaddr_t addr, size_t len, bool *hit) {
    CB *cb = this->check_read_hit(this, addr);
    if (cb == NULL) {
        *hit = false;
        return 0;
    }
    *hit = true;
    return cb->read(cb, GET_CO(addr), len);
}

//base
static void cwrite(Cache *this, hwaddr_t addr, uint32_t data, size_t len, bool *hit) {
    CB *cb = this->check_write_hit(this, addr);
    if (cb == NULL) {
        *hit = false;
        return;
    }
    *hit = true;
    cb->write(cb, GET_CO(addr), (uint8_t *)&data, len);
}


static CB *l1_check_read_hit(Cache *this, hwaddr_t addr) {
    return normal_check_hit(ASSOC(1)[GET_CI(addr, 1)], ASSOC_CL1, GET_CT(addr, 1));
}

static CB *l1_check_write_hit(Cache *this, hwaddr_t addr) {
    return normal_check_hit(ASSOC(1)[GET_CI(addr, 1)], ASSOC_CL1, GET_CT(addr, 1));
}

static void l1_read_replace(Cache *this, hwaddr_t addr) {
    CB *dst_cb = normal_find_replace(ASSOC(1)[GET_CI(addr, 1)], ASSOC_CL1);
    CB *src_cb = l2.check_read_hit(&l2, addr);
    dst_cb->tag = GET_CT(addr, 1);
    dst_cb->valid = 1;
    dst_cb->write(dst_cb, 0, src_cb->buf, CB_SIZE);
}

static void l1_write_replace(Cache *this, hwaddr_t addr) {
    return;                                                     //not write allocate
}

static CB *l2_check_read_hit(Cache *this, hwaddr_t addr) {
    return normal_check_hit(ASSOC(2)[GET_CI(addr, 2)], ASSOC_CL2, GET_CT(addr, 2));
}

static CB *l2_check_write_hit(Cache *this, hwaddr_t addr) {
    CB *ret = normal_check_hit(ASSOC(2)[GET_CI(addr, 2)], ASSOC_CL2, GET_CT(addr, 2));
    if (ret != NULL) ret->dirty = 1;
    return ret;
}

static void l2_read_replace(Cache *this, hwaddr_t addr) {
    CB *dst_cb = NULL;
    CB *p_cb = ASSOC(2)[GET_CI(addr, 2)];
    
    int idx;
    for (idx = 0; idx < ASSOC_CL2; ++idx) {
        if (p_cb[idx].dirty) {
            dst_cb = p_cb + idx;
            break;
        }
    }
    if (dst_cb == NULL) dst_cb = normal_find_replace(ASSOC(2)[GET_CI(addr, 2)], ASSOC_CL2);

    if (dst_cb->dirty) {
        //write back
        memcpy(hwa_to_va((((addr & (~CT_MASK(2))) ^ (dst_cb->tag << (32 - TAG_WIDTH(2)))) & (~CO_MASK))), dst_cb->buf, CB_SIZE);
    }

    dst_cb->valid = 1;
    dst_cb->write(dst_cb, 0, hwa_to_va((addr - GET_CO(addr))), CB_SIZE);
    dst_cb->dirty = 0;
    dst_cb->tag = GET_CT(addr, 2);
}

static void l2_write_replace(Cache *this, hwaddr_t addr) {
    CB *dst_cb = normal_find_replace(ASSOC(2)[GET_CI(addr, 2)], ASSOC_CL2);
    dst_cb->valid = 1;
    dst_cb->write(dst_cb, 0, hwa_to_va((addr - GET_CO(addr))), CB_SIZE);    //write allocate
    dst_cb->tag = GET_CT(addr, 2);
}

//main
static void init_cache_internal() {
    l1.cb_pool = (void *)l1_block;
    l2.cb_pool = (void *)l2_block;
    l1.read = l2.read = cread;
    l1.write = l2.write = cwrite;
    l1.check_read_hit = l1_check_read_hit;
    l2.check_read_hit = l2_check_read_hit;
    l1.check_write_hit = l1_check_write_hit;
    l2.check_write_hit = l2_check_write_hit;
    l1.read_replace = l1_read_replace;
    l2.read_replace = l2_read_replace;
    l1.write_replace = l1_write_replace;
    l2.write_replace = l2_write_replace;

    int l1_idx;
    for (l1_idx = 0; l1_idx < NR_CL1_BLOCK; ++l1_idx) {
        l1_block[l1_idx].read = cbread;
        l1_block[l1_idx].write = cbwrite;
        l1_block[l1_idx].valid = 0;
    }
    int l2_idx;
    for (l2_idx = 0; l2_idx < NR_CL2_BLOCK; ++l2_idx) {
        l2_block[l2_idx].read = cbread;
        l2_block[l2_idx].write = cbwrite;
        l2_block[l2_idx].valid = 0;
        l2_block[l2_idx].dirty = 0;
    }
}

//main
void init_cache() {
    init_cache_internal();
}

//main
uint32_t cache_read(hwaddr_t addr, size_t len, bool *hit) {
    if (addr >= 0x1002000 && addr <= 0x1002000 + 120000) Log("[%08x]%d",addr,(unsigned)len);
    uint32_t val = 0;
    
    int of = GET_CO(addr) + len - CB_SIZE;
    if (of > 0) {
        bool hit_l, hit_r;
        val += cache_read(addr, len - of, &hit_l);
        val += cache_read(addr + len - GET_CO(addr + len), of, &hit_r) << ((len - of) << 3);
        *hit = hit_l && hit_r;
        return val;
    }
    
    bool hit_l1, hit_l2;
    val = l1.read(&l1, addr, len, &hit_l1);
    if (hit_l1 == 0) {
        val = l2.read(&l2, addr, len, &hit_l2);
    } else {
        *hit = true;
        return val;
    }
    if (hit_l2 != 0) {
        l1.read_replace(&l1, addr);
        *hit = true;
        return val;
    } else {
        *hit = false;
        return 0;
    }
}

//main
void cache_write(hwaddr_t addr, uint32_t data, size_t len) {
    if (addr >= 0x1002000 && addr <= 0x1002000 + 120000) Log("(%08x,%08x,%d)",addr,data,(unsigned)len);
    int of = GET_CO(addr) + len - CB_SIZE;
    if (of > 0) {
        cache_write(addr, data, len - of);
        cache_write(addr + len - GET_CO(addr + len), data >> ((len - of) << 3), of);
        return;
    }

    bool hit_l1, hit_l2;
    l1.write(&l1, addr, data, len, &hit_l1);   //write through
    l2.write(&l2, addr, data, len, &hit_l2);
    if (hit_l2 == false) {
        l2.write_replace(&l2, addr);
        l2.write(&l2, addr, data, len, &hit_l2);   //write allocate (move to L2 and write again)
    }
}

//main
void cache_replace(hwaddr_t addr, size_t len) {
    int of = GET_CO(addr) + len - CB_SIZE;
    if (of > 0) {
        cache_replace(addr, 0);
        cache_replace(addr + len, 0);
        return;
    }

    l2.read_replace(&l2, addr);
}