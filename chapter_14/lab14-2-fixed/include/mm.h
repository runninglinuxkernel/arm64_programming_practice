#ifndef	_MM_H
#define	_MM_H

#define PAGE_SHIFT	 		12
#define TABLE_SHIFT 			9
#define SECTION_SHIFT			(PAGE_SHIFT + TABLE_SHIFT)

#define PAGE_SIZE   			(1 << PAGE_SHIFT)	
#define PAGE_MASK (~(PAGE_SIZE-1))
#define SECTION_SIZE			(1 << SECTION_SHIFT)	

#define LOW_MEMORY              	(2 * SECTION_SIZE)
#define TOTAL_MEMORY (512 * 0x100000)

/* CONFIG_ARM64_VA_BITS = 48*/
#define CONFIG_ARM64_VA_BITS 48
#define VA_BITS	 (CONFIG_ARM64_VA_BITS)

/*
 * Memory types available.
 */
#define MT_DEVICE_nGnRnE	0
#define MT_DEVICE_nGnRE		1
#define MT_DEVICE_GRE		2
#define MT_NORMAL_NC		3
#define MT_NORMAL		4
#define MT_NORMAL_WT		5

#define MAIR(attr, mt)	((attr) << ((mt) * 8))


#ifndef __ASSEMBLER__

void memzero(unsigned long src, unsigned long n);

#endif

/* to align the pointer to the (next) page boundary */
#define PAGE_ALIGN(addr) (((addr)+PAGE_SIZE-1)&PAGE_MASK)
#define PAGE_ALIGN_UP(addr)  PAGE_ALIGN(addr)
#define PAGE_ALIGN_DOWN(addr) (addr & PAGE_MASK)

#endif  /*_MM_H */
