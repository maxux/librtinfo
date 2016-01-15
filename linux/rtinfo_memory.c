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
#include "misc.h"
#include "rtinfo.h"

rtinfo_memory_t * rtinfo_get_memory(rtinfo_memory_t *memory) {
	FILE *fp;
	char data[32], missing;
	unsigned int _memfree = 0, _buffers = 0, _cached = 0;

	if(!(fp = fopen(LIBRTINFO_MEMORY_FILE, "r")))
		__rtinfo_internal_diep(LIBRTINFO_MEMORY_FILE);

	/* Init Memory */
	memory->ram_used  = 0;	/* Init Used ram to zero */
	memory->swap_free = 0;	/* Init free Swap */
	missing = 6;		/* Numbers of lines to read */

	while(missing && fgets(data, sizeof(data), fp) != NULL) {
		if(strncmp(data, "MemTotal:", 9) == 0) {
			memory->ram_total = atoll(data+10);
			missing--;

		} else if(strncmp(data, "MemFree:", 8) == 0) {
			_memfree = atoll(data+9);
			missing--;

		} else if(strncmp(data, "Buffers:", 8) == 0) {
			_buffers = atoll(data+9);
			missing--;

		} else if(strncmp(data, "Cached:", 7) == 0) {
			_cached = atoll(data+8);
			missing--;

		} else if(strncmp(data, "SwapTotal:", 10) == 0) {
			memory->swap_total = atoll(data+11);
			missing--;

		} else if(strncmp(data, "SwapFree:", 9) == 0) {
			memory->swap_free = atoll(data+9);
			missing--;
		}
	}

	fclose(fp);
	
	/* Checking if all data required is present */
	if(missing)
		return NULL;

	/* Calculating */
	memory->ram_used = memory->ram_total - _memfree - _buffers - _cached;
	
	return memory;
}
