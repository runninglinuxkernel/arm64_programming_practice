#ifndef ASM_PGTABLE_HWDEF_H
#define ASM_PGTABLE_HWDEF_H

/*
 * 暂时只4KB大小的页面和48位地址位宽
 */

/* PGD */
#define PGDIR_SHIFT 39
#define PGDIR_SIZE (1UL << PGDIR_SHIFT)
#define PGDIR_MASK (~(PGDIR_SIZE-1))
#define PTRS_PER_PGD (1 << (VA_BITS - PGDIR_SHIFT))

/* PUD */
#define PUD_SHIFT 30
#define PUD_SIZE (1UL << PUD_SHIFT)
#define PUD_MASK (~(PUD_SIZE-1))
#define PTRS_PER_PUD (1 << (PGDIR_SHIFT - PUD_SHIFT))

/* PMD */
#define PMD_SHIFT 21
#define PMD_SIZE (1UL << PMD_SHIFT)
#define PMD_MASK (~(PMD_SIZE-1))
#define PTRS_PER_PMD (1 << (PUD_SHIFT - PMD_SHIFT))

/* PTE */
#define PTE_SHIFT 12
#define PTE_SIZE (1UL << PTE_SHIFT)
#define PTE_MASK (~(PTE_SIZE-1))
#define PTRS_PER_PTE (1 << (PMD_SHIFT - PTE_SHIFT))

/* Section */
#define SECTION_SHIFT	PMD_SHIFT
#define SECTION_SIZE	(1UL << SECTION_SHIFT)
#define SECTION_MASK	(~(SECTION_SIZE-1))

/*
 * Hardware page table definitions.
 *
 * Level 1 descriptor (PUD).
 */
#define PUD_TYPE_TABLE		(3UL << 0)
#define PUD_TABLE_BIT		(1UL << 1)
#define PUD_TYPE_MASK		(3UL << 0)
#define PUD_TYPE_SECT		(1UL << 0)

/*
 * Level 2 descriptor (PMD).
 */
#define PMD_TYPE_MASK		(3UL << 0)
#define PMD_TYPE_FAULT		(0UL << 0)
#define PMD_TYPE_TABLE		(3UL << 0)
#define PMD_TYPE_SECT		(1UL << 0)
#define PMD_TABLE_BIT		(1UL << 1)

/*
 * Section
 */
#define PMD_SECT_VALID		(1UL << 0)
#define PMD_SECT_USER		(1UL << 6)		/* AP[1] */
#define PMD_SECT_RDONLY		(1UL << 7)		/* AP[2] */
#define PMD_SECT_S		(3UL << 8)
#define PMD_SECT_AF		(1UL << 10)
#define PMD_SECT_NG		(1UL << 11)
#define PMD_SECT_CONT		(1UL << 52)
#define PMD_SECT_PXN		(1UL << 53)
#define PMD_SECT_UXN		(1UL << 54)

/*
 * AttrIndx[2:0] encoding (mapping attributes defined in the MAIR* registers).
 */
#define PMD_ATTRINDX(t)		((t) << 2)
#define PMD_ATTRINDX_MASK	(7UL << 2)

/*
 * Level 3 descriptor (PTE).
 */
#define PTE_TYPE_MASK  (3UL << 0)
#define PTE_TYPE_FAULT (0UL << 0)
#define PTE_TYPE_PAGE  (3UL << 0)
#define PTE_TABLE_BIT  (1UL << 1)
#define PTE_USER       (1UL << 6) /* AP[1] */
#define PTE_RDONLY     (1UL << 7) /* AP[2] */
#define PTE_SHARED     (3UL << 8) /* SH[1:0], inner shareable */
#define PTE_AF	       (1UL << 10)	/* Access Flag */
#define PTE_NG	       (1UL << 11)	/* nG */
#define PTE_DBM	       (1UL << 51)	/* Dirty Bit Management */
#define PTE_CONT       (1UL << 52)	/* Contiguous range */
#define PTE_PXN	       (1UL << 53)	/* Privileged XN */
#define PTE_UXN	       (1UL << 54)	/* User XN */
#define PTE_HYP_XN  (1UL << 54)	/* HYP XN */

#define PTE_ADDR_LOW (((1UL << (48 - PAGE_SHIFT)) - 1) << PAGE_SHIFT)
#define PTE_ADDR_MASK PTE_ADDR_LOW

/*
 * AttrIndx[2:0] encoding
 * (mapping attributes defined in the MAIR* registers).
 */
#define PTE_ATTRINDX(t)		((t) << 2)
#define PTE_ATTRINDX_MASK	(7 << 2)

/*
 * TCR flags.
 */
#define TCR_T0SZ_OFFSET		0
#define TCR_T1SZ_OFFSET		16
#define TCR_T0SZ(x)		((UL(64) - (x)) << TCR_T0SZ_OFFSET)
#define TCR_T1SZ(x)		((UL(64) - (x)) << TCR_T1SZ_OFFSET)
#define TCR_TxSZ(x)		(TCR_T0SZ(x) | TCR_T1SZ(x))
#define TCR_TxSZ_WIDTH		6
#define TCR_T0SZ_MASK		(((UL(1) << TCR_TxSZ_WIDTH) - 1) << TCR_T0SZ_OFFSET)

#define TCR_EPD0_SHIFT		7
#define TCR_EPD0_MASK		(UL(1) << TCR_EPD0_SHIFT)
#define TCR_IRGN0_SHIFT		8
#define TCR_IRGN0_MASK		(UL(3) << TCR_IRGN0_SHIFT)
#define TCR_IRGN0_NC		(UL(0) << TCR_IRGN0_SHIFT)
#define TCR_IRGN0_WBWA		(UL(1) << TCR_IRGN0_SHIFT)
#define TCR_IRGN0_WT		(UL(2) << TCR_IRGN0_SHIFT)
#define TCR_IRGN0_WBnWA		(UL(3) << TCR_IRGN0_SHIFT)

#define TCR_EPD1_SHIFT		23
#define TCR_EPD1_MASK		(UL(1) << TCR_EPD1_SHIFT)
#define TCR_IRGN1_SHIFT		24
#define TCR_IRGN1_MASK		(UL(3) << TCR_IRGN1_SHIFT)
#define TCR_IRGN1_NC		(UL(0) << TCR_IRGN1_SHIFT)
#define TCR_IRGN1_WBWA		(UL(1) << TCR_IRGN1_SHIFT)
#define TCR_IRGN1_WT		(UL(2) << TCR_IRGN1_SHIFT)
#define TCR_IRGN1_WBnWA		(UL(3) << TCR_IRGN1_SHIFT)

#define TCR_IRGN_NC		(TCR_IRGN0_NC | TCR_IRGN1_NC)
#define TCR_IRGN_WBWA		(TCR_IRGN0_WBWA | TCR_IRGN1_WBWA)
#define TCR_IRGN_WT		(TCR_IRGN0_WT | TCR_IRGN1_WT)
#define TCR_IRGN_WBnWA		(TCR_IRGN0_WBnWA | TCR_IRGN1_WBnWA)
#define TCR_IRGN_MASK		(TCR_IRGN0_MASK | TCR_IRGN1_MASK)


#define TCR_ORGN0_SHIFT		10
#define TCR_ORGN0_MASK		(UL(3) << TCR_ORGN0_SHIFT)
#define TCR_ORGN0_NC		(UL(0) << TCR_ORGN0_SHIFT)
#define TCR_ORGN0_WBWA		(UL(1) << TCR_ORGN0_SHIFT)
#define TCR_ORGN0_WT		(UL(2) << TCR_ORGN0_SHIFT)
#define TCR_ORGN0_WBnWA		(UL(3) << TCR_ORGN0_SHIFT)

#define TCR_ORGN1_SHIFT		26
#define TCR_ORGN1_MASK		(UL(3) << TCR_ORGN1_SHIFT)
#define TCR_ORGN1_NC		(UL(0) << TCR_ORGN1_SHIFT)
#define TCR_ORGN1_WBWA		(UL(1) << TCR_ORGN1_SHIFT)
#define TCR_ORGN1_WT		(UL(2) << TCR_ORGN1_SHIFT)
#define TCR_ORGN1_WBnWA		(UL(3) << TCR_ORGN1_SHIFT)

#define TCR_ORGN_NC		(TCR_ORGN0_NC | TCR_ORGN1_NC)
#define TCR_ORGN_WBWA		(TCR_ORGN0_WBWA | TCR_ORGN1_WBWA)
#define TCR_ORGN_WT		(TCR_ORGN0_WT | TCR_ORGN1_WT)
#define TCR_ORGN_WBnWA		(TCR_ORGN0_WBnWA | TCR_ORGN1_WBnWA)
#define TCR_ORGN_MASK		(TCR_ORGN0_MASK | TCR_ORGN1_MASK)

#define TCR_SH0_SHIFT		12
#define TCR_SH0_MASK		(UL(3) << TCR_SH0_SHIFT)
#define TCR_SH0_INNER		(UL(3) << TCR_SH0_SHIFT)

#define TCR_SH1_SHIFT		28
#define TCR_SH1_MASK		(UL(3) << TCR_SH1_SHIFT)
#define TCR_SH1_INNER		(UL(3) << TCR_SH1_SHIFT)
#define TCR_SHARED		(TCR_SH0_INNER | TCR_SH1_INNER)

#define TCR_TG0_SHIFT		14
#define TCR_TG0_MASK		(UL(3) << TCR_TG0_SHIFT)
#define TCR_TG0_4K		(UL(0) << TCR_TG0_SHIFT)
#define TCR_TG0_64K		(UL(1) << TCR_TG0_SHIFT)
#define TCR_TG0_16K		(UL(2) << TCR_TG0_SHIFT)

#define TCR_TG1_SHIFT		30
#define TCR_TG1_MASK		(UL(3) << TCR_TG1_SHIFT)
#define TCR_TG1_16K		(UL(1) << TCR_TG1_SHIFT)
#define TCR_TG1_4K		(UL(2) << TCR_TG1_SHIFT)
#define TCR_TG1_64K		(UL(3) << TCR_TG1_SHIFT)

#define TCR_IPS_SHIFT		32
#define TCR_IPS_MASK		(UL(7) << TCR_IPS_SHIFT)
#define TCR_A1			(UL(1) << 22)
#define TCR_ASID16		(UL(1) << 36)
#define TCR_TBI0		(UL(1) << 37)
#define TCR_TBI1		(UL(1) << 38)
#define TCR_HA			(UL(1) << 39)
#define TCR_HD			(UL(1) << 40)
#define TCR_NFD1		(UL(1) << 54)

#define TCR_TG_FLAGS    (TCR_TG0_4K | TCR_TG1_4K)
#define TCR_KASLR_FLAGS 0
#define TCR_KASAN_FLAGS 0
#define TCR_SMP_FLAGS   TCR_SHARED
#define TCR_CACHE_FLAGS (TCR_IRGN_WBWA | TCR_ORGN_WBWA)

/* id_aa64mmfr0 */
#define ID_AA64MMFR0_TGRAN4_SHIFT	28
#define ID_AA64MMFR0_TGRAN64_SHIFT	24
#define ID_AA64MMFR0_TGRAN16_SHIFT	20
#define ID_AA64MMFR0_BIGENDEL0_SHIFT	16
#define ID_AA64MMFR0_SNSMEM_SHIFT	12
#define ID_AA64MMFR0_BIGENDEL_SHIFT	8
#define ID_AA64MMFR0_ASID_SHIFT		4
#define ID_AA64MMFR0_PARANGE_SHIFT	0

#define ID_AA64MMFR0_TGRAN4_NI		0xf
#define ID_AA64MMFR0_TGRAN4_SUPPORTED	0x0
#define ID_AA64MMFR0_TGRAN64_NI		0xf
#define ID_AA64MMFR0_TGRAN64_SUPPORTED	0x0
#define ID_AA64MMFR0_TGRAN16_NI		0x0
#define ID_AA64MMFR0_TGRAN16_SUPPORTED	0x1
#define ID_AA64MMFR0_PARANGE_48		0x5
#define ID_AA64MMFR0_PARANGE_52		0x6

#if defined(CONFIG_ARM64_4K_PAGES)
#define ID_AA64MMFR0_TGRAN_SHIFT	ID_AA64MMFR0_TGRAN4_SHIFT
#define ID_AA64MMFR0_TGRAN_SUPPORTED	ID_AA64MMFR0_TGRAN4_SUPPORTED
#endif

#endif /*ASM_PGTABLE_HWDEF_H*/
