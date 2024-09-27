#ifndef __REG_H__
#define __REG_H__

#include "common.h"
#include "cpu.h"
#include "mmu.h"

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

typedef union {
	struct {
		uint8_t rpl : 2;
		uint8_t ti  : 1;
		uint16_t index : 13;
	};

	uint16_t val;
} SegSel;

typedef struct {
	SegSel sel;
	SegDesc hid_desc;
} sreg;

typedef struct {

	CR0 cr0;
	CR3 cr3;
	
	struct {
		uint16_t limit;
		uint32_t LBA;
	} gdtr, idtr;

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

#define sr_base(index) (uint32_t)cpu.sr[index].hid_desc.base_15_0 + ((uint32_t)cpu.sr[index].hid_desc.base_23_16 << 16) + ((uint32_t)cpu.sr[index].hid_desc.base_31_24 << 24)
#define sr_lim(index) (uint32_t)cpu.sr[index].hid_desc.limit_15_0 + ((uint32_t)cpu.sr[index].hid_desc.limit_19_16 << 16)

#define set_sr_base(_sr, _val) {\
	union {\
		struct {\
			uint16_t seg_base;\
			uint8_t base_lo;\
			uint8_t base_hi;\
		};\
		uint32_t val;\
	} _srb;\
	_srb.val = (uint32_t)_val;\
	cpu.sr[_sr].hid_desc.base_15_0 = _srb.seg_base;\
	cpu.sr[_sr].hid_desc.base_23_16 = _srb.base_lo;\
	cpu.sr[_sr].hid_desc.base_31_24 = _srb.base_hi;\
}

#define set_sr_lim(_sr, _val) {\
	union {\
		struct {\
			uint16_t seg_limit;\
			uint8_t limit : 4;\
		};\
		uint32_t val;\
	} _slm;\
	_slm.val = (uint32_t)_val;\
	cpu.sr[_sr].hid_desc.limit_15_0 = _slm.seg_limit;\
	cpu.sr[_sr].hid_desc.limit_19_16 = _slm.limit;\
}

extern const char* regsl[];
extern const char* regsw[];
extern const char* regsb[];
extern const char* regsr[];

#endif
