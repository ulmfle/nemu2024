#include "cpu/exec/helper.h"

#define DATA_BYTE 1
#include "jcc/ja-template.h"
#include "jcc/jae-template.h"
#include "jcc/jb-template.h"
#include "jcc/jbe-template.h"
#include "jcc/jc-template.h"
#include "jcc/jcxz-template.h"
#include "jcc/jecxz-template.h"
#include "jcc/je-template.h"
#include "jcc/jg-template.h"
#include "jcc/jge-template.h"
#include "jcc/jl-template.h"
#include "jcc/jle-template.h"
#include "jcc/jna-template.h"
#include "jcc/jnae-template.h"
#include "jcc/jnb-template.h"
#include "jcc/jnbe-template.h"
#include "jcc/jnc-template.h"
#include "jcc/jne-template.h"
#include "jcc/jng-template.h"
#include "jcc/jnge-template.h"
#include "jcc/jnl-template.h"
#include "jcc/jnle-template.h"
#include "jcc/jno-template.h"
#include "jcc/jnp-template.h"
#include "jcc/jns-template.h"
#include "jcc/jnz-template.h"
#include "jcc/jo-template.h"
#include "jcc/jp-template.h"
#include "jcc/jpe-template.h"
#include "jcc/jpo-template.h"
#include "jcc/js-template.h"
#include "jcc/jz-template.h"
#undef DATA_BYTE

#define DATA_BYTE 2
#include "jcc/ja-template.h"
#include "jcc/jae-template.h"
#include "jcc/jb-template.h"
#include "jcc/jbe-template.h"
#include "jcc/jc-template.h"
#include "jcc/jcxz-template.h"
#include "jcc/jecxz-template.h"
#include "jcc/je-template.h"
#include "jcc/jg-template.h"
#include "jcc/jge-template.h"
#include "jcc/jl-template.h"
#include "jcc/jle-template.h"
#include "jcc/jna-template.h"
#include "jcc/jnae-template.h"
#include "jcc/jnb-template.h"
#include "jcc/jnbe-template.h"
#include "jcc/jnc-template.h"
#include "jcc/jne-template.h"
#include "jcc/jng-template.h"
#include "jcc/jnge-template.h"
#include "jcc/jnl-template.h"
#include "jcc/jnle-template.h"
#include "jcc/jno-template.h"
#include "jcc/jnp-template.h"
#include "jcc/jns-template.h"
#include "jcc/jnz-template.h"
#include "jcc/jo-template.h"
#include "jcc/jp-template.h"
#include "jcc/jpe-template.h"
#include "jcc/jpo-template.h"
#include "jcc/js-template.h"
#include "jcc/jz-template.h"
#undef DATA_BYTE

#define DATA_BYTE 4
#include "jcc/ja-template.h"
#include "jcc/jae-template.h"
#include "jcc/jb-template.h"
#include "jcc/jbe-template.h"
#include "jcc/jc-template.h"
#include "jcc/jcxz-template.h"
#include "jcc/jecxz-template.h"
#include "jcc/je-template.h"
#include "jcc/jg-template.h"
#include "jcc/jge-template.h"
#include "jcc/jl-template.h"
#include "jcc/jle-template.h"
#include "jcc/jna-template.h"
#include "jcc/jnae-template.h"
#include "jcc/jnb-template.h"
#include "jcc/jnbe-template.h"
#include "jcc/jnc-template.h"
#include "jcc/jne-template.h"
#include "jcc/jng-template.h"
#include "jcc/jnge-template.h"
#include "jcc/jnl-template.h"
#include "jcc/jnle-template.h"
#include "jcc/jno-template.h"
#include "jcc/jnp-template.h"
#include "jcc/jns-template.h"
#include "jcc/jnz-template.h"
#include "jcc/jo-template.h"
#include "jcc/jp-template.h"
#include "jcc/jpe-template.h"
#include "jcc/jpo-template.h"
#include "jcc/js-template.h"
#include "jcc/jz-template.h"
#undef DATA_BYTE

make_helper_v(ja_i);
make_helper_v(jae_i);
make_helper_v(jb_i);
make_helper_v(jbe_i);
make_helper_v(jc_i);
make_helper_v(jcxz_i);
make_helper_v(jecxz_i);
make_helper_v(je_i);
make_helper_v(jg_i);
make_helper_v(jge_i);
make_helper_v(jl_i);
make_helper_v(jle_i);
make_helper_v(jna_i);
make_helper_v(jnae_i);
make_helper_v(jnb_i);
make_helper_v(jnbe_i);
make_helper_v(jnc_i);
make_helper_v(jne_i);
make_helper_v(jng_i);
make_helper_v(jnge_i);
make_helper_v(jnl_i);
make_helper_v(jnle_i);
make_helper_v(jno_i);
make_helper_v(jnp_i);
make_helper_v(jns_i);
make_helper_v(jnz_i);
make_helper_v(jo_i);
make_helper_v(jp_i);
make_helper_v(jpe_i);
make_helper_v(jpo_i);
make_helper_v(js_i);
make_helper_v(jz_i);