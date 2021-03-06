#include <stdio.h>
#include <unistd.h>

#include "auz.h"

const char usage[] = "usage: %s -i img [-s out] [-v]\n";
int verbose=0;
char *fname=NULL, *outfile=NULL;

void parse_args(int argc, char **argv)
{
     extern char *optarg; int c, err=0;
     while ((c = getopt(argc, argv, "i:s:v")) != -1) {
          switch (c) {
          case 'i': fname = optarg; break;
          case 'v': verbose = 1; break;
          case 's': outfile = optarg; break;
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
     bmat src; int count=0;
     uint32_t contour[CNTR_MAX_PT];

     parse_args(argc, argv);
     src = bmat_read(fname);

     TIME_START;
     bmat_moore(src, &count, contour);
     TIME_STOP;
     TIME_NORM_PRINT(1, count, src.size); NL;

     if (verbose) cntr_print(contour); NL;
     if (outfile) cntr_write(contour, outfile);
     bmat_free(src);
     return 0;
}
