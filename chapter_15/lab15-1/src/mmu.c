#include <asm/pgtable.h>
#include <asm/pgtable_prot.h>
#include <asm/pgtable_hwdef.h>
#include <sysregs.h>
#include <asm/barrier.h>
#include <string.h>
#include <asm/base.h>

#define NO_BLOCK_MAPPINGS BIT(0)
#define NO_CONT_MAPPINGS BIT(1)

extern char idmap_pg_dir[];

extern char _text_boot[], _etext_boot[];
extern char _text[], _etext[];

static void alloc_init_pte(pmd_t *pmdp, unsigned long addr,
		unsigned long end, unsigned long phys,
		unsigned long prot,
		unsigned long (*alloc_pgtable)(void),
		unsigned long flags)
{
	pmd_t pmd = *pmdp;
	pte_t *ptep;

	if (pmd_none(pmd)) {
		unsigned long pte_phys;

		pte_phys = alloc_pgtable();
		set_pmd(pmdp, __pmd(pte_phys | PMD_TYPE_TABLE));
		pmd = *pmdp;
	}

	ptep = pte_offset_phys(pmdp, addr);
	do {
		set_pte(ptep, pfn_pte(phys >> PAGE_SHIFT, prot));
		phys += PAGE_SIZE;
	} while (ptep++, addr += PAGE_SIZE, addr != end);
}

void pmd_set_section(pmd_t *pmdp, unsigned long phys,
		unsigned long prot)
{
	unsigned long sect_prot = PMD_TYPE_SECT | mk_sect_prot(prot);

	pmd_t new_pmd = pfn_pmd(phys >> PMD_SHIFT, sect_prot);

	set_pmd(pmdp, new_pmd);
}

static void alloc_init_pmd(pud_t *pudp, unsigned long addr,
		unsigned long end, unsigned long phys,
		unsigned long prot,
		unsigned long (*alloc_pgtable)(void),
		unsigned long flags)
{
	pud_t pud = *pudp;
	pmd_t *pmdp;
	unsigned long next;

	if (pud_none(pud)) {
		unsigned long pmd_phys;

		pmd_phys = alloc_pgtable();
		set_pud(pudp, __pud(pmd_phys | PUD_TYPE_TABLE));
		pud = *pudp;
	}

	pmdp = pmd_offset_phys(pudp, addr);
	do {
		next = pmd_addr_end(addr, end);

		if (((addr | next | phys) & ~SECTION_MASK) == 0 &&
				(flags & NO_BLOCK_MAPPINGS) == 0)
			pmd_set_section(pmdp, phys, prot);
		else
			alloc_init_pte(pmdp, addr, next, phys,
					prot,  alloc_pgtable, flags);

		phys += next - addr;
	} while (pmdp++, addr = next, addr != end);
}

static void alloc_init_pud(pgd_t *pgdp, unsigned long addr,
		unsigned long end, unsigned long phys,
		unsigned long prot,
		unsigned long (*alloc_pgtable)(void),
		unsigned long flags)
{
	pgd_t pgd = *pgdp;
	pud_t *pudp;
	unsigned long next;

	if (pgd_none(pgd)) {
		unsigned long pud_phys;

		pud_phys = alloc_pgtable();

		set_pgd(pgdp, __pgd(pud_phys | PUD_TYPE_TABLE));
		pgd = *pgdp;
	}

	pudp = pud_offset_phys(pgdp, addr);
	do {
		next = pud_addr_end(addr, end);
		alloc_init_pmd(pudp, addr, next, phys,
				prot, alloc_pgtable, flags);
		phys += next - addr;

	} while (pudp++, addr = next, addr != end);
}

static void __create_pgd_mapping(pgd_t *pgdir, unsigned long phys,
		unsigned long virt, unsigned long size,
		unsigned long prot,
		unsigned long (*alloc_pgtable)(void),
		unsigned long flags)
{
	pgd_t *pgdp = pgd_offset_raw(pgdir, virt);
	unsigned long addr, end, next;

	phys &= PAGE_MASK;
	addr = virt & PAGE_MASK;
	end = PAGE_ALIGN(virt + size);

	do {
		next = pgd_addr_end(addr, end);
		alloc_init_pud(pgdp, addr, next, phys,
				prot, alloc_pgtable, flags);
		phys += next - addr;
	} while (pgdp++, addr = next, addr != end);
}

static unsigned long early_pgtable_alloc(void)
{
	unsigned long phys;

	phys = get_free_page();
	memset((void *)phys, 0, PAGE_SIZE);

	return phys;
}

static void create_identical_mapping(void)
{
	unsigned long start;
	unsigned long end;

	/*map text*/
	start = (unsigned long)_text_boot;
	end = (unsigned long)_etext;
	__create_pgd_mapping((pgd_t *)idmap_pg_dir, start, start,
			end - start, PAGE_KERNEL_ROX,
			early_pgtable_alloc,
			0);

	/*map memory*/
	start = PAGE_ALIGN((unsigned long)_etext);
	end = TOTAL_MEMORY;
	__create_pgd_mapping((pgd_t *)idmap_pg_dir, start, start,
			end - start, PAGE_KERNEL,
			early_pgtable_alloc,
			0);
}

static void create_mmio_mapping(void)
{
	__create_pgd_mapping((pgd_t *)idmap_pg_dir, PBASE, PBASE,
			DEVICE_SIZE, PROT_DEVICE_nGnRnE,
			early_pgtable_alloc,
			0);
}

void paging_init(void)
{
	create_identical_mapping();
	create_mmio_mapping();
}
