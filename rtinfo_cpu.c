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

rtinfo_cpu_t * rtinfo_init_cpu(int *nbcpu) {
	FILE *fp;
	char data[256];
	rtinfo_cpu_t *cpu;
	int i;
	
	fp = fopen(LIBRTINFO_CPU_FILE, "r");

	if(!fp) {
		perror(LIBRTINFO_CPU_FILE);
		exit(1);
	}

	/* Counting number of cpu availble */
	*nbcpu = 0;
	while(fgets(data, sizeof(data), fp) != NULL) {
		/* Checking cpu line */
		if(strncmp(data, "cpu", 3) != 0)
			break;
		
		*nbcpu += 1;
	}
	
	/* Allocating */
	cpu = (rtinfo_cpu_t*) malloc(sizeof(rtinfo_cpu_t) * *nbcpu);
	if(!cpu)
		return NULL;
	
	/* Initializing values */
	for(i = 0; i < *nbcpu; i++) {
		cpu[i].current.time_total = 0;
		cpu[i].current.time_idle  = 0;
		
		cpu[i].previous.time_total = 0;
		cpu[i].previous.time_idle  = 0;
	}
	
	return cpu;
}

/* For each CPU, save old values, write on the info_cpu_node_t current value read from CPU_FILE */
rtinfo_cpu_t * rtinfo_get_cpu(rtinfo_cpu_t *cpu, int nbcpu) {
	FILE *fp;
	char data[256];
	short i = 0, j = 0;

	fp = fopen(LIBRTINFO_CPU_FILE, "r");

	if(!fp) {
		perror(LIBRTINFO_CPU_FILE);
		exit(1);
	}

	while(fgets(data, sizeof(data), fp) != NULL && j < nbcpu) {
		/* Checking cpu line */
		if(strncmp(data, "cpu", 3) != 0)
			break;
		
		/* Searching first number */
		while(!isspace(*(data + i)))
			i++;
		
		/* Saving previous data */
		cpu[j].previous = cpu[j].current;
		
		/* cpu0    53464243 2698605 20822211 794620668 16726825 3778099 4582652 0 0 0	*/
		/*         [.......................] [ idle ]  [............................]	*/
		/* [name]  [..................... total cpu time ...........................]	*/
		
		cpu[j].current.time_total = sum_line(data + i);
		cpu[j].current.time_idle  = indexll(data, 4);

		j++;
	}

	fclose(fp);
	
	return cpu;
}

rtinfo_cpu_t * rtinfo_mk_cpu_usage(rtinfo_cpu_t *cpu, int nbcpu) {
	int i;
	
	/* CPU Usage: 100 * (delta cpu time - delta idle time) / delta cpu time */
	for(i = 0; i < nbcpu; i++) {
		if(cpu[i].current.time_total != cpu[i].previous.time_total)
			cpu[i].usage = 100 * ((cpu[i].current.time_total - cpu[i].previous.time_total) - (cpu[i].current.time_idle - cpu[i].previous.time_idle)) / (cpu[i].current.time_total - cpu[i].previous.time_total);
			
		else cpu[i].usage = 0;
	}
		
	return cpu;
}
