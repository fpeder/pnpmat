#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>

#include "auz.h"

static char *usage = "usage: %s -i img -k size [-n iter] [-v]\n";

int niter = 1;
int ksize = 0;
int verbose = 0;
char *fname = NULL;


void parse_args(int argc, char **argv)
{
     extern char *optarg;
     int c, err=0;

     while ((c = getopt(argc, argv, "i:k:n:v")) != -1) {
          switch (c) {
          case 'i': fname = optarg; break;
          case 'k': ksize = atoi(optarg); break;
          case 'n': niter = atoi(optarg); break;
          case 'v': verbose = 1; break;
          case '?': err = 1; break;
          }
     }
     if (!fname || !ksize) {
          fprintf(stderr, usage, argv[0]);
          exit(-1);
     }
}


int main (int argc, char **argv)
{
     pedemat src, dst, tmp;
     int count;

     parse_args(argc, argv);

     src = pedemat_read(fname, ksize);
     count = pedemat_count(src);

     TIME_START;
     for (int x = 0; x < NTIMES; x++) {
          for (int i = 0; i < niter; i++) {
               tmp = i ? dst : src;
               dst = pedemat_dilate(tmp, ksize);
          }
     }
     TIME_STOP;
     TIME_NORM_PRINT(NTIMES, count, src.size); NL;

     if (verbose) pedemat_print(dst);
     pedemat_free(src);
     pedemat_free(dst);
          
     return 0;
}
