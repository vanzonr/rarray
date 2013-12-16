/* 
   cstopwatch.h 
   Header file for the timing functions in C 
   rvz - 27 april 2010
*/

#ifndef _CSTOPWATCHH_
#define _CSTOPWATCHH_

#include <time.h>
#include <sys/time.h>
#include <sys/times.h>
#include <stdio.h>

#ifndef CLK_TCK
#include <unistd.h>
#define MY_CLK_TCK sysconf(_SC_CLK_TCK)
#else
#define MY_CLK_TCK CLK_TCK
#endif

struct Stopwatch
{
      char   prefix;
      double used;
      double elapsed;
};
typedef struct Stopwatch Stopwatch;

static double sw_utime();
static double sw_etime();
static void sw_write(double);

#define START { '#', -sw_utime(), -sw_etime() }

void stopwatchStart(Stopwatch *sw) 
{
   Stopwatch s = START;
   *sw = s;
}

void stopwatchStop(Stopwatch *sw)
{	
   sw->used    += sw_utime();
   sw->elapsed += sw_etime();
   fprintf(stderr, "%c", sw->prefix);
   sw_write(sw->elapsed); 
   fprintf(stderr, " elapsed, ");
   sw_write(sw->used);
   fprintf(stderr, " useful");
   
   if (sw->elapsed > 0.0)  
      fprintf(stderr, " (%4.1f%%)", 100*sw->used/sw->elapsed);
   
   fprintf(stderr,".\n");

   sw->used    = -sw_utime();
   sw->elapsed = -sw_etime();

}

double sw_utime() 
{  
   struct tms buf;
   times(&buf);
   return buf.tms_utime/(double)(MY_CLK_TCK);
}

double sw_etime() 
{
   struct timeval t;
   gettimeofday(&t, 0);
   return t.tv_sec + 0.000001 * t.tv_usec;
}

void sw_write(double time)
{
   int continu = 0;
   int recapsec = 0;
   unsigned long long itime = (unsigned long long int)(time*1000000);
   
   if (itime>=1000) 
   {
      if (itime > 86400000000ULL) 
      {
         fprintf(stderr, "%llud:", itime/86400000000ULL);
         itime %= 86400000000ULL;
         continu = 1;
         recapsec = 1;
      }
      if (itime>3600000000ULL) 
      {
         fprintf(stderr, "%lludh:", itime/3600000000ULL);
         itime %= 3600000000ULL;
         continu = 1;
         recapsec = 1;
      } 
      if (itime>60000000ULL) 
      {
         fprintf(stderr, "%llum:", itime/60000000ULL);
         itime %= 60000000ULL;
         continu = 1;
         recapsec = 1;
      }
      fprintf(stderr, "%llu.", itime/1000000);
      itime %= 1000000; 
      if (itime>1000 || continu==1) 
      {
         fprintf(stderr, "%03llus", itime/1000);
         itime %= 1000;
      }
   } else {
      fprintf(stderr, "%llu microsec.", itime);
   }
   if (recapsec == 1)
      fprintf(stderr, " (%ds)", (int)time);
}

#undef MY_CLK_TCK
#endif
