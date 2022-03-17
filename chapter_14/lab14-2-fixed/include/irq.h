static inline void arch_local_irq_enable(void)
{
	asm volatile(
		"msr	daifclr, #2"
		:
		:
		: "memory");
}

static inline void arch_local_irq_disable(void)
{
	asm volatile(
		"msr	daifset, #2"
		:
		:
		: "memory");
}


#define raw_local_irq_disable()	arch_local_irq_disable()
#define raw_local_irq_enable()	arch_local_irq_enable()

