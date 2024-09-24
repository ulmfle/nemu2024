#ifndef __REG_H__
#define __REG_H__

#include "common.h"
// #include "/lib-common/x86-inc/cpu.h"

enum { R_EAX, R_ECX, R_EDX, R_EBX, R_ESP, R_EBP, R_ESI, R_EDI };
enum { R_AX, R_CX, R_DX, R_BX, R_SP, R_BP, R_SI, R_DI };
enum { R_AL, R_CL, R_DL, R_BL, R_AH, R_CH, R_DH, R_BH };
enum { SR_ES, SR_CS, SR_SS, SR_DS, SR_FS, SR_GS };
/* TODO: Re-organize the `CPU_state' structure to match the register
 * encoding scheme in i386 instruction format. For example, if we
 * access cpu.gpr[3]._16, we will get the `bx' register; if we access
 * cpu.gpr[1]._8[1], we will get the 'ch' register. Hint: Use `union'.
 * For more details about the register encoding scheme, see i386 manual.
 */

/* the Control Register 0 */
typedef union CR0 {
	struct {
		uint32_t protect_enable      : 1;
		uint32_t monitor_coprocessor : 1;
		uint32_t emulation           : 1;
		uint32_t task_switched       : 1;
		uint32_t extension_type      : 1;
		uint32_t numeric_error       : 1;
		uint32_t pad0                : 10;
		uint32_t write_protect       : 1; 
		uint32_t pad1                : 1; 
		uint32_t alignment_mask      : 1;
		uint32_t pad2                : 10;
		uint32_t no_write_through    : 1;
		uint32_t cache_disable       : 1;
		uint32_t paging              : 1;
	};
	uint32_t val;
} CR0;

/* the Control Register 3 (physical address of page directory) */
typedef union CR3 {
	struct {
		uint32_t pad0                : 3;
		uint32_t page_write_through  : 1;
		uint32_t page_cache_disable  : 1;
		uint32_t pad1                : 7;
		uint32_t page_directory_base : 20;
	};
	uint32_t val;
} CR3;

typedef union {
	struct {
		union {
			struct {
				uint16_t seg_limit;
				uint16_t seg_base;
			};

			uint32_t lo_32;
		};

		union {
			struct {
				uint8_t base_lo;
				uint8_t type : 4;
				uint8_t s : 1;
				uint8_t dpl : 2;
				uint8_t seg_present : 1;
				uint8_t limit : 4;
				uint8_t avl : 1;
				uint8_t o : 1;
				uint8_t x : 1;
				uint8_t g : 1;
				uint8_t base_hi;
			};

			uint32_t hi_32;
		};
	};

	uint64_t val;
} descriptor;

typedef union {
	struct {
		uint16_t seg_base;
		uint8_t base_lo;
		uint8_t base_hi;
	};
	uint32_t val;
} srbase;

typedef union {
	struct {
		uint16_t seg_limit;
		uint8_t limit : 4;
	};
	uint32_t val;
} srlim;

typedef union {
	struct {
		uint8_t rpl : 2;
		uint8_t ti  : 1;
		uint16_t index : 13;
	};

	uint16_t val;
} selector;

typedef struct {
	selector sel;
	descriptor hid_desc;
} sreg;

typedef struct {
	uint8_t p     : 1;
	uint8_t rw    : 1;
	uint8_t us    : 1;
	uint8_t       : 2;
	uint8_t a     : 1;
	uint8_t d     : 1;
	uint8_t       : 2;
	uint8_t avail : 3;
	uint32_t page_frame_addr : 20;
} page_entry;

typedef struct {

	CR0 cr0;
	CR3 cr3;
	
	struct {
		uint16_t limit;
		uint32_t LBA;
	} gdtr;

    union {
        union {
		    uint32_t _32;
		    uint16_t _16;
		    uint8_t _8[2];
        } gpr[8];

     /* Do NOT change the order of the GPRs' definitions. */

        struct {
            uint32_t eax, ecx, edx, ebx, esp, ebp, esi, edi;
        };
    };

     swaddr_t eip;
     
     union {
		struct {
			uint32_t CF		:1;
			uint32_t pad0	:1;
			uint32_t PF		:1;
			uint32_t pad1	:1;
			uint32_t AF		:1;
			uint32_t pad2	:1;
			uint32_t ZF		:1;
			uint32_t SF		:1;
			uint32_t TF		:1;
			uint32_t IF		:1;
			uint32_t DF		:1;
			uint32_t OF		:1;
			uint32_t IOPL	:2;
			uint32_t NT		:1;
			uint32_t pad3	:1;
			uint16_t pad4;
		};
		uint32_t val;
	} eflags;

	union {
		sreg sr[4];
		sreg es, cs, ss, ds/*, fs, gs*/;
	};

} CPU_state;

extern CPU_state cpu;

static inline int check_reg_index(int index) {
	assert(index >= 0 && index < 8);
	return index;
}

#define reg_l(index) (cpu.gpr[check_reg_index(index)]._32)
#define reg_w(index) (cpu.gpr[check_reg_index(index)]._16)
#define reg_b(index) (cpu.gpr[check_reg_index(index) & 0x3]._8[index >> 2])

#define sr_base(index) (uint32_t)cpu.sr[index].hid_desc.seg_base + ((uint32_t)cpu.sr[index].hid_desc.base_lo << 16) + ((uint32_t)cpu.sr[index].hid_desc.base_hi << 24)
#define sr_lim(index) (uint32_t)cpu.sr[index].hid_desc.seg_limit + ((uint32_t)cpu.sr[index].hid_desc.limit << 16)
#define set_sr_base(_sr, _val) {srbase _srb; _srb.val = (uint32_t)_val;cpu.sr[_sr].hid_desc.seg_base = _srb.seg_base; cpu.sr[_sr].hid_desc.base_lo = _srb.base_lo; cpu.sr[_sr].hid_desc.base_hi = _srb.base_hi; } 
#define set_sr_lim(_sr, _val) {srlim _slm; _slm.val = (uint32_t)_val;cpu.sr[_sr].hid_desc.seg_limit = _slm.seg_limit; cpu.sr[_sr].hid_desc.limit = _slm.limit;} 

extern const char* regsl[];
extern const char* regsw[];
extern const char* regsb[];
extern const char* regsr[];

#endif
