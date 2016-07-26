#include <stdio.h>
#include <unistd.h>
#include "auz.h"

#define NTIMES 10

const char *usage = "usage: %s -i img -k size [-n iter] [-s out] [-v]\n";
int niter=1, ksize=0, verbose=0;
char *fname=NULL, *outfile=NULL;

void parse_args(int argc, char **argv)
{
     extern char *optarg;
     int c, err=0;
     while ((c = getopt(argc, argv, "i:k:n:s:v")) != -1) {
          switch (c) {
          case 'i': fname = optarg; break;
          case 'k': ksize = atoi(optarg); break;
          case 'n': niter = atoi(optarg); break;
          case 's': outfile = optarg; break;
          case 'v': verbose = 1; break;
          case '?': err = 1; break;
          }
     }
     if (!fname || !ksize || err) {
          fprintf(stderr, usage, argv[0]);
          exit(-1);
     }
}

int main (int argc, char **argv)
{
     pnpmat src, dst, tmp;
     int count;
     parse_args(argc, argv);
     src = pnpmat_read(fname, ksize);
     count = pnpmat_count(src);
     TIME_START;
     for (int x = 0; x < NTIMES; x++) {
          for (int i = 0; i < niter; i++) {
               tmp = i ? dst : src;
               dst = pnpmat_erode(tmp, ksize);
          }
     }
     TIME_STOP;
     TIME_NORM_PRINT(NTIMES, count, src.M*src.N); NL;

     if (verbose) pnpmat_print(dst);
     if (outfile) pnpmat_write(dst, outfile);
     pnpmat_free(src);
     pnpmat_free(dst);
     return 0;
}



