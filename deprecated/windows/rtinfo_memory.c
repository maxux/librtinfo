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
#include "misc.h"
#include "rtinfo.h"

/* Fix Windows Version Detection, for MEMORYSTATUSEX */
#if (_WIN32_WINNT < 0x0500) 
	#define _WIN32_WINNT  0x0501
#endif

#include "windows.h"

rtinfo_memory_t * rtinfo_get_memory(rtinfo_memory_t *memory) {
	MEMORYSTATUSEX meminfo = {
			.dwLength = sizeof(MEMORYSTATUSEX)
		};
	
	GlobalMemoryStatusEx(&meminfo);
    
	memory->swap_total = (meminfo.ullTotalPageFile - meminfo.ullTotalPhys) / 1024;
	memory->swap_free  = (meminfo.ullAvailPageFile - meminfo.ullTotalPhys) / 1024;
	
	memory->ram_total  = meminfo.ullTotalPhys / 1024;
	memory->ram_used   = (meminfo.ullTotalPhys - meminfo.ullAvailPhys) / 1024;
	
	return memory;
}
