#include <arm-gic.h>
#include "io.h"
#include <asm/irq.h>

struct gic_chip_data {
	unsigned long raw_dist_base;
	unsigned long raw_cpu_base;
	struct irq_domain *domain;
	struct irq_chip *chip;
	unsigned int gic_irqs;
};

#define gic_dist_base(d) ((d)->raw_dist_base)
#define gic_cpu_base(d) ((d)->raw_cpu_base)

#define ARM_GIC_MAX_NR 1
static struct gic_chip_data gic_data[ARM_GIC_MAX_NR];

/* IRQs start ID */
#define HW_IRQ_START 16

static unsigned long gic_get_dist_base(void)
{
	struct gic_chip_data *gic = &gic_data[0];

	return gic_dist_base(gic);
}

static unsigned long gic_get_cpu_base(void)
{
	struct gic_chip_data *gic = &gic_data[0];

	return gic_cpu_base(gic);
}

static void gic_set_irq(int irq, unsigned int offset)
{
	unsigned int mask = 1 << (irq % 32);

	writel(mask, gic_get_dist_base() + offset + (irq / 32) * 4);
}

void gicv2_mask_irq(int irq)
{
	gic_set_irq(irq, GIC_DIST_ENABLE_CLEAR);
}

void gicv2_unmask_irq(int irq)
{
	gic_set_irq(irq, GIC_DIST_ENABLE_SET);
}

void gicv2_eoi_irq(int irq)
{
	writel(irq, gic_get_cpu_base() + GIC_CPU_EOI);
}

static unsigned int gic_get_cpumask(struct gic_chip_data *gic)
{
	unsigned long base = gic_dist_base(gic);
	unsigned int mask, i;

	for (i = mask = 0; i < 32; i += 4) {
		mask = readl(base + GIC_DIST_TARGET + i);
		mask |= mask >> 16;
		mask |= mask >> 8;
		if (mask)
			break;
	}

	return mask;
}

static void gic_dist_init(struct gic_chip_data *gic)
{
	unsigned long base = gic_dist_base(gic);
	unsigned int cpumask;
	unsigned int gic_irqs = gic->gic_irqs;
	int i;

	/* 关闭中断*/
	writel(GICD_DISABLE, base + GIC_DIST_CTRL);

	/* 设置中断路由：GIC_DIST_TARGET
	 *
	 * 前32个中断怎么路由是GIC芯片固定的，因此先读GIC_DIST_TARGET前面的值
	 * 然后全部填充到 SPI的中断号 */
	cpumask = gic_get_cpumask(gic);
	cpumask |= cpumask << 8;
	cpumask |= cpumask << 16;

	for (i = 32; i < gic_irqs; i += 4)
		writel(cpumask, base + GIC_DIST_TARGET + i * 4 / 4);

	/* Set all global interrupts to be level triggered, active low */
	for (i = 32; i < gic_irqs; i += 16)
		writel(GICD_INT_ACTLOW_LVLTRIG, base + GIC_DIST_CONFIG + i / 4);

	/* Deactivate and disable all 中断（SGI， PPI， SPI）.
	 *
	 * 当注册中断的时候才 enable某个一个SPI中断，例如调用gic_unmask_irq()*/
	for (i = 0; i < gic_irqs; i += 32) {
		writel(GICD_INT_EN_CLR_X32, base +
				GIC_DIST_ACTIVE_CLEAR + i / 8);
		writel(GICD_INT_EN_CLR_X32, base +
				GIC_DIST_ENABLE_CLEAR + i / 8);
	}

	/*打开SGI中断（0～15），可能SMP会用到*/
	writel(GICD_INT_EN_SET_SGI, base + GIC_DIST_ENABLE_SET);

	/* 打开中断：Enable group0 and group1 interrupt forwarding.*/
	writel(GICD_ENABLE, base + GIC_DIST_CTRL);
}

void gic_cpu_init(struct gic_chip_data *gic)
{
	int i;
	unsigned long base = gic_cpu_base(gic);
	unsigned long dist_base = gic_dist_base(gic);

	/*
	 * Set priority on PPI and SGI interrupts
	 */
	for (i = 0; i < 32; i += 4)
		writel(0xa0a0a0a0,
			dist_base + GIC_DIST_PRI + i * 4 / 4);

	writel(GICC_INT_PRI_THRESHOLD, base + GIC_CPU_PRIMASK);

	writel(GICC_ENABLE, base + GIC_CPU_CTRL);
}

void gic_handle_irq(void)
{
	struct gic_chip_data *gic = &gic_data[0];
	unsigned long base = gic_cpu_base(gic);
	unsigned int irqstat, irqnr;

	do {
		irqstat = readl(base + GIC_CPU_INTACK);
		irqnr = irqstat & GICC_IAR_INT_ID_MASK;

		printk("%s: irqnr %d\n", __func__, irqnr);

		if (irqnr == GENERIC_TIMER_IRQ)
			handle_timer_irq();
		else if (irqnr == SYSTEM_TIMER1_IRQ)
			handle_stimer_irq();

		gicv2_eoi_irq(irqnr);

	} while (0);

}

int gic_init(int chip, unsigned long dist_base, unsigned long cpu_base)
{
	struct gic_chip_data *gic;
	int gic_irqs;
	int virq_base;

	gic = &gic_data[chip];

	gic->raw_cpu_base = cpu_base;
	gic->raw_dist_base = dist_base;

	/* readout how many interrupts are supported*/
	gic_irqs = readl(gic_dist_base(gic) + GIC_DIST_CTR) & 0x1f;
	gic_irqs = (gic_irqs + 1) * 32;
	if (gic_irqs > 1020)
		gic_irqs = 1020;
	gic->gic_irqs = gic_irqs;

	printk("%s: cpu_base:0x%x, dist_base:0x%x, gic_irqs:%d\n",
			__func__, cpu_base, dist_base, gic->gic_irqs);

	gic_dist_init(gic);
	gic_cpu_init(gic);

	return 0;
}

