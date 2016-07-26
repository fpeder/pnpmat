#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>

#include "auz.h"

#define NTIMES 10

const char usage[] = "usage: %s -i img -k size [-n iter] [-v] [-s out]\n";

int niter=1;
int ksize=0;
int verbose=0;
int save=0;
char *fname=NULL;
char *outfile=NULL;


void parse_args(int argc, char **argv)
{
     extern char *optarg; int c, err=0;
     while ((c = getopt(argc, argv, "i:k:n:v:s:")) != -1) {
          switch (c) {
          case 'i': fname = optarg; break;
          case 'k': ksize = atoi(optarg); break;
          case 'n': niter = atoi(optarg); break;
          case 'v': verbose = 1; break;
          case 's': outfile = optarg; break;
          case '?': err = 1; break;
          }
     }
     if (!fname || !ksize || err) {
          fprintf(stderr, usage, argv[0]);
          exit(-1);
     }
}

int main(int argc, char **argv)
{
     bmat src, dst, tmp;
     int count;

     parse_args(argc, argv);
     src = bmat_read(fname);
     count = bmat_count(src);

     TIME_START;
     for (int x=0; x < NTIMES; x++) {
          for (int i=0; i < niter; i++) {
               tmp = i ? dst : src;
               dst = bmat_erode(tmp, ksize);
          }
     }
     TIME_STOP;
     TIME_NORM_PRINT(NTIMES, count, src.size); NL;

     if (verbose) bmat_print(dst);
     if (outfile) bmat_write(dst, outfile);
     bmat_free(src);
     bmat_free(dst);
     return 0;
}
