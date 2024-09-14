#ifndef __CACHE_H__
#define __CACHE_H__

#include "common.h"
#include "memory/memory.h"
#include <stdlib.h>
#include <time.h>       //for random

#define CB_SIZE_WIDTH 6
#define NR_CL1_BLOCK_WIDTH 10
#define ASSOC_CL1_WIDTH 3
#define TAG_CL1_WIDTH (27 - CB_SIZE_WIDTH - NR_CL1_BLOCK_WIDTH + ASSOC_CL1_WIDTH)

#define CB_SIZE (1 << CB_SIZE_WIDTH)
#define NR_CL1_BLOCK (1 << NR_CL1_BLOCK_WIDTH)
#define ASSOC_CL1 (1 << ASSOC_CL1_WIDTH)

#define CT_L1_MASK (~0u << (27 - TAG_CL1_WIDTH))
#define CO_L1_MASK (CB_SIZE - 1)
#define CI_L1_MASK (((~0u) ^ (CT_L1_MASK)) ^ (CO_L1_MASK))

#define GET_CT_L1(addr) ((addr) & CT_L1_MASK)
#define GET_CI_L1(addr) ((addr) & CI_L1_MASK)
#define GET_CO_L1(addr) ((addr) & CO_L1_MASK)

// #define GET_CT(addr, ln) concat(GET_CT_L, ln)(addr)
// #define GET_CO(addr, ln) concat(GET_CO_L, ln)(addr)
// #define GET_CI(addr, ln) concat(GET_CI_L, ln)(addr)

#define CB_BASE \
struct {\
    uint8_t buf[CB_SIZE];\
    uint32_t (*read)(void *, uint8_t, size_t);\
    void (*write)(void *, uint8_t, uint8_t *, size_t);\
}

#define CACHE_BASE \
struct {\
    void *cb_pool;\
    void (*init)(void *);\
    void *(*check_hit)(void *, hwaddr_t);\
    void *(*replace)(void *, hwaddr_t, uint8_t *);\
    uint32_t (*read)(void *, hwaddr_t, size_t, bool *);\
    void (*write)(void *, hwaddr_t, uint32_t, size_t, bool *);\
}

typedef struct CB_L1 {
    CB_BASE;
    uint32_t tag : TAG_CL1_WIDTH;
    bool valid;
} CB_L1;

typedef struct Cache_L1 {
    CACHE_BASE;
    CB_L1 (*assoc)[ASSOC_CL1];    //8-way set associative
} Cache_L1;

extern Cache_L1 cache_l1;
extern uint8_t *hw_mem;

#endif