#ifndef TIME_H
#define TIME_H

#include <sys/time.h>

#define TIME_START                 time_start()
#define TIME_STOP                  time_stop()
#define TIME_PRINT(n)              print_elapsed_time(n)
#define TIME_NORM_PRINT(n, c, s)   print_elapsed_time_norm(n, c, s)

struct timeval ts, te;

void time_start();
void time_stop();
long elapsed_time();
void print_elapsed_time(int n);
void print_elapsed_time_norm(int n, int count, int sz);

#endif /* TIME_H */



