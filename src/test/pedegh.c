#include <stdio.h>
#include <unistd.h>

#include "auz.h"

#define NTIMES 1

const char usage[] = "usage: %s -i img [-s out] [-v]\n";
int verbose = 0;
char *fname=NULL, *outfile=NULL;

void parse_args(int argc, char **argv)
{
     extern char *optarg;
     int c, err=0;
     while ((c = getopt(argc, argv, "i:s:v")) != -1) {
          switch (c) {
          case 'i': fname = optarg; break;
          case 'v': verbose = 1; break;
          case 's': outfile = optarg; break;
          case '?': err = 1;
          }
     }
     if (!fname || err) {
          fprintf(stderr, usage, argv[0]);
          exit(-1);
     }
}

int main(int argc, char **argv)
{
     pnpmat src, dst;
     int count;
     
     parse_args(argc, argv);
     src = pnpmat_read(fname, 3);
     count = pnpmat_count(src);

     TIME_START;
     for (int x=0; x < NTIMES; x++) {
          dst = pnpmat_guohall(src);
     }
     TIME_STOP;
     TIME_NORM_PRINT(NTIMES, count, src.size); NL;

     if (verbose) pnpmat_print(dst);
     if (outfile) pnpmat_write(dst, outfile);
     pnpmat_free(src);
     pnpmat_free(dst);
     return 0;
}
