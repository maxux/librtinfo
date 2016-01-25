/*
 * system uptime support for librtinfo
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
#include <inttypes.h>
#include <string.h>
#include <sys/sysctl.h>
#include <unistd.h>
#include <time.h>
#include "misc.h"
#include "rtinfo.h"

/* For each interfaces, save old values, write on node */
rtinfo_uptime_t * rtinfo_get_uptime(rtinfo_uptime_t *uptime) {
	struct timeval tv;
	int mib[] = {CTL_KERN, KERN_BOOTTIME};
	size_t lentv = sizeof(tv);
	
	if(sysctl(mib, sizeof(mib) / sizeof(int), &tv, &lentv, NULL, 0))
		diep("uptime sysctl");
	
	uptime->uptime = time(NULL) - tv.tv_sec;
	
	return uptime;
}
