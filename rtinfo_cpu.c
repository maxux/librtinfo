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
#include <stropts.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include "misc.h"
#include "rtinfo.h"

rtinfo_cpu_t * rtinfo_init_cpu() {
	FILE *fp;
	char data[256];
	rtinfo_cpu_t *cpu;
	unsigned int nbcpu;
	
	if(!(fp = fopen(LIBRTINFO_CPU_FILE, "r")))
		diep(LIBRTINFO_CPU_FILE);

	/* Counting number of cpu availble */
	nbcpu = 0;
	while(fgets(data, sizeof(data), fp) != NULL) {
		/* Checking cpu line */
		if(strncmp(data, "cpu", 3) != 0)
			break;
		
		nbcpu++;
	}
	
	fclose(fp);
	
	/* Allocating */
	if(!(cpu = (rtinfo_cpu_t*) malloc(sizeof(rtinfo_cpu_t))))
		return NULL;
	
	cpu->nbcpu = nbcpu;
	
	if(!(cpu->dev = (rtinfo_cpu_dev_t*) calloc(cpu->nbcpu, sizeof(rtinfo_cpu_dev_t))))
		return NULL;
	
	return cpu;
}

/* For each CPU, save old values, write on the info_cpu_node_t current value read from CPU_FILE */
rtinfo_cpu_t * rtinfo_get_cpu(rtinfo_cpu_t *cpu) {
	FILE *fp;
	char data[256];
	unsigned short i = 0, j = 0;

	if(!(fp = fopen(LIBRTINFO_CPU_FILE, "r")))
		diep(LIBRTINFO_CPU_FILE);

	while(fgets(data, sizeof(data), fp) != NULL && j < cpu->nbcpu) {
		/* Checking cpu line */
		if(strncmp(data, "cpu", 3) != 0)
			break;
		
		/* Searching first number */
		while(!isspace(*(data + i)))
			i++;
		
		/* Saving previous data */
		cpu->dev[j].previous = cpu->dev[j].current;
		
		/* cpu0    53464243 2698605 20822211 794620668 16726825 3778099 4582652 0 0 0	*/
		/*         [.......................] [ idle ]  [............................]	*/
		/* [name]  [..................... total cpu time ...........................]	*/
		
		cpu->dev[j].current.time_total = sum_line(data + i);
		cpu->dev[j].current.time_idle  = indexll(data, 4);

		j++;
	}

	fclose(fp);
	
	return cpu;
}

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
