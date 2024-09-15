#ifndef __CACHE_H__
#define __CACHE_H__

#include "common.h"
#include "memory/memory.h"
#include <stdlib.h>
#include <time.h>       //for random

#define CB_SIZE_WIDTH 6

#define NR_CL1_BLOCK_WIDTH 10
#define ASSOC_CL1_WIDTH 3
#define TAG_CL1_WIDTH (32 - CB_SIZE_WIDTH - NR_CL1_BLOCK_WIDTH + ASSOC_CL1_WIDTH)

#define NR_CL2_BLOCK_WIDTH 12
#define ASSOC_CL2_WIDTH 16
#define TAG_CL2_WIDTH (32 - CB_SIZE_WIDTH - NR_CL2_BLOCK_WIDTH + ASSOC_CL2_WIDTH)

#define CB_SIZE (1 << CB_SIZE_WIDTH)
#define NR_CL1_BLOCK (1 << NR_CL1_BLOCK_WIDTH)
#define ASSOC_CL1 (1 << ASSOC_CL1_WIDTH)

#define NR_CL2_BLOCK (1 << NR_CL2_BLOCK_WIDTH)
#define ASSOC_CL2 (1 << ASSOC_CL2_WIDTH)

#define CT_L1_MASK (~0u << (32 - TAG_CL1_WIDTH))
#define CO_L1_MASK (CB_SIZE - 1)
#define CI_L1_MASK (((~0u) ^ (CT_L1_MASK)) ^ (CO_L1_MASK))

#define CT_L2_MASK (~0u << (32 - TAG_CL2_WIDTH))
#define CO_L2_MASK (CB_SIZE - 1)
#define CI_L2_MASK (((~0u) ^ (CT_L2_MASK)) ^ (CO_L2_MASK))

#define GET_CT_L1(addr) (((addr) & CT_L1_MASK) >> (32 - TAG_CL1_WIDTH))
#define GET_CI_L1(addr) (((addr) & CI_L1_MASK) >> CB_SIZE_WIDTH)
#define GET_CO_L1(addr) ((addr) & CO_L1_MASK)

#define GET_CT_L2(addr) (((addr) & CT_L2_MASK) >> (32 - TAG_CL2_WIDTH))
#define GET_CI_L2(addr) (((addr) & CI_L2_MASK) >> CB_SIZE_WIDTH)
#define GET_CO_L2(addr) ((addr) & CO_L2_MASK)

#define CB_BASE \
struct {\
    uint8_t buf[CB_SIZE];\
    uint32_t tag;\
    bool valid;\
    uint32_t (*read)(struct CB *, uint8_t, size_t);\
    void (*write)(struct CB *, uint8_t, uint8_t *, size_t);\
}

#define CACHE_BASE \
struct {\
    void *cb_pool;\
    void (*init)(struct Cache *);\
    CB *(*check_read_hit)(struct Cache *, hwaddr_t);\
    CB *(*check_write_hit)(struct Cache *, hwaddr_t);\
    void (*read_replace)(struct Cache *, hwaddr_t);\
    void (*write_replace)(struct Cache *, hwaddr_t);\
    uint32_t (*read)(struct Cache *, hwaddr_t, size_t, bool *);\
    void (*write)(struct Cache *, hwaddr_t, uint32_t, size_t, bool *);\
}

#define CB_INIT(level) \
    int idx;\
    concat(CB_L, level) *pool = this->cb_pool;\
    for (idx = 0; idx < NR_CL1_BLOCK; ++idx) {\
        pool[idx].valid = 0;\
        pool[idx].read = cb_read;\
        pool[idx].write = cb_write;\
    }

#define CACHE_INIT(level)\
    this->cb_pool = (void *)concat3(l, level, _block);\
    this->check_read_hit = concat3(l, level, _check_read_hit);\
    this->check_write_hit = concat3(l, level, _check_write_hit);\
    this->read_replace = concat3(l, level, _read_replace);\
    this->write_replace = concat3(l, level, _write_replace);\
    this->read = concat3(l, level, _read);\
    this->write = concat3(l, level, _write)

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

extern Cache_L1 cache_l1;
extern Cache_L2 cache_l2;

#endif