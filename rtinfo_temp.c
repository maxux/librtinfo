/*
 * librtinfo is a small library for Linux for retreive some system status monitor
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

rtinfo_temp_t * rtinfo_get_temp(rtinfo_temp_t *temp) {
	FILE *fp;
	glob_t globbuf;
	char data[32];
	size_t i;
	uint64_t value = 0;

	globbuf.gl_offs = 1;
	glob("/sys/devices/platform/coretemp.0/temp*_input", GLOB_NOSORT, NULL, &globbuf);
	
	if(globbuf.gl_pathc == 0) {
		temp->cpu_average = 0;
		return temp;
	}
	
	for(i = 0; i < globbuf.gl_pathc; i++) {
		fp = fopen(globbuf.gl_pathv[i], "r");
		if(!fp) {
			perror(globbuf.gl_pathv[i]);
			return NULL;
		}
		
		if(fgets(data, sizeof(data), fp))
			value += atoi(data);
		
		fclose(fp);
	}
	
	/* Divide per 1000, and divide by core numbers, to get an average */
	temp->cpu_average = value / (globbuf.gl_pathc * 1000);

	return temp;
}
