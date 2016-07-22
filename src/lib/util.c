#include <stdio.h>
#include <stdlib.h>

#include "util.h"


void bin_print(uint64_t x)
{
    for (int i = 63; i >= 0; i--)
         printf("%d", (int)((x >> i) &1ULL));
    printf(" ");
}

void cntr_print(uint32_t *c)
{
     for(int i=0; c[i]; i++) {
          printf("(%d,%d)", c[i] >> 16, c[i] & 0xFFFF);
     }
}

void cntr_write(uint32_t *c, char *file)
{
     FILE *pf = fopen(file, "w");
     assert(pf);
     for (int i=0; c[i]; i++) {
          fprintf(pf, "%d %d\n", c[i] >> 16, c[i] & 0xFFFF);
     }
     fclose(pf);
}
