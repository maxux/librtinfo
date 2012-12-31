/*
 * hddtemp tcp support for librtinfo
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
#include <inttypes.h>
#include "misc.h"
#include "rtinfo.h"

int __rtinfo_internal_hddtemp_connect() {
	return 0;
}

uint16_t __rtinfo_internal_hddtemp_parse(char *buffer, unsigned int *peak) {
	(void) buffer;
	(void) peak;
	
	return 0;
}

rtinfo_temp_hdd_t * rtinfo_get_temp_hdd(rtinfo_temp_hdd_t *hddtemp) {
	hddtemp->hdd_average = 0;
	hddtemp->peak 	     = 0;
	
	return hddtemp;
}
