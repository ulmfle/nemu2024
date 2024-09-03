#include "cpu/exec/helper.h"

// #define DATA_BYTE 2
// #include "call-template.h"
// #undef DATA_BYTE

#define DATA_BYTE 4
#include "call-template.h"
#undef DATA_BYTE

make_helper(call_rel_v) {
    return call_rel_l(eip);
}