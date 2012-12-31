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
#include <unistd.h>
#include "misc.h"
#include "rtinfo.h"

#include <windows.h>
#include <stdio.h>

typedef BOOL (__stdcall * pfnGetSystemTimes)(LPFILETIME, LPFILETIME, LPFILETIME);
static pfnGetSystemTimes s_pfnGetSystemTimes = NULL;

uint64_t __rtinfo_internal_cpu_filetime(FILETIME ft) {
	ULARGE_INTEGER large;
	
	large.u.LowPart  = ft.dwLowDateTime;
	large.u.HighPart = ft.dwHighDateTime;
	
	return large.QuadPart;
}

rtinfo_cpu_t * rtinfo_init_cpu() {
	rtinfo_cpu_t *cpu;
	unsigned int nbcpu = atoi(getenv("NUMBER_OF_PROCESSORS"));
	HMODULE s_hKernel = NULL;
	
	/* Allocating */
	if(!(cpu = (rtinfo_cpu_t*) malloc(sizeof(rtinfo_cpu_t))))
		return NULL;
	
	cpu->nbcpu = nbcpu + 1;
	
	if(!(cpu->dev = (rtinfo_cpu_dev_t*) calloc(cpu->nbcpu, sizeof(rtinfo_cpu_dev_t))))
		return NULL;
	
	if((s_hKernel = LoadLibrary("Kernel32.dll"))) {
		if(!(s_pfnGetSystemTimes = (pfnGetSystemTimes) GetProcAddress(s_hKernel, "GetSystemTimes"))) {
			FreeLibrary(s_hKernel);
			s_hKernel = NULL;
		}
	}
	
	return cpu;
}

/* At this time, only summary are availible */
rtinfo_cpu_t * rtinfo_get_cpu(rtinfo_cpu_t *cpu) {
	unsigned int i;
	FILETIME ft_sys_idle, ft_sys_kernel, ft_sys_user;
	uint64_t user_time, kernel_time;
	
	s_pfnGetSystemTimes(&ft_sys_idle, &ft_sys_kernel, &ft_sys_user);
	
	kernel_time = __rtinfo_internal_cpu_filetime(ft_sys_kernel);
	user_time   = __rtinfo_internal_cpu_filetime(ft_sys_user);
	
	/* for(i = 0; i < cpu->nbcpu; i++) { */
	for(i = 0; i < 1; i++) {
		cpu->dev[i].previous = cpu->dev[i].current;
		
		cpu->dev[i].current.time_idle  = __rtinfo_internal_cpu_filetime(ft_sys_idle);
		cpu->dev[i].current.time_total = user_time + kernel_time;
	}
	
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
