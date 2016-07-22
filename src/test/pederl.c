#include <stdio.h>
#include <unistd.h>

#include "auz.h"


const char usage[] = "usage: %s -i img [-v]\n";
int verbose = 0;
char *fname = NULL;

void parse_args(int argc, char **argv)
{
     extern char *optarg ; int c, err=0;
     while ((c = getopt(argc, argv, "i:v")) != -1) {
          switch (c) {
          case 'i': fname = optarg; break;
          case 'v': verbose = 1; break;
          case '?': err =1;
          }
     }
     if (!fname || err) {
          fprintf(stderr, usage, argv[0]);
          exit(-1);
     }
}

int main(int argc, char **argv)
{
     pnpmat src; int count;
     uint32_t rl[RL_MAX];

     parse_args(argc, argv);
     src = pnpmat_read(fname, 3);
     count = pnpmat_count(src);

     TIME_START;
     pnpmat_rl(src, rl);
     TIME_STOP; TIME_NORM_PRINT(1, count, src.size); NL;

     if (verbose) cntr_print(rl);
     cntr_write(rl, "rl.our.txt");
     pnpmat_free(src);
     return 0;
}
