#include "common.h"
#include "memory/memory.h"
#include <stdlib.h>
#include <time.h>       //for random

#define level 2

#define CB_SIZE_WIDTH 6

#define NR_CL1_BLOCK_WIDTH 10
#define ASSOC_CL1_WIDTH 3
#define TAG_CL1_WIDTH (32 - CB_SIZE_WIDTH - NR_CL1_BLOCK_WIDTH + ASSOC_CL1_WIDTH)

#define NR_CL2_BLOCK_WIDTH 12
#define ASSOC_CL2_WIDTH 4
#define TAG_CL2_WIDTH (32 - CB_SIZE_WIDTH - NR_CL2_BLOCK_WIDTH + ASSOC_CL2_WIDTH)

#define CB_SIZE (1 << CB_SIZE_WIDTH)
#define NR_CL1_BLOCK (1 << NR_CL1_BLOCK_WIDTH)
#define ASSOC_CL1 (1 << ASSOC_CL1_WIDTH)

#define NR_CL2_BLOCK (1 << NR_CL2_BLOCK_WIDTH)
#define ASSOC_CL2 (1 << ASSOC_CL2_WIDTH)

#define CT_L1_MASK (~0u << (32 - TAG_CL1_WIDTH))
#define CO_L1_MASK (CB_SIZE - 1)
#define CI_L1_MASK (((~0u) ^ (CT_L1_MASK)) ^ (concat3(CO_L, level, _MASK)))

#define CT_L2_MASK (~0u << (32 - TAG_CL2_WIDTH))
#define CO_L2_MASK (CB_SIZE - 1)
#define CI_L2_MASK (((~0u) ^ (CT_L2_MASK)) ^ (CO_L2_MASK))

#define GET_CT_L1(addr) (((addr) & CT_L1_MASK) >> (32 - TAG_CL1_WIDTH))
#define GET_CI_L1(addr) (((addr) & CI_L1_MASK) >> CB_SIZE_WIDTH)
#define GET_CO_L1(addr) ((addr) & concat3(CO_L, level, _MASK))

#define GET_CT_L2(addr) (((addr) & CT_L2_MASK) >> (32 - TAG_CL2_WIDTH))
#define GET_CI_L2(addr) (((addr) & CI_L2_MASK) >> CB_SIZE_WIDTH)
#define GET_CO_L2(addr) ((addr) & CO_L2_MASK)

#define GET_CT(addr, level) concat(GET_CT_L, level)(addr)
#define GET_CI(addr, level) concat(GET_CI_L, level)(addr)
#define GET_CO(addr, level) concat(GET_CO_L, level)(addr)

#define CB_BASE \
struct {\
    uint8_t buf[CB_SIZE];\
    uint32_t tag;\
    bool valid;\
    /*virtual*/ uint32_t (*read)(struct CB *, uint8_t, size_t);\
    /*virtual*/ void (*write)(struct CB *, uint8_t, uint8_t *, size_t);\
}

#define CACHE_BASE \
struct {\
    void *cb_pool;\
    /*virtual...=0*/ CB *(*check_read_hit)(struct Cache *, hwaddr_t);\
    /*virtual...=0*/ CB *(*check_write_hit)(struct Cache *, hwaddr_t);\
    /*virtual...=0*/ void (*read_replace)(struct Cache *, hwaddr_t);\
    /*virtual...=0*/ void (*write_replace)(struct Cache *, hwaddr_t);\
    /*virtual*/ uint32_t (*read)(struct Cache *, hwaddr_t, size_t, bool *);\
    /*virtual*/ void (*write)(struct Cache *, hwaddr_t, uint32_t, size_t, bool *);\
}

#define NORMAL_CHECK_HIT(level) \
    do {\
        int idx;\
        concat(CB_L, level) *p_cb = (concat(CB_L, level) *)(((concat(Cache_L, level) *)this)->assoc[concat(GET_CI_L, level)(addr)]);\
        for (idx = 0; idx < concat(ASSOC_CL, level); ++idx) {\
            Log("\nvalid: %d tag: 0x%08x hit_tag: 0x%08x", p_cb[idx].valid, p_cb[idx].tag, concat(GET_CT_L, level)(addr));\
            if (p_cb[idx].valid && p_cb[idx].tag == concat(GET_CT_L, level)(addr)) return (CB *)(p_cb + idx);\
        }\
        return NULL;\
    } while (0)

#define FIND_REPLACE(level) \
    concat(CB_L, level) *p_cb = ((concat(Cache_L, level) *)this)->assoc[GET_CI(addr, level)];\
    int idx;\
    for (idx = 0; idx < concat(ASSOC_CL, level); ++idx) {\
        if (p_cb[idx].valid) continue;\
        dst_cb = p_cb + idx;\
        break;\
    }\
    if (dst_cb == NULL) {\
        srand((unsigned)time(NULL));\
        dst_cb = p_cb + (rand() % concat(ASSOC_CL, level));\
    }


typedef struct CB {
    CB_BASE;
} CB;

typedef struct {
    CB_BASE;
} CB_L1;

typedef struct {
    CB_BASE;
    bool dirty;
} CB_L2;

typedef struct Cache {
    CACHE_BASE;
} Cache;

typedef struct {
    CACHE_BASE;
    CB_L1 (*assoc)[ASSOC_CL1];
} Cache_L1;

typedef struct {
    CACHE_BASE;
    CB_L2 (*assoc)[ASSOC_CL2];
} Cache_L2;

static int of;
static CB_L1 l1_block[NR_CL1_BLOCK];
static CB_L2 l2_block[NR_CL2_BLOCK];

Cache_L1 cache_l1;
Cache_L2 cache_l2;

//base
static uint32_t cbread(CB *this, uint8_t off, size_t len) {
    int idx;
    for (idx = 0; idx < CB_SIZE; ++idx) printf("%02x ", this->buf[idx]);
    printf("\n");
    Log("%d %d", off, (unsigned)len);
    return (*(uint32_t *)(this->buf + off)) & (~0u >> ((4 - len) << 3));
}

//base
static void cbwrite(CB *this, uint8_t off, uint8_t *data, size_t len) {
    Log("%d %d", off, (unsigned)len);
    memcpy(this->buf + off, data, len);
}

//base
static uint32_t cread(Cache *this, hwaddr_t addr, size_t len, bool *hit) {
    uint32_t val;
    CB *cb,*cb_of = NULL;
    of = GET_CO_L1(addr) + len - CB_SIZE;
    of = of > 0 ? of : 0;
    cb = this->check_read_hit(this, addr);
    if (of) cb_of = this->check_read_hit(this, addr + len);
    if (((of == 0) && (cb == NULL)) || ((of > 0) && (cb == NULL || cb_of == NULL))) {
        *hit = false;
        return 0;
    }
    *hit = true;
    val = cb->read(cb, GET_CO_L1(addr), len - of);
    if (of) val += (cb_of->read(cb_of, 0, of) << ((len - of) << 3));
    of = 0;
    return val;
}

//base
static void cwrite(Cache *this, hwaddr_t addr, uint32_t data, size_t len, bool *hit) {
    CB *cb,*cb_of = NULL;
    of = GET_CO_L1(addr) + len - CB_SIZE;
    of = of > 0 ? of : 0;
    cb = this->check_read_hit(this, addr);
    if (of) cb_of = this->check_read_hit(this, addr + len);
    if (((of == 0) && (cb == NULL)) || ((of > 0) && (cb == NULL || cb_of == NULL))) {
        *hit = false;
        return;
    }
    *hit = true;
    uint8_t *of_data = ((uint8_t *)&data) + len - of;
    cb->write(cb, GET_CO_L1(addr), (uint8_t *)&data, len - of);
    if (of) cb_of->write(cb_of, 0, of_data, of);
    of = 0;
}

static CB *l1_check_read_hit(Cache *this, hwaddr_t addr) {
    NORMAL_CHECK_HIT(1);
}

static CB *l1_check_write_hit(Cache *this, hwaddr_t addr) {
    NORMAL_CHECK_HIT(1);
}

static void l1_read_replace(Cache *this, hwaddr_t addr) {
    CB_L1 *dst_cb = NULL;
    FIND_REPLACE(1);
    CB *src_cb = cache_l2.check_read_hit((Cache *)&cache_l2, addr);
    dst_cb->tag = GET_CT(addr, level);
    dst_cb->valid = 1;
    dst_cb->write((CB *)dst_cb, 0, src_cb->buf, CB_SIZE);
    if (of != 0) {
        of = 0;
        l1_read_replace(this, addr + CB_SIZE + 1);
    }
}

static void l1_write_replace(Cache *this, hwaddr_t addr) {
    return;                                                     //not write allocate
}

static CB *l2_check_read_hit(Cache *this, hwaddr_t addr) {
    NORMAL_CHECK_HIT(2);
}

static CB *l2_check_write_hit(Cache *this, hwaddr_t addr) {
    int idx;
    CB_L2 *p_cb = (CB_L2 *)(((Cache_L2 *)this)->assoc[GET_CI(addr, 2)]);
    for (idx = 0; idx < ASSOC_CL2; ++idx) {
        Log("valid: %d dirty:%d tag: 0x%08x hit_tag: 0x%08x", p_cb[idx].valid, p_cb[idx].dirty, p_cb[idx].tag, GET_CT(addr, 2));
        if (p_cb[idx].valid && p_cb[idx].tag == GET_CT(addr, 2)) {
            p_cb[idx].dirty = 1;
            return (CB *)(p_cb + idx);
        }
    }
    return NULL;
}

static void l2_read_replace(Cache *this, hwaddr_t addr) {
    CB_L2 *dst_cb = NULL;

    CB_L2 *p_cb = ((Cache_L2 *)this)->assoc[GET_CI(addr, 2)];
    int idx;

    for (idx = 0; idx < ASSOC_CL2; ++idx) {
        if (p_cb[idx].dirty) {
            dst_cb = p_cb + idx;
            break;
        }
    }

    if (dst_cb == NULL) {
        for (idx = 0; idx < ASSOC_CL2; ++idx) {
            if (!p_cb[idx].valid) {
                dst_cb = p_cb + idx;
                break;
            }
        }
    }

    if (dst_cb == NULL) {
        srand((unsigned)time(NULL));
        dst_cb = p_cb + (rand() % ASSOC_CL2);
    }

    dst_cb->tag = GET_CT(addr, 2);
    dst_cb->valid = 1;
    if (dst_cb->dirty) memcpy(hwa_to_va((((addr & (~CT_L2_MASK)) ^ (dst_cb->tag << (32 - TAG_CL2_WIDTH))) & (~CB_SIZE))), dst_cb->buf, CB_SIZE);
    dst_cb->write((CB *)dst_cb, 0, hwa_to_va((addr & (~CB_SIZE))), CB_SIZE);
    dst_cb->dirty = 0;
    if (of != 0) {
        of = 0;
        l2_read_replace(this, addr + CB_SIZE + 1);
    }
}

static void l2_write_replace(Cache *this, hwaddr_t addr) {
    CB_L2 *dst_cb = NULL;
    FIND_REPLACE(2);
    dst_cb->tag = GET_CT(addr, 2);
    dst_cb->valid = 1;
    dst_cb->write((CB *)dst_cb, 0, hwa_to_va((addr & (~CB_SIZE))), CB_SIZE);    //write allocate
    if (of != 0) {
        of = 0;
        l2_write_replace(this, addr + CB_SIZE + 1);
    }
}

static void init_cache_internal() {
    cache_l1.cb_pool = (void *)l1_block;
    cache_l1.check_read_hit = l1_check_read_hit;
    cache_l1.check_write_hit = l1_check_write_hit;
    cache_l1.read_replace = l1_read_replace;
    cache_l1.write_replace = l1_write_replace;
    cache_l1.assoc = (CB_L1 (*)[8])cache_l1.cb_pool;

    cache_l2.cb_pool = (void *)l2_block;
    cache_l2.check_read_hit = l2_check_read_hit;
    cache_l2.check_write_hit = l2_check_write_hit;
    cache_l2.read_replace = l2_read_replace;
    cache_l2.write_replace = l2_write_replace;
    cache_l2.assoc = (CB_L2 (*)[16])cache_l2.cb_pool;

    cache_l1.read = cache_l2.read = cread;
    cache_l1.write = cache_l2.write = cwrite;

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
    }
}

void init_cache() {
    init_cache_internal();
}

//main
uint32_t cache_read(hwaddr_t addr, size_t len, bool *hit) {
    uint32_t val;
    bool hit_l1, hit_l2;
    val = cache_l1.read((Cache *)&cache_l1, addr, len, &hit_l1);
    Log("l1 %d l2 %d",hit_l1, hit_l2);
    if (hit_l1 == 0) {
        val = cache_l2.read((Cache *)&cache_l2, addr, len, &hit_l2);
        Log("l1 %d l2 %d",hit_l1, hit_l2);
    } else {
        *hit = true;
        return val;
    }
    if (hit_l2 != 0) {
        cache_l1.read_replace((Cache *)&cache_l1, addr);
        *hit = true;
        return val;
    } else {
        *hit = false;
        return 0;
    }

}

void cache_write(hwaddr_t addr, uint32_t data, size_t len) {
    bool hit_l1, hit_l2;
    cache_l1.write((Cache *)&cache_l1, addr, data, len, &hit_l1);   //write through
    cache_l2.write((Cache *)&cache_l2, addr, data, len, &hit_l2);
    if (hit_l2 == false) {
        cache_l2.write_replace((Cache *)&cache_l2, addr);
        cache_l2.write((Cache *)&cache_l2, addr, data, len, &hit_l2);   //write allocate (move to L2 and write again)
    }
}

//main
void cache_replace(hwaddr_t addr) {
    cache_l2.read_replace((Cache *)&cache_l2, addr);
    cache_l1.read_replace((Cache *)&cache_l1, addr);
}
