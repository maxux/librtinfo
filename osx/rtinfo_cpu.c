/*
 * cpu statistics usage support for librtinfo
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
#include <sys/ioctl.h>
#include <unistd.h>
#include <sys/sysctl.h>
#include <mach/mach_init.h>
#include <mach/mach_error.h>
#include <mach/mach_host.h>
#include <mach/vm_map.h>
#include "misc.h"
#include "rtinfo.h"

rtinfo_cpu_t * rtinfo_init_cpu() {
	rtinfo_cpu_t *cpu;
	unsigned int nbcpu = 0;
	
	size_t cpulen = sizeof(nbcpu);;
	int mib[] = {CTL_HW, HW_NCPU};
	
	/* Request */
	if(sysctl(mib, sizeof(mib) / sizeof(int), &nbcpu, &cpulen, NULL, 0))
		diep("cpu sysctl");
	
	/* Allocating */
	if(!(cpu = (rtinfo_cpu_t*) malloc(sizeof(rtinfo_cpu_t))))
		return NULL;
	
	cpu->nbcpu = nbcpu + 1;
	
	if(!(cpu->dev = (rtinfo_cpu_dev_t*) calloc(cpu->nbcpu, sizeof(rtinfo_cpu_dev_t))))
		return NULL;
	
	return cpu;
}

/* At this time, only summary are availible */
rtinfo_cpu_t * rtinfo_get_cpu(rtinfo_cpu_t *cpu) {
	host_cpu_load_info_data_t cpuinfo;
	mach_msg_type_number_t count = HOST_CPU_LOAD_INFO_COUNT;
	unsigned int i;
   
	if(host_statistics(mach_host_self(), HOST_CPU_LOAD_INFO, (host_info_t) &cpuinfo, &count) != KERN_SUCCESS) {
		fprintf(stderr, "cpu host_statistics failed");
		return cpu;
	}
	
	/* Saving previous data */
	cpu->dev[0].previous = cpu->dev[0].current;
	
	/* Reset value */
	cpu->dev[0].current.time_total = 0;
	
	/* Counting cpu time */
	for(i = 0; i < CPU_STATE_MAX; i++)
		cpu->dev[0].current.time_total += cpuinfo.cpu_ticks[i];
	
	/* Reading idle time */
	cpu->dev[0].current.time_idle = cpuinfo.cpu_ticks[CPU_STATE_IDLE];
	
	return cpu;
}

/* At this time, only one cpu (summary) availible */
rtinfo_cpu_t * rtinfo_mk_cpu_usage(rtinfo_cpu_t *cpu) {
	unsigned int i;
	
	/* CPU Usage: 100 * (delta cpu time - delta idle time) / delta cpu time */
	for(i = 0; i < cpu->nbcpu; i++) {
		if(cpu->dev[i].current.time_total != cpu->dev[i].previous.time_total)
			cpu->dev[i].usage = 100 * ((cpu->dev[i].current.time_total - cpu->dev[i].previous.time_total) - (cpu->dev[i].current.time_idle - cpu->dev[i].previous.time_idle)) / (cpu->dev[i].current.time_total - cpu->dev[i].previous.time_total);
			
		else cpu->dev[i].usage = 0;
	}
		
	return cpu;
}

/* 




float CalculateCPULoad(unsigned long long idleTicks, unsigned long long totalTicks) {
  unsigned long long totalTicksSinceLastTime = totalTicks-_previousTotalTicks;
  unsigned long long idleTicksSinceLastTime  = idleTicks-_previousIdleTicks;
  
  float ret = 1.0f-((totalTicksSinceLastTime > 0) ? ((float)idleTicksSinceLastTime)/totalTicksSinceLastTime : 0);
  
  return ret;
}

*/
