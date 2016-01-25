/*
 * system memory usage support for librtinfo
 * Copyright (C) 2012  DANIEL Maxime <root@maxux.net>
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
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 */
 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdint.h>
#include <unistd.h>
#include <sys/sysctl.h>
#include <mach/vm_statistics.h>
#include <mach/mach_types.h>
#include <mach/mach_init.h>
#include <mach/mach_host.h>
#include "misc.h"
#include "rtinfo.h"

rtinfo_memory_t * rtinfo_get_memory(rtinfo_memory_t *memory) {
	struct xsw_usage swap;
	unsigned long ram;
	
	int mib[] = {CTL_VM, VM_SWAPUSAGE};
	size_t swaplen = sizeof(swap);
	size_t ramlen  = sizeof(ram);
	
	vm_size_t page_size = 0;
	vm_statistics_data_t vm_stats;
	mach_msg_type_number_t count = sizeof(vm_stats) / sizeof(natural_t);
	
	/* Request: swap */
	if(sysctl(mib, sizeof(mib) / sizeof(int), &swap, &swaplen, NULL, 0))
		diep("swap sysctl");
	
	memory->swap_total = swap.xsu_total / 1024;
	memory->swap_free  = swap.xsu_avail / 1024;

	/* Request: ram */
	mib[0] = CTL_HW;
	mib[1] = HW_PHYSMEM;
	
	if(sysctl(mib, sizeof(mib) / sizeof(int), &ram, &ramlen, NULL, 0))
		diep("ram sysctl");
	
	memory->ram_total = ram / 1024;
	
	/* Used ram */
	bzero(&vm_stats, sizeof(vm_statistics_data_t));
	
	if(host_page_size(mach_host_self(), &page_size) != KERN_SUCCESS)
		fprintf(stderr, "host_page_size failed\n");
		
	if(host_statistics(mach_host_self(), HOST_VM_INFO, (host_info_t) &vm_stats, &count) != KERN_SUCCESS)
		fprintf(stderr, "host_statistics failed\n");
	
	memory->ram_used = ((vm_stats.active_count + vm_stats.inactive_count + vm_stats.wire_count) * page_size) / 1024;
	
	return memory;
}
