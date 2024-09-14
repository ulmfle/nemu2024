#include "common.h"
#include "memory/memory.h"
#include <stdlib.h>

#define CB_SIZE_WIDTH 6
#define NR_CL1_BLOCK_WIDTH 10
#define ASSOC_CL1_WIDTH 3
#define TAG_CL1_WIDTH (27 - CB_SIZE_WIDTH - NR_CL1_BLOCK_WIDTH + ASSOC_CL1_WIDTH)

#define CB_SIZE (1 << CB_SIZE_WIDTH)
#define NR_CL1_BLOCK (1 << NR_CL1_BLOCK_WIDTH)
#define ASSOC_CL1 (1 << ASSOC_CL1_WIDTH)

#define CT_L1_MASK ()
#define CI_L1_MASK ()
#define CO_L1_MASK ()

#define GET_CT_L1(addr) ((addr) & CT_L1_MASK)
#define GET_CI_L1(addr) ((addr) & CI_L1_MASK)
#define GET_CO_L1(addr) ((addr) & CO_L1_MASK)

#define CB_BASE \
struct {\
    uint8_t buf[CB_SIZE];\
    uint32_t (*read)(void *, uint8_t, size_t);\
    void (*write)(void *, uint8_t, uint32_t, size_t);\
}

#define CACHE_BASE \
struct {\
    void *cb_pool;\
    void (*init)();\
    uint32_t (*read)(void *, hwaddr_t, size_t, bool *);\
    void (*write)(void *, hwaddr_t ,uint32_t, size_t, bool *);\
}

typedef struct Cache_L1 Cache_L1;

// l1 cache block
typedef struct CB_L1 {
    CB_BASE;
    uint32_t tag : TAG_CL1_WIDTH;
    bool valid;
} CB_L1;

static struct Cache_L1 {
    CACHE_BASE;
    uint8_t (*assoc)[ASSOC_CL1];    //8-way set associative
} cache_l1;

static CB_L1 cl1_block[NR_CL1_BLOCK];

static uint32_t cb_l1_read(void *this, uint8_t off, size_t len) {
    Assert(off + len < CB_SIZE, "CB read error\n");
    return *(uint32_t *)((((CB_L1 *)this)->buf) + off);
}

static void cb_l1_write(void *this, uint8_t off, uint32_t data, size_t len) {
    Assert(off + len < CB_SIZE, "CB write error\n");
    uint8_t *p_data = (uint8_t *)&data;
    uint8_t *dst = (uint8_t *)(((CB_L1 *)this)->buf);
    int idx;
    for (idx = off; idx < off + len; ++idx) {
        dst[idx] = *p_data++;
    }
}

static void l1_init() {
    int idx;
    for (idx = 0; idx < NR_CL1_BLOCK; ++idx) {
        cl1_block[idx].valid = 0;
        cl1_block[idx].read = cb_l1_read;
        cl1_block[idx].write = cb_l1_write;
    }
}

uint32_t l1_read(void *this, hwaddr_t addr, size_t len, bool *hit) {
    return 0;
}

void l1_write(void *this, hwaddr_t addr, uint32_t data, size_t len, bool *hit) {

}


static void cache_l1_init_internal() {
    cache_l1.cb_pool = (void *)cl1_block;
    cache_l1.init = l1_init;
    cache_l1.read = l1_read;
    cache_l1.write = l1_write;
    cache_l1.assoc = (uint8_t (*)[8])cache_l1.cb_pool;
}

void cache_init() {
    cache_l1_init_internal();
    cache_l1.init(&cache_l1);
}