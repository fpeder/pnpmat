#include <stdio.h>

#include "time.h"

void time_start()
{
     gettimeofday(&ts, NULL);
}

void time_stop()
{
     gettimeofday(&te, NULL);
}

long elapsed_time()
{
     long sec, microsec;
     
     sec = te.tv_sec - ts.tv_sec;
     microsec = sec*1000000 + te.tv_usec - ts.tv_usec;
     return microsec;
}

void print_elapsed_time(int n)
{
     printf("%.2f ", ((double)elapsed_time()) / n);
}

void print_elapsed_time_norm(int n, int count, int sz)
{
     double t;

     t = (double)elapsed_time() / n;
     t = t / (double)sz * (double)count;
     printf("%.2f ", t);
}
