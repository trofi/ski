/*
 * Copyright (C) 1995-2007, Hewlett-Packard Development Company, L.P.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or 
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, 
 * but WITHOUT ANY WARRANTY; without even the implied warranty of 
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the 
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 */

#include <sys/types.h>
#include <assert.h>
#include <errno.h>
#include <signal.h>
#include <string.h>

#include "std.h"
#include "types.h"
#include "sim.h"
#include "libsrs.h"
#include "state.h"

#include "freebsd/signal.h"

static struct {
	int	valid;
	int	flags;
	ADDR	handler;
	sigset_t mask;
	struct siginfo64 info;
} siginfo[_SIG_MAXSIG];

static sigset_t sigpend, sigmask;

void signal_return(void)
{
	assert(0);
}

void signal_invoke_handler(int in_syscall)
{
	int i, sig;
	uint32_t sigword;

	for (i = 0, sig = 0; i < _SIG_WORDS; i++, sig += 32) {
		sigword = sigpend.__bits[i] & ~sigmask.__bits[i];
		if (sigword)
			break;
	}
	assert(sigword);
	sig += ffs(sigword);
	assert(_SIG_VALID(sig));

	/* XXX need to actually deliver the signal. Ignore for now. */
	sigdelset(&sigpend, sig);
}

int signal_pending(void)
{
	int i;

	for (i = 0; i < _SIG_WORDS; i++) {
		if (sigpend.__bits[i] &&
		    sigpend.__bits[i] & ~sigmask.__bits[i])
			return (1);
	}
	return (0);
}

void signal_queue_info(int sig, ADDR ifa)
{
	struct siginfo64 *si64;

	/* Sanity checks. */
	if (!_SIG_VALID(sig))
		return;

	/* XXX We need to deliver the trap. For now, halt the program. */
	progStop("Got trap %d (IFA=0x%016llx, IP=0x%016llx)\n", sig, ifa, ip);

	sigaddset(&sigpend, sig);
	si64 = &siginfo[_SIG_IDX(sig)].info;
	memset(si64, 0, sizeof(*si64));
	si64->si_addr = ifa;
}

static void signal_handler(int sig, siginfo_t *si, void *uc)
{
	struct siginfo64 *si64;

	/* Sanity checks. */
	if (!_SIG_VALID(sig))
		return;

#if 0
	printf("Got signal %d\n", sig);
#endif

	sigaddset(&sigpend, sig);
	si64 = &siginfo[_SIG_IDX(sig)].info;
	si64->si_signo = sig;
	si64->si_errno = si->si_errno;
	si64->si_code = si->si_code;
	si64->si_pid = si->si_pid;
	si64->si_uid = si->si_uid;
	si64->si_status = si->si_status;
	si64->si_addr = (long)si->si_addr;
	si64->si_value.sigval_ptr = (long)si->si_value.sigval_ptr;
	si64->si_band = si->si_band;
}

int signal_get_handler(int sig, struct sigaction64 *sa64)
{
	struct sigaction sa;

	if (!_SIG_VALID(sig))
		return (EINVAL);

	if (!siginfo[_SIG_IDX(sig)].valid) {
		if (sigaction(sig, NULL, &sa) == -1)
			return (errno);
		siginfo[_SIG_IDX(sig)].valid = 1;
		siginfo[_SIG_IDX(sig)].handler = 0;
		siginfo[_SIG_IDX(sig)].flags = sa.sa_flags;
		siginfo[_SIG_IDX(sig)].mask = sa.sa_mask;
	}
	sa64->sa_handler = siginfo[_SIG_IDX(sig)].handler;
	sa64->sa_flags = siginfo[_SIG_IDX(sig)].flags;
	sa64->sa_mask = siginfo[_SIG_IDX(sig)].mask;
	return (0);
}

int signal_set_handler(int sig, struct sigaction64 *sa64)
{
	struct sigaction sa;

	if (!_SIG_VALID(sig))
		return (EINVAL);

	sa.sa_flags = sa64->sa_flags;
	sa.sa_mask = sa64->sa_mask;
	if (sa64->sa_handler == 0)
		sa.sa_handler = SIG_DFL;
	else if (sa64->sa_handler == 1)
		sa.sa_handler = SIG_IGN;
	else {
		sa.sa_sigaction = signal_handler;
		sa.sa_flags |= SA_SIGINFO;
	}
	if (sigaction(sig, &sa, NULL) == -1)
		return (errno);

	siginfo[_SIG_IDX(sig)].handler = sa64->sa_handler;
	siginfo[_SIG_IDX(sig)].flags = sa64->sa_flags;
	siginfo[_SIG_IDX(sig)].mask = sa64->sa_mask;
	siginfo[_SIG_IDX(sig)].valid = 1;
	return (0);
}
