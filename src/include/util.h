#ifndef UTIL_H
#define UTIL_H

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#define NL printf("\n")

#define CNTR_MAX_PT 1000000
#define CNTR_PT_PACK(a, b) ((a) << 16) | (b)
#define RL_MAX_ROW 1000
#define RL_MAX 1000000


void bin_print(uint64_t x);
void cntr_print(uint32_t *c);
void cntr_write(uint32_t *c, char *file);
void rl_print(uint64_t *c);

#endif /* UTIL_H */
