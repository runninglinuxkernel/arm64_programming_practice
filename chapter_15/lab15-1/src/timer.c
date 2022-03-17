#include <asm/timer.h>
#include <asm/irq.h>
#include <io.h>
#include <asm/arm_local_reg.h>
#include <timer.h>

#define HZ 250
#define NSEC_PER_SEC    1000000000L

static unsigned int val = NSEC_PER_SEC / HZ;

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

static void enable_timer_interrupt(void)
{
	writel(CNT_PNS_IRQ, TIMER_CNTRL0);
}

void timer_init(void)
{
	generic_timer_init();
	generic_timer_reset(val);

	gicv2_unmask_irq(GENERIC_TIMER_IRQ);

	enable_timer_interrupt();
}

void handle_timer_irq(void)
{
	generic_timer_reset(val);
	printk("Core0 Timer interrupt received\r\n");
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
