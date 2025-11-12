/*
 * Simulates a 2.0 MHz ITC on top of setitimer(ITIMER_REAL) and gettimeofday().
 *
 * The 2.0 MHz frequency is chosen because it matches the Linux kernel's default
 * for CONFIG_IA64_HP_SIM=y: SAL_FREQ_BASE_PLATFORM=200MHz, itc_ratio=1/100.
 */

#include <sys/time.h>
#include <limits.h>
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "itc.h"

static unsigned long long itm;
static unsigned long long itc;
static struct timeval tv;
static int itimer_active;
char itc_itimer_fired;

static void onalrm(int sig)
{
    itc_itimer_fired = 1;
}

static void itimer_stop(void)
{
    if (itimer_active) {
	struct itimerval itv;

	memset(&itv, 0, sizeof itv);
	setitimer(ITIMER_REAL, &itv, NULL);
	itimer_active = 0;
    }
}

/*
 * PRE:
 * - !itimer_active
 * - tv and itc in sync
 */
static void itimer_start(void)
{
    unsigned long long ticks, usecs, seconds;
    struct itimerval itv;
    static int catch_sigalrm_done;

    itc_itimer_fired = 0;

    ticks = itm - itc;

    usecs = ticks / 2ULL;
    if ((ticks % 2ULL) != 0)
	++usecs;

    itv.it_value.tv_usec = usecs % 1000000ULL;
    seconds = usecs / 1000000ULL;
    itv.it_value.tv_sec =
	(seconds > (unsigned long long)LONG_MAX)
	? LONG_MAX
	: (long)seconds;

    if (itv.it_value.tv_sec == 0 && itv.it_value.tv_usec == 0) {
	itc_itimer_fired = 1;
	return;
    }

    itv.it_interval.tv_sec = 0;
    itv.it_interval.tv_usec = 0;

    if (!catch_sigalrm_done) {
	struct sigaction act;

	catch_sigalrm_done = 1;

	memset(&act, 0, sizeof act);
	act.sa_handler = onalrm;

	if (sigaction(SIGALRM, &act, NULL) == -1) {
	    perror("sigaction");
	    exit(1);
	}
    }

    itimer_active = 1;
    if (setitimer(ITIMER_REAL, &itv, NULL) == -1) {
	perror("setitimer");
	exit(1);
    }
}

static void tv_update(void)
{
    if (gettimeofday(&tv, NULL) == -1) {
	perror("gettimeofday");
	exit(1);
    }
}

static unsigned long long tv_update_ticks(void)
{
    struct timeval prev_tv;
    unsigned long long usecs;

    prev_tv = tv;

    tv_update();

    if (prev_tv.tv_sec == 0 && prev_tv.tv_usec == 0)
	return 1;	/* the ITC always increments.. */

    usecs = ((tv.tv_sec - prev_tv.tv_sec) * 1000000LL
	     + tv.tv_usec
	     - prev_tv.tv_usec);

    if (usecs == 0)
	return 1;	/* the ITC always increments.. */

    return usecs * 2;
}

static void itc_update(void)
{
    itc += tv_update_ticks();
}

unsigned long long itc_read(void)
{
    itc_update();
    return itc;
}

void itc_write(unsigned long long val)
{
    itimer_stop();
    tv_update();
    itc = val;
    itimer_start();
}

void itm_write(unsigned long long val)
{
    itimer_stop();
    itc_update();
    itm = val;
    itimer_start();
}

void itc_untick(int flag)
{
    if (flag)
	itc_itimer_fired = 1;
}
