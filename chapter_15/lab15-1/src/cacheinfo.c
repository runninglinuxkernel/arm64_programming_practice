#include "io.h"
#include "type.h"

enum cache_type {
	CACHE_TYPE_NOCACHE = 0,
	CACHE_TYPE_INST = BIT(0),
	CACHE_TYPE_DATA = BIT(1),
	CACHE_TYPE_SEPARATE = CACHE_TYPE_INST | CACHE_TYPE_DATA,
	CACHE_TYPE_UNIFIED = BIT(2),
};

static const char * cache_type_string[] = {
	"nocache",
	"i-cache",
	"d-cache",
	"separate cache",
	"unifed cache"
};

#define ICACHE_POLICY_VPIPT	0
#define ICACHE_POLICY_VIPT	2
#define ICACHE_POLICY_PIPT	3

static const char *icache_policy_str[] = {
	[0 ... ICACHE_POLICY_PIPT]	= "RESERVED/UNKNOWN",
	[ICACHE_POLICY_VIPT]		= "VIPT",
	[ICACHE_POLICY_PIPT]		= "PIPT",
	[ICACHE_POLICY_VPIPT]		= "VPIPT",
};

#define CTR_L1IP_SHIFT		14
#define CTR_L1IP_MASK		3
#define CTR_DMINLINE_SHIFT	16
#define CTR_IMINLINE_SHIFT	0
#define CTR_ERG_SHIFT		20
#define CTR_CWG_SHIFT		24
#define CTR_CWG_MASK		15
#define CTR_IDC_SHIFT		28
#define CTR_DIC_SHIFT		29

#define CTR_L1IP(ctr)		(((ctr) >> CTR_L1IP_SHIFT) & CTR_L1IP_MASK)

#define CSSELR_IND_I     BIT(0)
#define CSSELR_LEVEL_SHIFT 1

#define CCSIDR_NUMSETS_SHIFT 13
#define CCSIDR_NUMSETS_MASK (0x1fff << CCSIDR_NUMSETS_SHIFT)
#define CCSIDR_ASS_SHIFT 3
#define CCSIDR_ASS_MASK (0x3ff << CCSIDR_ASS_SHIFT)
#define CCSIDR_LINESIZE_MASK (0x7)

/* armv8架构最多支持7级cache */
#define MAX_CACHE_LEVEL		7

#define CLIDR_ICB_SHIFT         30
#define CLIDR_LOUU_SHIFT	27
#define CLIDR_LOC_SHIFT		24
#define CLIDR_LOUIS_SHIFT	21

#define CLIDR_ICB(clidr)	(((clidr) >> CLIDR_ICB_SHIFT) & 0x7)
#define CLIDR_LOUU(clidr)	(((clidr) >> CLIDR_LOUU_SHIFT) & 0x7)
#define CLIDR_LOC(clidr)	(((clidr) >> CLIDR_LOC_SHIFT) & 0x7)
#define CLIDR_LOUIS(clidr)	(((clidr) >> CLIDR_LOUIS_SHIFT) & 0x7)

/* Ctypen, bits[3(n - 1) + 2 : 3(n - 1)], for n = 1 to 7 */
#define CLIDR_CTYPE_SHIFT(level)	(3 * (level - 1))
#define CLIDR_CTYPE_MASK(level)		(7 << CLIDR_CTYPE_SHIFT(level))
#define CLIDR_CTYPE(clidr, level)	\
	(((clidr) & CLIDR_CTYPE_MASK(level)) >> CLIDR_CTYPE_SHIFT(level))

static inline unsigned int cache_type_cwg(void)
{
	return (read_sysreg(CTR_EL0) >> CTR_CWG_SHIFT) & CTR_CWG_MASK;
}

/* 获取cache的line size */
static inline int cache_line_size(void)
{
	u32 cwg = cache_type_cwg();
	return 4 << cwg;
}

/* 获取cache的类型*/
static inline enum cache_type get_cache_type(int level)
{
	unsigned long clidr;

	if (level > MAX_CACHE_LEVEL)
		return CACHE_TYPE_NOCACHE;
	clidr = read_sysreg(clidr_el1);
	return CLIDR_CTYPE(clidr, level);
}

/*
 * 获取每一级cache的way和set
 *
 * 从树莓派官网可以知道：
 * https://www.raspberrypi.org/documentation/hardware/raspberrypi/bcm2711/README.md
 *
 * Caches: 32 KB data + 48 KB instruction L1 cache per core. 1MB L2 cache.
 * */
static void get_cache_set_way(unsigned int level, unsigned int ind)
{
	unsigned long val;
	unsigned int line_size, set, way;
	int tmp;

	/* 1. 先写CSSELR_EL1寄存器，告知要查询那个cache */
	tmp = (level -1) << CSSELR_LEVEL_SHIFT | ind;
	write_sysreg(tmp, CSSELR_EL1);

	/*
	 * 2. 读取CCSIDR_EL1寄存器的值，当没有实现ARMv8.3-CCIDX时，这个寄存器只有低32为有效。
	 * 注意这个寄存器有两种layout的方式。
	 * */
	val = read_sysreg(CCSIDR_EL1);

	set = (val & CCSIDR_NUMSETS_MASK) >> CCSIDR_NUMSETS_SHIFT;
	set += 1;
	way = (val &  CCSIDR_ASS_MASK) >> CCSIDR_ASS_SHIFT;
	way += 1;

	line_size = (val & CCSIDR_LINESIZE_MASK);
	line_size = 1 << (line_size + 4);

	printk("          %s: set %u way %u line_size %u size %uKB\n",
			ind ? "i-cache":"d/u cache", set, way, line_size,
			(line_size * way * set)/1024);
}

int init_cache_info(void)
{
	int level;
	unsigned long ctype;

	printk("parse cache info:\n");

	for (level = 1; level <= MAX_CACHE_LEVEL; level++) {
		/* 获取cache type */
		ctype = get_cache_type(level);
		/* 如果cache type为NONCACHE，则退出循环 */
		if (ctype == CACHE_TYPE_NOCACHE) {
			level--;
			break;
		}
		printk("   L%u: %s, cache line size %u\n",
				level, cache_type_string[ctype], cache_line_size());
		if (ctype == CACHE_TYPE_SEPARATE) {
			get_cache_set_way(level, 1);
			get_cache_set_way(level, 0);
		} else if (ctype == CACHE_TYPE_UNIFIED)
			get_cache_set_way(level, 0);
	}

	/*
	 * 获取ICB，LOUU，LOC和LOUIS
	 * ICB: Inner cache boundary
	 * LOUU: 单核处理器PoU的cache边界。
	 * LOC: PoC的cache边界
	 * LOUIS:PoU for inner share的cache边界。 
	 * */
	unsigned clidr = read_sysreg(clidr_el1);
	printk("   IBC:%u LOUU:%u LoC:%u LoUIS:%u\n",
			CLIDR_ICB(clidr), CLIDR_LOUU(clidr),
			CLIDR_LOC(clidr), CLIDR_LOUIS(clidr));

	unsigned ctr = read_sysreg(ctr_el0);
	printk("   Detected %s I-cache\n", icache_policy_str[CTR_L1IP(ctr)]);

	return level;
}
