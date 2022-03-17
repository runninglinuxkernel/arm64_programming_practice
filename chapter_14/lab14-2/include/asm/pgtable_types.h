#ifndef ASM_PGTABLE_TYPES_H
#define ASM_PGTABLE_TYPES_H

#include <type.h>

typedef u64 pteval_t;
typedef u64 pmdval_t;
typedef u64 pudval_t;
typedef u64 pgdval_t;

typedef struct {
	pteval_t pte;
} pte_t;
#define pte_val(x) ((x).pte)
#define __pte(x) ((pte_t) { (x) })

typedef struct {
	pmdval_t pmd;
} pmd_t;
#define pmd_val(x) ((x).pmd)
#define __pmd(x) ((pmd_t) { (x) })

typedef struct {
	pudval_t pud;
} pud_t;
#define pud_val(x) ((x).pud)
#define __pud(x) ((pud_t) { (x) })

typedef struct {
	pgdval_t pgd;
} pgd_t;
#define pgd_val(x) ((x).pgd)
#define __pgd(x) ((pgd_t) { (x) })

#endif /*ASM_PGTABLE_TYPES_H*/
