#include <stdio.h>
#include <assert.h>

#include "util.h"
#include "bmat.h"
#include "pnpmat.h"

//------------------------------------------------------------------------
pnpmat pnpmat_erode(pnpmat mat, const int ksize)
{
     int i, j; uint64_t neigh;
     const int n = pnpmat_nneigh(mat);
     pnpmat dst = pnpmat_copy(mat);
     while (pnpmat_foreach_in(mat, &i, &j, ksize)) {
          neigh = (uint64_t)pnpmat_get_neigh(mat, i, j);
          if (neigh != (1ULL<<n) - 1)
               pnpmat_clear(dst, i, j);

     }
     return dst;
}

pnpmat pnpmat_dilate(pnpmat mat, const int ksize)
{
     int i, j; uint64_t neigh;
     //const int n = pnpmat_nneigh(mat);
     pnpmat dst = pnpmat_copy(mat);
     while (pnpmat_foreach_zero_in(mat, &i, &j, ksize)) {
          neigh = (uint64_t)pnpmat_get_neigh(mat, i, j);
          if (neigh)
               pnpmat_set(dst, i, j);
     }
     return dst;
}

//-------------------------------------------------------------------------
#define MID(s, e) ((e-s) >> 1)
#define MASK32(s,e) ((0xFFFFFFFF << (31-(e)+(s))) >> (s));
#define MASK64(s,e) ((0xFFFFFFFFFFFFFFFF << (63-(e)+(s))) >> (s));

#define OFF 64
static int off=0;
static int id=0;
static int rl=0;
static int16_t ss[RL_MAX_ROW];
static int16_t se[RL_MAX_ROW];

void split_hi(uint64_t v, int s, int e);
void split_lo(uint64_t v, int s, int e);

void split_lo(uint64_t v, int s, int e)
{
     uint64_t mask = MASK64(s, e);
     uint64_t tmp = v & mask;
     if (!tmp) return;
     if (tmp == mask) {
          ss[id]=s+off; se[id]=e+off; id++;
          return;
     }
     else {
          split_lo(v, s, s + MID(s, e));
          split_hi(v, s + MID(s, e) + 1, e);
     }
}

void split_hi(uint64_t v, int s, int e)
{
     uint64_t mask = MASK64(s, e);
     uint64_t tmp = v & mask;
     if (!tmp) return;
     if (tmp == mask) {
          ss[id] = s+off; se[id] = e+off; id++;
          return;
     }
     else {
          split_lo(v, s, s + MID(s, e));
          split_hi(v, s + MID(s,e) + 1, e);
     }
}

void sub_print(int8_t *v)
{
     for (int i=0; v[i]!=-1; printf("%d ", v[i++]));
     NL;
}

void merge(int r, uint32_t *pt)
{
     for (int i=0; se[i]!=-1; i++) {
          pt[rl++] = CNTR_PT_PACK(r, ss[i]);
          while(ss[i+1] == se[i]+1)
               i++;
          pt[rl++] = CNTR_PT_PACK(r, se[i]);
     }
}

void pnpmat_rl(pnpmat src, uint32_t *pt)
{
     uint64_t v;
     //id=0; off=0;
     for (int i=0; i<src.M; i++) {
          for (int j=0; j<src.N_; j++) {
               v = src.data[i*src.N_ + j];
               split_lo(v, 0, 63);
               off += OFF;
          }
          ss[id]=-1; se[id]=-1; id=0; off=0;
          merge(i, pt);
     }
     pt[rl]=0;
}

//-------------------------------------------------------------------------
void pnpmat_moore(pnpmat src, int *count, uint32_t *contour)
{
     const uint8_t map[] = {4, 5, 6, 7, 0, 1, 2, 3};
     const int8_t di[] = {0,  +1, +1, +1,  0, -1, -1, -1};
     const int8_t dj[] = {-1, -1,  0, +1, +1, +1,  0, -1};
     int i, j, k=0;
     uint32_t start, end=0;
     uint8_t prev=0, next, pos, neigh;
     //bmat dst = bmat_zeros(src.M, src.N);

     pnpmat_foreach(src, &i, &j); // find the first
     start = CNTR_PT_PACK(i, j);
     while (start != end) {
          neigh = pnpmat_get_neigh(src, i, j);
          /* __asm__("mov %2,%%cl; rorb %%cl,%0" */
          /*         : "=r" (neigh) */
          /*         : "r"  (neigh), "ir" (prev) */
          /*         : "%cl"); */
          neigh = (neigh >> prev) | (neigh << (8-prev));
          next = 31 - __builtin_clz(neigh);
          next = (next + prev) & 7;
          i += di[next]; j += dj[next];
          //bmat_set(dst, i, j);
          //bmat_print(dst); getchar();
          end = CNTR_PT_PACK(i, j);
          contour[k++] = end;
          prev = map[next];
          (*count)++;
     }
     contour[k] = 0;
     //return dst;
}

//-------------------------------------------------------------------------
#define TAB_LEN 256

static uint8_t ltab[TAB_LEN], rtab[TAB_LEN];
static uint8_t *tab[] = {ltab, rtab};

/* static inline */
/* int del_condition(uint8_t *tab, uint8_t config) */
/* { */
/*      return tab[config]; */
/* } */

void gh_load_tab(char *infile, uint8_t *tab)
{
     size_t i;
     FILE *pf = fopen(infile, "rb");
     assert(pf);
     for (i=0; i < TAB_LEN; i++) {
          fread(tab + i, sizeof(uint8_t), 1, pf);
     }
     fclose(pf);
}

pnpmat lookup_guohall_iter(pnpmat in, int *count)
{
     static int iter = 1;
     int i=0, j=0;
     uint8_t conf;
     pnpmat out = pnpmat_copy(in);
     while (pnpmat_foreach(in, &i, &j)) {
          conf = pnpmat_get_neigh(in, i, j);
          if (tab[iter][conf]) {
               pnpmat_clear(out, i, j);
               (*count)++;
          }
     }
     iter ^= 1;
     return out;
}

pnpmat pnpmat_guohall(pnpmat m)
{
     int count;
     gh_load_tab("build/ltab.dat", ltab);
     gh_load_tab("build/rtab.dat", rtab);
     do {
          count = 0;
          m = lookup_guohall_iter(m, &count);
     } while(count);
     return m;
}
