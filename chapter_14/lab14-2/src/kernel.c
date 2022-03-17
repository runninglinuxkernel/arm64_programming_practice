#include "uart.h"
#include "esr.h"
#include "irq.h"
#include "asm/base.h"
#include "mm.h"

extern void ldr_test(void);
extern void my_memcpy_test(void);
extern void access_label_test(void);

void my_ldr_str_test(void)
{

	ldr_test();
	my_memcpy_test();

	memset(0x200004, 0x55, 102);

	access_label_test();


}

extern unsigned long compare_and_return(unsigned long a, unsigned long b);

extern void add_inst_test(void);
extern void ands_test(void);
extern void bitfield_test(void);
extern void cmp_cmn_test(void);
extern unsigned long csel_test(unsigned long r, unsigned long b);
extern void bl_test(void);
extern void adrp_test(void);
extern unsigned long my_atomic_write(long data);
extern void atomic_set(unsigned long a, unsigned long b);
void my_data_process_inst(void)
{
	unsigned long ret;
	unsigned long val;
	unsigned long p1 = 0;

	add_inst_test();

	ret = compare_and_return(10, 9);
	val = compare_and_return(9, 10);

	ands_test();
	bitfield_test();
	cmp_cmn_test();
	val = csel_test(1, 3);

	bl_test();

	adrp_test();

	/*
	 * 在树莓派4上跑ldxr和stxr指令会系统宕机（A72核心不支持 通过ldxr指令来访问device memory），
但是在QEMU上能跑
	 */
	//val = my_atomic_write(0x345);

	//atomic_set(0x11, &p1);
}

extern unsigned long func_addr[];
extern unsigned long func_num_syms;
extern char func_string[];

static int print_func_name(unsigned long addr)
{
	int i;
	char *p, *string;

	for (i = 0; i < func_num_syms; i++) {
		if (addr == func_addr[i])
			goto found;
	}

	return 0;

found:
    p = &func_string;

    while (1) {
    	p++;

    	if (*p == '\0')
    		i--;

    	if (i == 0) {
    		p++;
    		string = p;
		printk("<0x%lx> %s\n", addr, string);
    		break;
    	}
    }

    return 0;
}

extern unsigned long macro_test_1(long a, long b);
extern unsigned long macro_test_2(long a, long b);

extern char _text_boot[], _etext_boot[];
extern char _text[], _etext[];
extern char _rodata[], _erodata[];
extern char _data[], _edata[];
extern char _bss[], _ebss[];

static void print_mem(void)
{
	printk("BenOS image layout:\n");
	printk("  .text.boot: 0x%08lx - 0x%08lx (%6ld B)\n",
			(unsigned long)_text_boot, (unsigned long)_etext_boot,
			(unsigned long)(_etext_boot - _text_boot));
	printk("       .text: 0x%08lx - 0x%08lx (%6ld B)\n",
			(unsigned long)_text, (unsigned long)_etext,
			(unsigned long)(_etext - _text));
	printk("     .rodata: 0x%08lx - 0x%08lx (%6ld B)\n",
			(unsigned long)_rodata, (unsigned long)_erodata,
			(unsigned long)(_erodata - _rodata));
	printk("       .data: 0x%08lx - 0x%08lx (%6ld B)\n",
			(unsigned long)_data, (unsigned long)_edata,
			(unsigned long)(_edata - _data));
	printk("        .bss: 0x%08lx - 0x%08lx (%6ld B)\n",
			(unsigned long)_bss, (unsigned long)_ebss,
			(unsigned long)(_ebss - _bss));
}

/*
 * 内嵌汇编 lab1：实现简单的memcpy函数
 *
 * 实现一个小的memcpy汇编函数
 * 从0x80000地址拷贝32字节到0x100000地址处，并使用gdb来比较数据是否拷贝正确
 */
#if 0
static void my_memcpy_asm_test(unsigned long src, unsigned long dst,
		unsigned long counter)
{
	unsigned long tmp;
	unsigned long end = src + counter;

	asm volatile (
			"1: ldr %1, [%2], #8\n"
			"str %1, [%0], #8\n"
			"cmp %2, %3\n"
			"b.cc 1b"
			: "+r" (dst), "+r" (tmp), "+r" (src)
			: "r" (end)
			: "memory");
}
#else
static void my_memcpy_asm_test(unsigned long src, unsigned long dst,
		unsigned long counter)
{
	unsigned long tmp;
	unsigned long end = src + counter;

	asm volatile (
			"1: ldr %[tmp], [%[src]], #8\n"
			"str %[tmp], [%[dst]], #8\n"
			"cmp %[src], %[end]\n"
			"b.cc 1b"
			: [dst]"+r" (dst), [tmp]"+r" (tmp), [src]"+r" (src)
			: [end]"r" (end)
			: "memory");
}
#endif

#define MY_OPS(ops, asm_ops) \
static inline my_asm_##ops(unsigned long mask, void *p) \
{                                                     \
	unsigned long tmp;                            \
	asm volatile (                                \
			"ldr %1, [%0]\n"              \
			" "#asm_ops" %1, %1, %2\n"    \
			"str %1, [%0]\n"               \
			: "+r"(p), "+r" (tmp)          \
			: "r" (mask)                   \
			: "memory"	               \
		     );                                \
}

MY_OPS(or, orr)
MY_OPS(and, and)
MY_OPS(andnot, bic)

static void my_ops_test(void)
{
	unsigned long p;

	p = 0xf;
	my_asm_and(0x2, &p);
	printk("test and: p=0x%x\n", p);

	p = 0;
	my_asm_or(0x3, &p);
	printk("test or: p=0x%x\n", p);

	p = 0x3;
	my_asm_andnot(0x2, &p);
	printk("test andnot: p=0x%x\n", p);
}

/*
 * 在带参数的宏，#号作为一个预处理运算符,
 * 可以把记号转换成字符串
 *
 * 下面这句话会在预编译阶段变成：
 *  asm volatile("mrs %0, " "reg" : "=r" (__val)); __val; });
 */
#define read_sysreg(reg) ({ \
		unsigned long _val; \
		asm volatile("mrs %0," #reg \
		: "=r"(_val)); \
		_val; \
})

#define write_sysreg(val, reg) ({ \
		unsigned long _val = (unsigned long)val; \
		asm volatile("msr " #reg ", %x0" \
		:: "rZ"(_val)); \
})

static void test_sysregs(void)
{
	unsigned long el;

	el = read_sysreg(CurrentEL);
	printk("el = %d\n", el >> 2);

	//write_sysreg(0x10000, vbar_el2);
	//printk("read vbar: 0x%x\n", read_sysreg(vbar_el2));
}

static int test_asm_goto(int a)
{
	asm goto (
			"cmp %w0, 1\n"
			"b.eq %l[label]\n"
			: 
			: "r" (a)
			: "memory"
			: label);

	return 0;

label:
	printk("%s: a = %d\n", __func__, a);
	return 0;
}

static const char * const bad_mode_handler[] = {
	"Sync Abort",
	"IRQ",
	"FIQ",
	"SError"
};

static const char *data_fault_code[] = {
	[0] = "Address size fault, level0",
	[1] = "Address size fault, level1",
	[2] = "Address size fault, level2",
	[3] = "Address size fault, level3",
	[4] = "Translation fault, level0",
	[5] = "Translation fault, level1",
	[6] = "Translation fault, level2",
	[7] = "Translation fault, level3",
	[9] = "Access flag fault, level1",
	[10] = "Access flag fault, level2",
	[11] = "Access flag fault, level3",
	[13] = "Permission fault, level1",
	[14] = "Permission fault, level2",
	[15] = "Permission fault, level3",
	[0x21] = "Alignment fault",
	[0x35] = "Unsupported Exclusive or Atomic access",
};

static const char *esr_get_dfsc_string(unsigned int esr)
{
	return data_fault_code[esr & 0x3f];
}

static const char *esr_class_str[] = {
	[0 ... ESR_ELx_EC_MAX]		= "UNRECOGNIZED EC",
	[ESR_ELx_EC_UNKNOWN]		= "Unknown/Uncategorized",
	[ESR_ELx_EC_WFx]		= "WFI/WFE",
	[ESR_ELx_EC_CP15_32]		= "CP15 MCR/MRC",
	[ESR_ELx_EC_CP15_64]		= "CP15 MCRR/MRRC",
	[ESR_ELx_EC_CP14_MR]		= "CP14 MCR/MRC",
	[ESR_ELx_EC_CP14_LS]		= "CP14 LDC/STC",
	[ESR_ELx_EC_FP_ASIMD]		= "ASIMD",
	[ESR_ELx_EC_CP10_ID]		= "CP10 MRC/VMRS",
	[ESR_ELx_EC_CP14_64]		= "CP14 MCRR/MRRC",
	[ESR_ELx_EC_ILL]		= "PSTATE.IL",
	[ESR_ELx_EC_SVC32]		= "SVC (AArch32)",
	[ESR_ELx_EC_HVC32]		= "HVC (AArch32)",
	[ESR_ELx_EC_SMC32]		= "SMC (AArch32)",
	[ESR_ELx_EC_SVC64]		= "SVC (AArch64)",
	[ESR_ELx_EC_HVC64]		= "HVC (AArch64)",
	[ESR_ELx_EC_SMC64]		= "SMC (AArch64)",
	[ESR_ELx_EC_SYS64]		= "MSR/MRS (AArch64)",
	[ESR_ELx_EC_IMP_DEF]		= "EL3 IMP DEF",
	[ESR_ELx_EC_IABT_LOW]		= "IABT (lower EL)",
	[ESR_ELx_EC_IABT_CUR]		= "IABT (current EL)",
	[ESR_ELx_EC_PC_ALIGN]		= "PC Alignment",
	[ESR_ELx_EC_DABT_LOW]		= "DABT (lower EL)",
	[ESR_ELx_EC_DABT_CUR]		= "DABT (current EL)",
	[ESR_ELx_EC_SP_ALIGN]		= "SP Alignment",
	[ESR_ELx_EC_FP_EXC32]		= "FP (AArch32)",
	[ESR_ELx_EC_FP_EXC64]		= "FP (AArch64)",
	[ESR_ELx_EC_SERROR]		= "SError",
	[ESR_ELx_EC_BREAKPT_LOW]	= "Breakpoint (lower EL)",
	[ESR_ELx_EC_BREAKPT_CUR]	= "Breakpoint (current EL)",
	[ESR_ELx_EC_SOFTSTP_LOW]	= "Software Step (lower EL)",
	[ESR_ELx_EC_SOFTSTP_CUR]	= "Software Step (current EL)",
	[ESR_ELx_EC_WATCHPT_LOW]	= "Watchpoint (lower EL)",
	[ESR_ELx_EC_WATCHPT_CUR]	= "Watchpoint (current EL)",
	[ESR_ELx_EC_BKPT32]		= "BKPT (AArch32)",
	[ESR_ELx_EC_VECTOR32]		= "Vector catch (AArch32)",
	[ESR_ELx_EC_BRK64]		= "BRK (AArch64)",
};

static const char *esr_get_class_string(unsigned int esr)
{
	return esr_class_str[esr >> ESR_ELx_EC_SHIFT];
}

void parse_esr(unsigned int esr)
{
	unsigned int ec = ESR_ELx_EC(esr);

	printk("ESR info:\n");
	printk("  ESR = 0x%08x\n", esr);
	printk("  Exception class = %s, IL = %u bits\n",
		 esr_get_class_string(esr),
		 (esr & ESR_ELx_IL) ? 32 : 16);


	if (ec == ESR_ELx_EC_DABT_LOW || ec == ESR_ELx_EC_DABT_CUR) {
		printk("  Data abort:\n");
		if ((esr & ESR_ELx_ISV)) {
			printk("  Access size = %u byte(s)\n",
			 1U << ((esr & ESR_ELx_SAS) >> ESR_ELx_SAS_SHIFT));
			printk("  SSE = %lu, SRT = %lu\n",
			 (esr & ESR_ELx_SSE) >> ESR_ELx_SSE_SHIFT,
			 (esr & ESR_ELx_SRT_MASK) >> ESR_ELx_SRT_SHIFT);
			printk("  SF = %lu, AR = %lu\n",
			 (esr & ESR_ELx_SF) >> ESR_ELx_SF_SHIFT,
			 (esr & ESR_ELx_AR) >> ESR_ELx_AR_SHIFT);
		}
		
		printk("  SET = %lu, FnV = %lu\n",
			(esr >> ESR_ELx_SET_SHIFT) & 3,
			(esr >> ESR_ELx_FnV_SHIFT) & 1);
		printk("  EA = %lu, S1PTW = %lu\n",
			(esr >> ESR_ELx_EA_SHIFT) & 1,
			(esr >> ESR_ELx_S1PTW_SHIFT) & 1);
		printk("  CM = %lu, WnR = %lu\n",
		 (esr & ESR_ELx_CM) >> ESR_ELx_CM_SHIFT,
		 (esr & ESR_ELx_WNR) >> ESR_ELx_WNR_SHIFT);
		printk("  DFSC = %s\n", esr_get_dfsc_string(esr));
	}
}

void panic(void)
{
	printk("Kernel panic\n");

	while (1)
		;
}

void bad_mode(struct pt_regs *regs, int reason, unsigned int esr)
{
	printk("Bad mode for %s handler detected, far:0x%x esr:0x%x - %s\n",
			bad_mode_handler[reason], read_sysreg(far_el1),
			esr, esr_get_class_string(esr));

	parse_esr(esr);

	panic();
}

static int test_access_map_address(void)
{
	unsigned long address = TOTAL_MEMORY - 4096;

	*(unsigned long *)address = 0x55;

	printk("%s access 0x%x done\n", __func__, address);

	return 0;
}

/*
 * 访问一个没有建立映射的地址
 * 应该会触发一级页表访问错误。
 *
 * Translation fault, level 1
 *
 * 见armv8.6手册第2995页
 */
static int test_access_unmap_address(void)
{
	unsigned long address = TOTAL_MEMORY + 4096;

	*(unsigned long *)address = 0x55;

	printk("%s access 0x%x done\n", __func__, address);

	return 0;
}


static void test_mmu(void)
{
	test_access_map_address();
	test_access_unmap_address();
}

extern void trigger_alignment(void);

void kernel_main(void)
{
	unsigned long val = 0;

	uart_init();
	init_printk_done();
	uart_send_string("Welcome BenOS!\r\n");
	printk("printk init done\n");

	/* my test*/
	my_ldr_str_test();
	my_data_process_inst();

	/*汇编器lab1：查表*/
	print_func_name(0x800880);
	val = macro_test_1(5, 5);
	val = macro_test_2(5, 5);

	print_mem();

	my_memcpy_asm_test(0x80000, 0x100000, 32);
	
	/*内嵌汇编 lab4：使用内嵌汇编与宏的结合*/
	my_ops_test();

	/*内嵌汇编 lab5：实现读和写系统寄存器的宏*/
	test_sysregs();

	test_asm_goto(1);

	//trigger_alignment();
	printk("done\n");

	paging_init();
	test_mmu();

	gic_init(0, GIC_V2_DISTRIBUTOR_BASE, GIC_V2_CPU_INTERFACE_BASE);

	//timer_init();
	system_timer_init();
	raw_local_irq_enable();

	while (1) {
		uart_send(uart_recv());
	}
}
