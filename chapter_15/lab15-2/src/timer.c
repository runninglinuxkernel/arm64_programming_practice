#include <asm/timer.h>
#include <asm/irq.h>
#include <io.h>
#include <asm/arm_local_reg.h>
#include <timer.h>
#include <type.h>

#define HZ 250

unsigned long volatile cacheline_aligned jiffies;

static unsigned int arch_timer_rate;

static int generic_timer_init(void)
{
	asm volatile(
		"mov x0, #1\n"
		"msr cntp_ctl_el0, x0"
		:
		:
		: "memory");

	return 0;
}

static int generic_timer_reset(unsigned int val)
{
	asm volatile(
		"msr cntp_tval_el0, %x[timer_val]"
		:
		: [timer_val] "r" (val)
		: "memory");

	return 0;
}

static unsigned int generic_timer_get_freq(void)
{
	unsigned int freq;

	asm volatile(
		"mrs %0, cntfrq_el0"
		: "=r" (freq)
		:
		: "memory");

	return freq;
}

static void enable_timer_interrupt(void)
{
	writel(CNT_PNS_IRQ, TIMER_CNTRL0);
}

void timer_init(void)
{
	arch_timer_rate = generic_timer_get_freq();
	printk("cntp freq:0x%x\r\n", arch_timer_rate);
	arch_timer_rate /= HZ;

	generic_timer_init();
	generic_timer_reset(arch_timer_rate);

	gicv2_unmask_irq(GENERIC_TIMER_IRQ);

	enable_timer_interrupt();
}

void handle_timer_irq(void)
{
	generic_timer_reset(arch_timer_rate);
	//printk("Core0 Timer interrupt received\r\n");
	jiffies++;
}

static unsigned int stimer_val = 0;
static unsigned int sval = 200000;

void system_timer_init(void)
{
	stimer_val = readl(TIMER_CLO);
	stimer_val += sval;
	writel(stimer_val, TIMER_C1);

	gicv2_unmask_irq(SYSTEM_TIMER1_IRQ);

	/* enable system timer*/
	writel(SYSTEM_TIMER_IRQ_1, ENABLE_IRQS_0);
}

void handle_stimer_irq(void)
{
	stimer_val += sval;
	writel(stimer_val, TIMER_C1);
	writel(TIMER_CS_M1, TIMER_CS);
	printk("Sytem Timer1 interrupt \n");
}
