/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 4 -*- */
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

#include "std.h"

#ifdef SKINET_ENABLE

#include "netdev.h"

#include <sys/types.h>
#include <asm/types.h>
#include <fcntl.h>
#include <sys/param.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <linux/filter.h>
#include <linux/if_packet.h>
#include <net/ethernet.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <net/if.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#include "std.h"
#include "bits.h"
#include "types.h"
#include "fields.h"
#include "state.h"
#include "ssc.h"
#include "sim.h"
#include "ui.h"


BOOL ski_nonet;			/* when true network is disabled, i.e. no interface is found */

#define ETH_DETACHED	0x0
#define ETH_ATTACHED	0x1

typedef struct _eth_dev_t
  {
    struct _eth_dev_t *eth_next;	/* linked list of devices */
    int eth_fd;			/* current file descriptor */
    int eth_flags;			/* attached|detached */
# if !HAVE_TUN_TAP
    char eth_name[IFNAMSIZ];		/* real name */
    unsigned int eth_ipaddr;		/* current ipaddress */
    struct sockaddr_ll eth_sa_ll;	/* save extra copies on send */
# endif
  }
eth_dev_t;

/*
 * list of currently detected devices
 */
static eth_dev_t *eth_list;

static inline eth_dev_t *
find_eth (int fd)
{
  eth_dev_t *p;

  for (p = eth_list; p; p = p->eth_next)
    {
      if (p->eth_fd == fd)
	return p;
    }
  return NULL;
}

# if HAVE_TUN_TAP

#include <poll.h>
#include <linux/if_tun.h>

/* init 1st two bytes to 1Ah 64h */
unsigned char macaddr_tmp[IFHWADDRLEN] = { 26, 100 };

int
netdev_open (char *name, unsigned char *macaddr)
{
  int fd;
  struct ifreq ifr;
  eth_dev_t *eth;

  if (ski_nonet == YES)
    return -1;

  fd = open ("/dev/net/tun", O_RDWR);
  if (fd == -1)
    {
      cmdwPrint ("netdev_open: open /dev/net/tun failed: %d\n", errno);
      return -1;
    }

  memset (&ifr, 0, sizeof ifr);
  ifr.ifr_flags = IFF_TAP | IFF_NO_PI;
  if (name)
    strncpy (ifr.ifr_name, name, IFNAMSIZ);

  if (ioctl (fd, TUNSETIFF, &ifr) < 0)
    {
      cmdwPrint ("netdev_open: ioctl TUNSETIFF failed: %d\n", errno);
      close (fd);
      return -1;
    }

  if (ioctl (fd, SIOCGIFHWADDR, &ifr) == -1)
    {
      cmdwPrint ("netdev_open: ioctl SIOCGIFHWADDR failed: %d\n", errno);
      close (fd);
      return -1;
    }

  memcpy (macaddr, (char *) ifr.ifr_hwaddr.sa_data, IFHWADDRLEN);
  memcpy (macaddr, macaddr_tmp, 2); /* overwrite first two bytes */

  eth = malloc (sizeof *eth);
  if (!eth)
    {
      cmdwPrint ("netdev_open: malloc failed: %d\n", errno);
      close (fd);
      return -1;
    }

  /*
   * the device is not yet attached
   */
  eth->eth_fd = fd;
  eth->eth_next = eth_list;
  eth->eth_flags = ETH_DETACHED;
  eth_list = eth;		/* we're at the head of the list now */

  return fd;
}

int
netdev_attach (int fd, unsigned int ipaddr)
{
  int r;
  eth_dev_t *eth = find_eth (fd);

  if (eth == NULL)
    progExit ("netdev_attach: can't find eth %d\n", fd);

  /*
   * prepare for SIGIO: set ownership + asynchronous notification
   */
  r = fcntl (fd, F_SETOWN, getpid ());
  if (r == -1)
    progExit ("netdev_attach: f_setown %d\n", errno);

  r = fcntl (fd, F_SETFL, fcntl (fd, F_GETFL, 0) | O_ASYNC | O_NONBLOCK);
  if (r == -1)
    progExit ("netdev_attach: f_setfl %d\n", errno);

  eth->eth_flags = ETH_ATTACHED;

  return 0;
}

int
netdev_detach (int fd)
{
  int r;
  eth_dev_t *eth = find_eth (fd);

  if (eth == NULL)
    progExit ("netdev_detach: can't find eth %d\n", fd);

  /*
   * don't notify
   *
   * should also remove ownership but it does not really matter
   */
  r = fcntl (fd, F_SETFL, fcntl (fd, F_GETFL, 0) & (~O_ASYNC | O_NONBLOCK));
  if (r == -1)
    progExit ("netdev_detach: f_setfl %d\n", errno);

  eth->eth_flags = ETH_DETACHED;

  return 0;
}

/*
 * read the next frame from interface
 */
long
netdev_recv (int fd, char *fbuf, int len)
{
  int r;

  if ((unsigned long) fbuf & 0x3)
    progExit ("receive frame not aligned");

  r = read (fd, fbuf, len);
  if (r == -1 && errno != EAGAIN)
    progExit ("netdev_recv: error on read %d\n", errno);

  return (long) (r > 0 ? r : 0);
}

/*
 * write next frame to interface
 */
long
netdev_send (int fd, char *fbuf, int len)
{
  int r;
  eth_dev_t *eth = find_eth (fd);

  if ((unsigned long) fbuf & 0x3)
    progExit ("trasmit frame not aligned");

  if (eth == NULL)
    progExit ("netdev_send: can't find %d\n", fd);

  r = write (fd, fbuf, len);
  if (r == -1)
    progExit ("netdev_send: error on write %d\n", errno);

  return (long) len;
}

static int
netdev_can_read (int fd)
{
  struct pollfd pfd;
  int r;

  pfd.fd = fd;
  pfd.events = POLLIN;
  pfd.revents = 0;
  r = poll (&pfd, 1, 0);
  if (r == -1 || r == 0)
      return r;
  return pfd.revents & POLLIN;
}

# else /* !HAVE_TUN_TAP */

#define MAX_FRAME_SIZE	1536	/* should be more than enough for ethernet */

#define MAX_FD		256

/*
 * thanks to tcpdump for the program dump
 *
 * /usr/sbin/tcpdump -dd dst xx.xx.xx.xx or ether broadcast
 * Basically we accept:
 *      - Ethernet broadcasts, like ARP requests
 *      - any packets whose destination is xx.xx.xx.xx (could be an ARP reply).
 *
 * xx.xx.xx.xx is going to be patched on the fly
 *
 * We must make sure that the return value is the largest value possible
 * to avoid dropping extra frames.
 */

#define BPF_COOKIE	0xffffffff
#define BPF_INS1 3		/* 1st instruction to patch for IP address to filter */
#define BPF_INS2 6		/* 2nd .... */

static struct sock_filter insns[] =
{
  /* A=P[12:2] =ether_type */
  BPF_STMT (BPF_LD + BPF_H + BPF_ABS, 12),

  /* A==IP ? fall through  : check ARP */
  BPF_JUMP (BPF_JMP + BPF_JEQ + BPF_K, ETHERTYPE_IP, 0, 2),

  /* A=ip_dst */
  BPF_STMT (BPF_LD + BPF_W + BPF_ABS, 30),

  /* A==our IP ? success:fail */
  BPF_JUMP (BPF_JMP + BPF_JEQ + BPF_K, BPF_COOKIE, 8, 7),

  /* A==ARP ? fall through : fail */
  BPF_JUMP (BPF_JMP + BPF_JEQ + BPF_K, ETHERTYPE_ARP, 0, 6),

  /* A=ARP_TPA */
  BPF_STMT (BPF_LD + BPF_W + BPF_ABS, 38),

  /* A=our IP ? success : fall through */
  BPF_JUMP (BPF_JMP + BPF_JEQ + BPF_K, BPF_COOKIE, 5, 0),

  /* A=4 bytes of ether_dst */
  BPF_STMT (BPF_LD + BPF_W + BPF_ABS, 0),

  /* A==4bytes worth of broadcast ? fall through : fail */
  BPF_JUMP (BPF_JMP + BPF_JEQ + BPF_K, 0xffffffff, 0, 2),

  /* A=2 last bytes of ethr_dst */
  BPF_STMT (BPF_LD + BPF_H + BPF_ABS, 4),

  /* A==2bytes worth of broadcast ?  sucess: fall through */
  BPF_JUMP (BPF_JMP + BPF_JEQ + BPF_K, 0xffff, 1, 0),

  /* return failure (0byte) */
  BPF_STMT (BPF_RET + BPF_K, 0),

  /* return success (maximum size) */
  BPF_STMT (BPF_RET + BPF_K, 0xffffffff)
};

/*
 * taken and adapted from
 * https://github.com/torvalds/linux/blob/v3.10/Documentation/networking/ifenslave.c
 */
static
int
set_if_up (char *ifname)
{
  int skfd = -1; /* AF_INET socket for ioctl() calls.*/
  int saved_errno;

  struct ifreq ifr;
  int res = 0;

  ifr.ifr_flags = IFF_UP;
  strncpy(ifr.ifr_name, ifname, IFNAMSIZ);

  /* Open a basic socket */
  if ((skfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
    perror("set_if_up: socket() failed\n");
    res = 1;
    goto out;
  }

  res = ioctl(skfd, SIOCSIFFLAGS, &ifr);
  if (res < 0) {
    saved_errno = errno;
    cmdwPrint("Host interface '%s': Error: SIOCSIFFLAGS failed: %s\n",
            ifname, strerror(saved_errno));
  } else {
    cmdwPrint("Host interface '%s': flags set to %04X.\n", ifname, IFF_UP);
  }

  out:
  if (skfd >= 0) {
    close(skfd);
  }

  return res;
}

int
netdev_open (char *name, unsigned char *macaddr)
{
  struct sockaddr_ll sa_ll = {
    .sll_family = AF_PACKET,
    .sll_protocol = htons(ETH_P_ALL),
    .sll_ifindex = if_nametoindex(name),
  };
  struct sockaddr_in *sin;
  struct ifreq ifr;
  eth_dev_t *eth;
  int r;
  int fd;

  if (ski_nonet == YES)
    return -1;

  /*
   * Open a RAW/PACKET socket (just like tcpdump)
   *
   * See 'man 7 packet' on Linux.
   */
  fd = socket (AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
  if (fd == -1)
    {
      if (errno == EPERM)
		cmdwPrint("No network support: must be run as root\n");
      else
		cmdwPrint("No network support: make sure SOCK_PACKET is configured\n");
      return -1;
    }

  /*
   * Make sure the selected interface is up, before using it.
   */
  set_if_up (name);

  r = bind (fd, (struct sockaddr *)&sa_ll, sizeof (sa_ll));
  if (r) {
    cmdwPrint("Cannot bind (disabling network): make sure %s exists\n", name);
    close(fd);
    return -1;
  }

  memset (&ifr, 0, sizeof (ifr));

  sin = (struct sockaddr_in *) &ifr.ifr_addr;
  strncpy (ifr.ifr_name, name, IFNAMSIZ);


  eth = (eth_dev_t *) malloc (sizeof (eth_dev_t));
  if (eth == NULL)
    progExit ("netdev_open: can't allocate eth_dev_t\n");



  strncpy (eth->eth_name, name, IFNAMSIZ);

  /*
   * the device is not yet attached
   */
  eth->eth_fd = fd;
  eth->eth_ipaddr = 0;
  eth->eth_next = eth_list;
  eth->eth_flags = ETH_DETACHED;
  eth_list = eth;		/* we're at the head of the list now */

  eth->eth_sa_ll = sa_ll;

  r = ioctl (fd, SIOCGIFHWADDR, &ifr);
  if (r)
    progExit ("netdev_open: can't get HW addr\n");

  memcpy (macaddr, (char *) ifr.ifr_hwaddr.sa_data, IFHWADDRLEN);

  /*
   * The device is still not in promiscuous mode
   */
  return fd;

}

static inline
void
filter_patch (struct sock_filter *filter, unsigned int ipaddr)
{
  filter[BPF_INS1].k = ipaddr;
  filter[BPF_INS2].k = ipaddr;
}


/*
 * Put the device in promiscuous mode
 * and enable packet filtering on the IP address
 */
int
netdev_attach (int fd, unsigned int ipaddr)
{
  int r;
  struct ifreq ifr;
  struct sock_fprog prg;
  eth_dev_t *eth = find_eth (fd);

  if (eth == NULL)
    progExit ("netdev_attach: can't find eth %d\n", fd);

  /*
   * prepare for SIGIO: set ownership + asynchronous notification
   */
  r = fcntl (fd, F_SETOWN, getpid ());
  if (r == -1)
    progExit ("netdev_attach: f_setown %d\n", errno);

  r = fcntl (fd, F_SETFL, fcntl (fd, F_GETFL, 0) | O_ASYNC | O_NONBLOCK);
  if (r == -1)
    progExit ("netdev_attach: f_setfl %d\n", errno);

  /* attach BPF filter */

  filter_patch (insns, ipaddr);

  prg.filter = insns;
  prg.len = sizeof (insns) / sizeof (struct sock_filter);

  r = setsockopt (fd, SOL_SOCKET, SO_ATTACH_FILTER, &prg, sizeof (prg));
  if (r)
    progExit ("netdev_attach: can't attach filter %d\n", errno);

  memset (&ifr, 0, sizeof (ifr));

  strncpy (ifr.ifr_name, eth->eth_name, IFNAMSIZ);

  r = ioctl (fd, SIOCGIFFLAGS, &ifr);
  if (r)
    progExit ("netdev_attach: ioctl getflags");

  ifr.ifr_flags |= IFF_PROMISC;

  /*
   * promiscuous mode enabled now
   */
  r = ioctl (fd, SIOCSIFFLAGS, &ifr);
  if (r)
    progExit ("netdev_attach: ioctl set promisc");

  eth->eth_flags = ETH_ATTACHED;

  return 0;
}

/*
 * remove filter & promiscuous mode
 */
int
netdev_detach (int fd)
{
  int r;
  int dummy = 0;
  struct ifreq ifr;
  eth_dev_t *eth = find_eth (fd);

  if (eth == NULL)
    progExit ("netdev_detach: can't find eth %d\n", fd);

  /*
   * don't notify
   *
   * should also remove ownership but it does not really matter
   */
  r = fcntl (fd, F_SETFL, fcntl (fd, F_GETFL, 0) & (~O_ASYNC | O_NONBLOCK));
  if (r == -1)
    progExit ("netdev_detach: f_setfl %d\n", errno);

  /*
   * remove filter
   */
  r = setsockopt (fd, SOL_SOCKET, SO_DETACH_FILTER, &dummy, sizeof (dummy));
  if (r)
    progExit ("netdev_detach: can't detach filter %d\n", errno);

  memset (&ifr, 0, sizeof (ifr));

  strncpy (ifr.ifr_name, eth->eth_name, IFNAMSIZ);

  r = ioctl (fd, SIOCGIFFLAGS, &ifr);
  if (r)
    progExit ("netdev_detach: ioctl getflags");

  ifr.ifr_flags &= ~IFF_PROMISC;

  /*
   * not promiscuous
   */
  r = ioctl (fd, SIOCSIFFLAGS, &ifr);
  if (r)
    progExit ("netdev_detach: ioctl unset promisc");

  eth->eth_flags = ETH_DETACHED;

  return 0;
}

/*
 * read the next frame from interface
 */
long
netdev_recv (int fd, char *fbuf, int len)
{
  int r;

  if ((unsigned long) fbuf & 0x3)
    progExit ("receive frame not aligned");

  r = recv (fd, fbuf, len, 0);
  if (r == -1 && errno != EAGAIN)
    progExit ("netdev_recv: error on recv %d\n", errno);

  return (long) (r > 0 ? r : 0);
}


/*
 * write next frame to interface
 */
long
netdev_send (int fd, char *fbuf, int len)
{
  int r;
  struct sockaddr_ll sa_ll;
  eth_dev_t *eth = find_eth (fd);

  if ((unsigned long) fbuf & 0x3)
    progExit ("transmit frame not aligned");

  if (eth == NULL)
    progExit ("netdev_send: can't find %d\n", fd);

  r = sendto (fd, fbuf, len, 0, (struct sockaddr *)&eth->eth_sa_ll, sizeof (sa_ll));
  if (r == -1)
    progExit ("netdev_send: error on sendto %d\n", errno);

  return (long) len;
}

static int
netdev_can_read (int fd)
{
  char c;

  /*
   * We can't use FIONREAD here because it's only supported
   * by TCP sockets.
   */
  return recv (fd, &c, 1, MSG_PEEK);
}

# endif /* !HAVE_TUN_TAP */

/*
 * check if SIGIO was because of network I/O
 * the other possible reason is keyboard input
 *
 * This is kind of heavy weight but we need to demultiplex somewhow !
 * maybe real time signals ?
 *
 */
int
isnetio (void)
{
  int r;
  eth_dev_t *p;

  if (eth_list == NULL)
    return 0;

  for (p = eth_list; p; p = p->eth_next)
    {

      /* could use 2 lists instead */
      if (p->eth_flags != ETH_ATTACHED)
	continue;

      r = netdev_can_read (p->eth_fd);
      if (r > 0)
	return 1;

      if (r == -1 && errno != EAGAIN)
	progExit ("isnetio: error on recv %d\n", errno);
    }
  return 0;
}

/*
 * Try to cleanup the devices: at least remove promiscuous mode
 */
void
netdev_cleanup (void)
{
  eth_dev_t *p, *next;

  for (p = eth_list; p; p = next)
    {

      next = p->eth_next;

      /* could use 2 lists instead */
      if (p->eth_flags == ETH_ATTACHED)
	netdev_detach (p->eth_fd);

      close (p->eth_fd);

      free (p);

    }
}
#endif /* SKINET_ENABLE */
