/*
 * cpu temerature support for librtinfo
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
#include <stdint.h>
#include <unistd.h>
#include <glob.h>
#include "misc.h"
#include "rtinfo.h"

typedef struct cpu_temp_t {
	char *path;
	double multiplier;
	
} cpu_temp_t;

/* should be ordered by pertinence */
cpu_temp_t __temp_cpu[] = {
	{
		.path = "/sys/devices/platform/coretemp.0/temp*_input",
		.multiplier = 0.001
	},
	{
		.path = "/sys/devices/platform/coretemp.0/hwmon/hwmon*/temp*_input",
		.multiplier = 0.001
	},
	{
		.path = "/sys/class/thermal/thermal_zone0/temp",
		.multiplier = 0.001
	},
};

/*
 *  Coretemp support
 */
rtinfo_temp_cpu_t * rtinfo_get_temp_cpu(rtinfo_temp_cpu_t *temp) {
	FILE *fp;
	glob_t globbuf;
	char data[32];
	size_t k, i;
	double value = 0;
	unsigned int found = 0;
	
	/* default value */
	temp->cpu_average = 0;
	temp->critical    = 0;
	
	globbuf.gl_offs = 1;
	
	for(k = 0; k < sizeof(__temp_cpu) / sizeof(cpu_temp_t); k++) {
		glob(__temp_cpu[k].path, GLOB_NOSORT, NULL, &globbuf);
		
		for(i = 0; i < globbuf.gl_pathc; i++) {
			fp = fopen(globbuf.gl_pathv[i], "r");
			if(!fp) {
				perror(globbuf.gl_pathv[i]);
				return NULL;
			}
			
			if(fgets(data, sizeof(data), fp))
				value += atoi(data) * __temp_cpu[k].multiplier;

			found++;
			
			fclose(fp);
		}
		
		globfree(&globbuf);
		
		/* found a sensors, break here */
		if(globbuf.gl_pathc)
			break;
	}
	
	/* no temperature found */
	if(!value)
		return temp;
	
	/* Reading Critital value */
	/* glob("/sys/devices/platform/coretemp.0/temp*_crit", GLOB_NOSORT, NULL, &globbuf);
	if(globbuf.gl_pathc != 0) {	
		fp = fopen(globbuf.gl_pathv[0], "r");
		if(fp) {
			if(fgets(data, sizeof(data), fp))
				temp->critical = atoi(data) / 1000;
				
		} else {
			perror(globbuf.gl_pathv[0]);
			temp->critical = 0;
		}
				
		fclose(fp);
		
	} else temp->critical = 0;
	
	globfree(&globbuf); */
	
	/* Compute average */
	temp->cpu_average = value / found;

	return temp;
}
