#ifndef _SKI_ITC_H
#define _SKI_ITC_H

void itc_write(unsigned long long);
unsigned long long itc_read(void);
void itm_write(unsigned long long);
void itc_untick(int);

extern char itc_itimer_fired;

static __inline__ int itc_tick_overflow(void)
{
    if (!itc_itimer_fired)
	return 0;
    itc_itimer_fired = 0;
    return 1;
}

#endif	/* _SKI_ITC_H */
