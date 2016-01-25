/*
 * unix load average support for librtinfo
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
#include <sys/sysctl.h>
#include <sys/resource.h>
#include <unistd.h>
#include "misc.h"
#include "rtinfo.h"

rtinfo_loadagv_t * rtinfo_get_loadavg(rtinfo_loadagv_t *load) {
	struct loadavg loadavg;
	int mib[] = {CTL_VM, VM_LOADAVG};
	size_t loadlen = sizeof(loadavg);
	unsigned int i;
	
	if(sysctl(mib, sizeof(mib) / sizeof(int), &loadavg, &loadlen, NULL, 0))
		diep("loadavg sysctl");
	
	if((sizeof(loadavg.ldavg) / sizeof(fixpt_t)) < 3) {
		fprintf(stderr, "load avg error\n");
		bzero(load, sizeof(rtinfo_loadagv_t));
		return load;
	}
	
	for(i = 0; i < 3; i++)
		load->load[i] = (float) loadavg.ldavg[i] / loadavg.fscale;
	
	return load;
}
