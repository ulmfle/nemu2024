#include "cpu/exec/helper.h"

#define DATA_BYTE 2
#include "enter-template.h"
#undef DATA_BYTE

#define DATA_BYTE 4
#include "enter-template.h"
#undef DATA_BYTE

make_helper_v(enter);