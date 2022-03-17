
#ifndef _P_IO_H
#define _P_BASE_H

#if 1
#define __arch_getl(a)			(*(volatile unsigned int *)(a))
#define __arch_putl(v,a)		(*(volatile unsigned int *)(a) = (v))

#define dmb()		__asm__ __volatile__ ("" : : : "memory")
#define __iormb()	dmb()
#define __iowmb()	dmb()

#define readl(c)	({ unsigned int  __v = __arch_getl(c); __iormb(); __v; })
#define writel(v,c)	({ unsigned int  __v = v; __iowmb(); __arch_putl(__v,c);})
#else
static inline void writel(unsigned int value, unsigned int addr)
{
	*(volatile unsigned int *)addr = value;
}

static inline unsigned int readl(unsigned int addr)
{
	return *(volatile unsigned int *)addr;
}
#endif

static inline void delay(unsigned int n)
{
	while(n--)
		;
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

#endif
