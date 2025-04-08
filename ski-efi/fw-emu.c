// SPDX-License-Identifier: GPL-2.0
/*
 * PAL & SAL emulation.
 *
 * Copyright (C) 1998-2001 Hewlett-Packard Co
 *	David Mosberger-Tang <davidm@hpl.hp.com>
 */

#include "fw-emu.h"

#include "bootloader.h"
#include "memstr.h"
#include "ssc.h"

#define MB	(1024*1024UL)


char fake_image_handle;
efi_loaded_image_t *efi_loaded_image_protocol;

static char fw_mem[(  sizeof(efi_system_table_t)
		    + sizeof(efi_runtime_services_t)
		    + sizeof(efi_boot_services_t)
		    + sizeof(efi_loaded_image_t)
  	            + sizeof(efi_simple_text_output_interface_t)
		    + sizeof(efi_simple_input_interface_t)
		    + 1*sizeof(efi_config_table_t)
		    + sizeof(struct ia64_sal_systab)
		    + sizeof(struct ia64_sal_desc_entry_point)
		    + 1024)] __attribute__ ((aligned (8)));

#define SECS_PER_HOUR   (60 * 60)
#define SECS_PER_DAY    (SECS_PER_HOUR * 24)

/* Compute the `struct tm' representation of *T,
   offset OFFSET seconds east of UTC,
   and store year, yday, mon, mday, wday, hour, min, sec into *TP.
   Return nonzero if successful.  */
int
offtime (unsigned long t, efi_time_t *tp)
{
	const unsigned short int __mon_yday[2][13] =
	{
		/* Normal years.  */
		{ 0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334, 365 },
		/* Leap years.  */
		{ 0, 31, 60, 91, 121, 152, 182, 213, 244, 274, 305, 335, 366 }
	};
	long int days, rem, y;
	const unsigned short int *ip;

	days = t / SECS_PER_DAY;
	rem = t % SECS_PER_DAY;
	while (rem < 0) {
		rem += SECS_PER_DAY;
		--days;
	}
	while (rem >= SECS_PER_DAY) {
		rem -= SECS_PER_DAY;
		++days;
	}
	tp->hour = rem / SECS_PER_HOUR;
	rem %= SECS_PER_HOUR;
	tp->minute = rem / 60;
	tp->second = rem % 60;
	/* January 1, 1970 was a Thursday.  */
	y = 1970;

#	define DIV(a, b) ((a) / (b) - ((a) % (b) < 0))
#	define LEAPS_THRU_END_OF(y) (DIV (y, 4) - DIV (y, 100) + DIV (y, 400))
#	define __isleap(year) \
	  ((year) % 4 == 0 && ((year) % 100 != 0 || (year) % 400 == 0))

	while (days < 0 || days >= (__isleap (y) ? 366 : 365)) {
		/* Guess a corrected year, assuming 365 days per year.  */
		long int yg = y + days / 365 - (days % 365 < 0);

		/* Adjust DAYS and Y to match the guessed year.  */
		days -= ((yg - y) * 365 + LEAPS_THRU_END_OF (yg - 1)
			 - LEAPS_THRU_END_OF (y - 1));
		y = yg;
	}
	tp->year = y;
	ip = __mon_yday[__isleap(y)];
	for (y = 11; days < (long int) ip[y]; --y)
		continue;
	days -= ip[y];
	tp->month = y + 1;
	tp->day = days + 1;
	return 1;
}

extern void pal_emulator_static (void);

/* Macro to emulate SAL call using legacy IN and OUT calls to CF8, CFC etc.. */

#define BUILD_CMD(addr)		((0x80000000 | (addr)) & ~3)

#define REG_OFFSET(addr)	(0x00000000000000FF & (addr))
#define DEVICE_FUNCTION(addr)	(0x000000000000FF00 & (addr))
#define BUS_NUMBER(addr)	(0x0000000000FF0000 & (addr))

static efi_status_t
fw_efi_get_time (efi_time_t *tm, efi_time_cap_t *tc)
{
	struct {
		int tv_sec;	/* must be 32bits to work */
		int tv_usec;
	} tv32bits;

	ssc((unsigned long) &tv32bits, 0, 0, 0, SSC_GET_TOD);

	memset(tm, 0, sizeof(*tm));
	offtime(tv32bits.tv_sec, tm);

	if (tc)
		memset(tc, 0, sizeof(*tc));
	return EFI_SUCCESS;
}

static void
efi_reset_system (int reset_type, efi_status_t status, unsigned long data_size, efi_char16_t *data)
{
	ssc(status, 0, 0, 0, SSC_EXIT);
}

void
cons_write (const char *buf);

static efi_status_t
efi_output_string (struct efi_simple_text_output_interface *this,
		   unsigned short *string)
{
  char buf[5];
  for (int i = 0; string[i]; i++) {
    unsigned int code = string[i];
    // TODO: surrogates
    if (code <= 0x7f)
      ssc(code, 0, 0, 0, SSC_PUTCHAR);
    else if (code <= 0x07FF)
      {
	ssc((code >> 6) | 0xC0, 0, 0, 0, SSC_PUTCHAR);
	ssc((code & 0x3F) | 0x80, 0, 0, 0, SSC_PUTCHAR);
      }
    else
      {
	ssc((code >> 12) | 0xE0, 0, 0, 0, SSC_PUTCHAR);
	ssc(((code >> 6) & 0x3F) | 0x80, 0, 0, 0, SSC_PUTCHAR);
	ssc((code & 0x3F) | 0x80, 0, 0, 0, SSC_PUTCHAR);
      }
  }

  return EFI_SUCCESS;
}

static efi_status_t
efi_read_key_stroke(struct efi_simple_input_interface *this,
		    efi_input_key_t *key)
{
  char c = ssc(0, 0, 0, 0, SSC_GETCHAR);
  if (c)
    {
      key->unicode_char = c;
      key->scan_code = 0;
      return EFI_SUCCESS;
    }
  return EFI_NOT_READY;
}


static efi_status_t
efi_unimplemented (void)
{
  //  cons_write("unsupported\n");
	return EFI_UNSUPPORTED;
}

static efi_status_t
efi_success_stub (void)
{
	return EFI_SUCCESS;
}

static struct sal_ret_values
sal_emulator (long index, unsigned long in1, unsigned long in2,
	      unsigned long in3, unsigned long in4, unsigned long in5,
	      unsigned long in6, unsigned long in7)
{
	long r9  = 0;
	long r10 = 0;
	long r11 = 0;
	long status;

	/*
	 * Don't do a "switch" here since that gives us code that
	 * isn't self-relocatable.
	 */
	status = 0;
	if (index == SAL_FREQ_BASE) {
		if (in1 == SAL_FREQ_BASE_PLATFORM)
			r9 = 200000000;
		else if (in1 == SAL_FREQ_BASE_INTERVAL_TIMER) {
			/*
			 * Is this supposed to be the cr.itc frequency
			 * or something platform specific?  The SAL
			 * doc ain't exactly clear on this...
			 */
			r9 = 700000000;
		} else if (in1 == SAL_FREQ_BASE_REALTIME_CLOCK)
			r9 = 1;
		else
			status = -1;
	} else if (index == SAL_SET_VECTORS) {
		;
	} else if (index == SAL_GET_STATE_INFO) {
		;
	} else if (index == SAL_GET_STATE_INFO_SIZE) {
		;
	} else if (index == SAL_CLEAR_STATE_INFO) {
		;
	} else if (index == SAL_MC_RENDEZ) {
		;
	} else if (index == SAL_MC_SET_PARAMS) {
		;
	} else if (index == SAL_CACHE_FLUSH) {
		;
	} else if (index == SAL_CACHE_INIT) {
		;
#ifdef CONFIG_PCI
	} else if (index == SAL_PCI_CONFIG_READ) {
		/*
		 * in1 contains the PCI configuration address and in2
		 * the size of the read.  The value that is read is
		 * returned via the general register r9.
		 */
                outl(BUILD_CMD(in1), 0xCF8);
                if (in2 == 1)                           /* Reading byte  */
                        r9 = inb(0xCFC + ((REG_OFFSET(in1) & 3)));
                else if (in2 == 2)                      /* Reading word  */
                        r9 = inw(0xCFC + ((REG_OFFSET(in1) & 2)));
                else                                    /* Reading dword */
                        r9 = inl(0xCFC);
                status = PCIBIOS_SUCCESSFUL;
	} else if (index == SAL_PCI_CONFIG_WRITE) {
	      	/*
		 * in1 contains the PCI configuration address, in2 the
		 * size of the write, and in3 the actual value to be
		 * written out.
		 */
                outl(BUILD_CMD(in1), 0xCF8);
                if (in2 == 1)                           /* Writing byte  */
                        outb(in3, 0xCFC + ((REG_OFFSET(in1) & 3)));
                else if (in2 == 2)                      /* Writing word  */
                        outw(in3, 0xCFC + ((REG_OFFSET(in1) & 2)));
                else                                    /* Writing dword */
                        outl(in3, 0xCFC);
                status = PCIBIOS_SUCCESSFUL;
#endif /* CONFIG_PCI */
	} else if (index == SAL_UPDATE_PAL) {
		;
	} else {
		status = -1;
	}
	return ((struct sal_ret_values) {status, r9, r10, r11});
}

static unsigned char memory_map[MB*2];
static unsigned long map_key;

void
mark_memory(unsigned long start, unsigned long end, unsigned char type)
{
  unsigned long start_page = ((unsigned long)start) >> 12;
  unsigned long end_page = ((unsigned long)end + 0xfff) >> 12;
  unsigned long i;
  for (i = start_page; i < end_page && i < sizeof(memory_map); i++)
    memory_map[i] = type;
  map_key++;
}

extern unsigned char _start, _end;

void init_memory_map(void)
{
  mark_memory(0*MB, 2048*MB, EFI_CONVENTIONAL_MEMORY);
  mark_memory((long) &_start, (long) &_end, EFI_PAL_CODE);
  mark_memory(4096*MB, 6144*MB, EFI_CONVENTIONAL_MEMORY);
}


efi_status_t
efi_allocate_pages(int type, int memory_type, unsigned long pages,
		   efi_physical_addr_t *memory)
{
  switch(type)
    {
    case 0: // Any pages
      *memory = sizeof(memory_map) << 12;
    case 1: {// max address
      unsigned long found_pages = 0;
      for (long i = *memory >> 12; i >= 0; i--)
	{
	  if (memory_map[i] == EFI_CONVENTIONAL_MEMORY)
	    found_pages++;
	  else
	    found_pages = 0;
	  if (found_pages >= pages)
	    {
	      *memory = i << 12;
	      mark_memory(i << 12, (i + pages) << 12, memory_type);
	      return EFI_SUCCESS;
	      }
	}
      return EFI_OUT_OF_RESOURCES;
    }
    case 2: {// Address
        unsigned long start_page = ((unsigned long)*memory) >> 12;
	unsigned long end_page = start_page + pages;
	unsigned long i;
	if (end_page > sizeof(memory_map))
	  return EFI_OUT_OF_RESOURCES;
	for (i = start_page; i < end_page; i++)
	  if (memory_map[i] != EFI_CONVENTIONAL_MEMORY)
	    return EFI_OUT_OF_RESOURCES;
	mark_memory(start_page << 12, end_page << 12, memory_type);
	return EFI_SUCCESS;
    }
    default:
      cons_write("unsupported allocation type");
      return EFI_UNSUPPORTED;  
    }
}

static unsigned long count_regions(void)
{
  unsigned long ret = 0;
  for (unsigned long i = 0; i < sizeof(memory_map); i++)
    {
      if (memory_map[i] && (i == 0 || memory_map[i-1] != memory_map[i]))
	ret++;
    }
  return ret;
}

efi_status_t
efi_get_memory_map(unsigned long *memory_map_size, void *memory_map_ptr, unsigned long *map_key_ptr,
		   unsigned long *descriptor_size, u32 *descriptor_version)
{
  efi_memory_desc_t *descs = memory_map_ptr;
  *descriptor_size = sizeof(efi_memory_desc_t);
  *descriptor_version = 1;
  *map_key_ptr = map_key;

  unsigned long num_regions = count_regions();
  if (num_regions * sizeof(efi_memory_desc_t) > *memory_map_size)
    {
      *memory_map_size = num_regions * sizeof(efi_memory_desc_t);
      return EFI_BUFFER_TOO_SMALL;
    }
  *memory_map_size = num_regions * sizeof(efi_memory_desc_t);
  unsigned long outcnt = 0;
  for (unsigned long i = 0; i < sizeof(memory_map); i++)
    {
      if (memory_map[i] && (i == 0 || memory_map[i-1] != memory_map[i]))
	{
	  unsigned long j;
	  for (j = i; j < sizeof(memory_map); j++)
	    if (memory_map[j] != memory_map[i])
	      break;
	  descs[outcnt].type = memory_map[i];
	  descs[outcnt].pad = 0;
	  descs[outcnt].phys_addr = i << 12;
	  descs[outcnt].virt_addr = i << 12;
	  descs[outcnt].num_pages = j - i;
	  descs[outcnt].attribute = EFI_MEMORY_WB;
	  i = j - 1;
	  outcnt++;
	}
    }
  return EFI_SUCCESS;
}

int
memcmp(void *ai, void *bi, unsigned long sz)
{
  char *a = ai;
  char *b = bi;
  for (unsigned long i = 0; i < sz; i++)
    if (a[i] != b[i])
      {
	return (unsigned char)a[i] - (unsigned char)b[i];
      }
  return 0;
}

efi_status_t
efi_open_protocol(efi_handle_t handle,
		  efi_guid_t *protocol,
		  void **protocol_interface,
		  efi_handle_t agent_handle,
		  efi_handle_t controller_handle,
		  unsigned int attributes)
{
  efi_guid_t loaded_image = EFI_LOADED_IMAGE_GUID;
  if (memcmp(protocol, &loaded_image, 16) == 0 && attributes == EFI_OPEN_PROTOCOL_GET_PROTOCOL && handle == &fake_image_handle) {
    *protocol_interface = efi_loaded_image_protocol;
    return EFI_SUCCESS;
  }
  cons_write("Unsupported open_protocol\n");
  return EFI_UNSUPPORTED;
}

efi_system_table_t * sys_fw_init (const char *args, int arglen, u64 image_base, u64 image_size)
{
	efi_system_table_t *efi_systab;
	efi_runtime_services_t *efi_runtime;
	efi_boot_services_t *efi_boottime;
	efi_simple_text_output_interface_t *efi_conout;
	efi_simple_input_interface_t *efi_conin;
	efi_config_table_t *efi_tables;
	struct ia64_sal_systab *sal_systab;
	efi_memory_desc_t *efi_memmap, *md;
	unsigned long *pal_desc, *sal_desc;
	struct ia64_sal_desc_entry_point *sal_ed;
	unsigned char checksum = 0;
	char *cp, *sal_end;
	int i = 0;

	memset(fw_mem, 0, sizeof(fw_mem));

	pal_desc = (unsigned long *) &pal_emulator_static;
	sal_desc = (unsigned long *) &sal_emulator;

	cp = fw_mem;
	efi_systab  = (void *) cp; cp += sizeof(*efi_systab);
	efi_runtime = (void *) cp; cp += sizeof(*efi_runtime);
	efi_conout  = (void *) cp; cp += sizeof(*efi_conout);
	efi_conin  = (void *) cp; cp += sizeof(*efi_conin); 
	efi_boottime = (void *) cp; cp += sizeof(*efi_boottime);
	efi_loaded_image_protocol = (void *) cp; cp += sizeof(*efi_loaded_image_protocol);
	efi_tables  = (void *) cp; cp += sizeof(*efi_tables);
	sal_systab  = (void *) cp; cp += sizeof(*sal_systab);
	sal_ed      = (void *) cp; cp += sizeof(*sal_ed);
	sal_end     = (void *) cp;

	memset(efi_systab, 0, sizeof(*efi_systab));
	efi_systab->hdr.signature = EFI_SYSTEM_TABLE_SIGNATURE;
	efi_systab->hdr.revision  = ((1 << 16) | 00);
	efi_systab->hdr.headersize = sizeof(efi_systab->hdr);
	efi_systab->fw_vendor = __pa("H\0e\0w\0l\0e\0t\0t\0-\0P\0a\0c\0k\0a\0r\0d\0\0");
	efi_systab->fw_revision = 1;
	efi_systab->runtime = (void *) __pa(efi_runtime);
	efi_systab->boottime = (void *) __pa(efi_boottime);
	efi_systab->con_in = (void *) __pa(efi_conin);
	efi_systab->con_out = (void *) __pa(efi_conout);
	efi_systab->nr_tables = 1;
	efi_systab->tables = __pa(efi_tables);

	efi_conout->reset = (void *)__pa(&efi_unimplemented);
	efi_conout->output_string = (void *)__pa(&efi_output_string);
	efi_conout->test_string = (void *)__pa(&efi_unimplemented);
	efi_conout->query_mode = (void *)__pa(&efi_unimplemented);
	efi_conout->set_mode = (void *)__pa(&efi_unimplemented);
	efi_conout->set_attributes = (void *)__pa(&efi_unimplemented);
	efi_conout->clear_screen = (void *)__pa(&efi_unimplemented);
	efi_conout->set_cursor_position = (void *)__pa(&efi_unimplemented);
	efi_conout->enable_cursor = (void *)__pa(&efi_unimplemented);

	efi_conin->reset = (void *)__pa(&efi_unimplemented);
	efi_conin->read_key_stroke = (void *)__pa(&efi_read_key_stroke);

	efi_boottime->hdr.signature = EFI_BOOT_SERVICES_SIGNATURE;
	efi_boottime->hdr.revision = EFI_BOOT_SERVICES_REVISION;
	efi_boottime->hdr.headersize = sizeof(efi_boottime->hdr);

	efi_boottime->raise_tpl = (void *)__pa(&efi_unimplemented);
	efi_boottime->restore_tpl = (void *)__pa(&efi_unimplemented);
	efi_boottime->allocate_pages = (void *)__pa(&efi_allocate_pages);
	efi_boottime->free_pages = (void *)__pa(&efi_unimplemented);
	efi_boottime->get_memory_map = (void *)__pa(&efi_get_memory_map);
	efi_boottime->allocate_pool = (void *)__pa(&efi_unimplemented);
	efi_boottime->free_pool = (void *)__pa(&efi_unimplemented);
	efi_boottime->create_event = (void *)__pa(&efi_unimplemented);
	efi_boottime->set_timer = (void *)__pa(&efi_unimplemented);
	efi_boottime->wait_for_event = (void *)__pa(&efi_unimplemented);
	efi_boottime->signal_event = (void *)__pa(&efi_unimplemented);
	efi_boottime->close_event = (void *)__pa(&efi_unimplemented);
	efi_boottime->check_event = (void *)__pa(&efi_unimplemented);
	efi_boottime->install_protocol_interface = (void *)__pa(&efi_unimplemented);
	efi_boottime->reinstall_protocol_interface = (void *)__pa(&efi_unimplemented);
	efi_boottime->uninstall_protocol_interface = (void *)__pa(&efi_unimplemented);
	efi_boottime->handle_protocol = (void *)__pa(&efi_unimplemented);
	efi_boottime->__reserved = (void *)__pa(&efi_unimplemented);
	efi_boottime->register_protocol_notify = (void *)__pa(&efi_unimplemented);
	efi_boottime->locate_handle = (void *)__pa(&efi_unimplemented);
	efi_boottime->locate_device_path = (void *)__pa(&efi_unimplemented);
	efi_boottime->install_configuration_table = (void *)__pa(&efi_unimplemented);
	efi_boottime->load_image = (void *)__pa(&efi_unimplemented);
	efi_boottime->start_image = (void *)__pa(&efi_unimplemented);
	efi_boottime->exit = (void *)__pa(&efi_unimplemented);
	efi_boottime->unload_image = (void *)__pa(&efi_unimplemented);
	efi_boottime->exit_boot_services = (void *)__pa(&efi_success_stub);
	efi_boottime->get_next_monotonic_count = (void *)__pa(&efi_unimplemented);
	efi_boottime->stall = (void *)__pa(&efi_unimplemented);
	efi_boottime->set_watchdog_timer = (void *)__pa(&efi_unimplemented);
	efi_boottime->connect_controller = (void *)__pa(&efi_unimplemented);
	efi_boottime->disconnect_controller = (void *)__pa(&efi_unimplemented);
	efi_boottime->open_protocol = (void *)__pa(&efi_open_protocol);
	efi_boottime->close_protocol = (void *)__pa(&efi_unimplemented);
	efi_boottime->open_protocol_information = (void *)__pa(&efi_unimplemented);
	efi_boottime->protocols_per_handle = (void *)__pa(&efi_unimplemented);
	efi_boottime->locate_handle_buffer = (void *)__pa(&efi_unimplemented);
	efi_boottime->locate_protocol = (void *)__pa(&efi_unimplemented);
	efi_boottime->install_multiple_protocol_interfaces = (void *)__pa(&efi_unimplemented);
	efi_boottime->uninstall_multiple_protocol_interfaces = (void *)__pa(&efi_unimplemented);
	efi_boottime->calculate_crc32 = (void *)__pa(&efi_unimplemented);
	efi_boottime->copy_mem = (void *)__pa(&efi_unimplemented);
	efi_boottime->set_mem = (void *)__pa(&efi_unimplemented);
	efi_boottime->create_event_ex = (void *)__pa(&efi_unimplemented);

	efi_loaded_image_protocol->revision = 0x1000;
	efi_loaded_image_protocol->image_base = (void *) image_base;
	efi_loaded_image_protocol->image_size = image_size;
	efi_loaded_image_protocol->image_code_type = EFI_LOADER_CODE;
	efi_loaded_image_protocol->image_data_type = EFI_LOADER_DATA;
	efi_loaded_image_protocol->unload = (void *)__pa(&efi_unimplemented);

	efi_runtime->hdr.signature = EFI_RUNTIME_SERVICES_SIGNATURE;
	efi_runtime->hdr.revision = EFI_RUNTIME_SERVICES_REVISION;
	efi_runtime->hdr.headersize = sizeof(efi_runtime->hdr);
	efi_runtime->get_time = (void *)__pa(&fw_efi_get_time);
	efi_runtime->set_time = (void *)__pa(&efi_unimplemented);
	efi_runtime->get_wakeup_time = (void *)__pa(&efi_unimplemented);
	efi_runtime->set_wakeup_time = (void *)__pa(&efi_unimplemented);
	efi_runtime->set_virtual_address_map = (void *)__pa(&efi_unimplemented);
	efi_runtime->get_variable = (void *)__pa(&efi_unimplemented);
	efi_runtime->get_next_variable = (void *)__pa(&efi_unimplemented);
	efi_runtime->set_variable = (void *)__pa(&efi_unimplemented);
	efi_runtime->get_next_high_mono_count = (void *)__pa(&efi_unimplemented);
	efi_runtime->reset_system = (void *)__pa(&efi_reset_system);

	efi_tables->guid = SAL_SYSTEM_TABLE_GUID;
	efi_tables->table = __pa(sal_systab);

	/* fill in the SAL system table: */
	memcpy(sal_systab->signature, "SST_", 4);
	sal_systab->size = sizeof(*sal_systab);
	sal_systab->sal_rev_minor = 1;
	sal_systab->sal_rev_major = 0;
	sal_systab->entry_count = 1;

	strcpy(sal_systab->oem_id, "Hewlett-Packard");
	strcpy(sal_systab->product_id, "HP-simulator");

	/* fill in an entry point: */
	sal_ed->type = SAL_DESC_ENTRY_POINT;
	sal_ed->pal_proc = __pa(pal_desc[0]);
	sal_ed->sal_proc = __pa(sal_desc[0]);
	sal_ed->gp = __pa(sal_desc[1]);

	for (cp = (char *) sal_systab; cp < (char *) sal_end; ++cp)
		checksum += *cp;

	sal_systab->checksum = -checksum;

	return (efi_system_table_t *) __pa(&fw_mem);
}
