/*
 * acpi battery support for librtinfo
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
#include <stropts.h>
#include <unistd.h>
#include <glob.h>
#include "misc.h"
#include "rtinfo.h"

rtinfo_battery_t * rtinfo_get_battery(rtinfo_battery_t *battery, char *name) {
	FILE *fp;
	char data[32];
	char path[128], init_path[100];
	glob_t globbuf;
	
	/* Auto-Name Search */
	if(!name) {
		globbuf.gl_offs = 1;
		glob(LIBRTINFO_BATTERY_PATH "/BAT*", GLOB_NOSORT, NULL, &globbuf);
		
		/* No battery found / TODO: Multiple batteries found (not supported yet) */
		if(globbuf.gl_pathc != 1) {
			battery->load = -1;
			return battery;
			
		}
		
		/* Init Path: Glob found */
		strcpy(init_path, globbuf.gl_pathv[0]);
	
	/* Init Path: BATTERY_PATH/name */
	} else sprintf(init_path, "%s/%s", LIBRTINFO_BATTERY_PATH, name);
	
	if(LIBRTINFO_DEBUG)
		printf("[+] Init Path: %s\n", init_path);

	/* Checking for battery presence */
	sprintf(path, "%s/present", init_path);
	
	fp = fopen(path, "r");
	if(!fp) {
		// perror(BATTERY_PATH);
		battery->load = -1;
		return battery;
	}
	
	fclose(fp);

	/* Reading current charge */
	sprintf(path, "%s/charge_now", init_path);
	if(!file_get(path, data, sizeof(data)))
		return NULL;
	
	battery->charge_now = atol(data);
	
	/* Reading full_charge */
	sprintf(path, "%s/charge_full", init_path);
	if(!file_get(path, data, sizeof(data)))
		return NULL;
	
	battery->charge_full = atol(data);
	
	/* Reading status */
	sprintf(path, "%s/status", init_path);
	
	if(!file_get(path, data, sizeof(data)))
		return NULL;
	
	if(!strncmp(data, "Full", 4))
		battery->status = FULL;
	
	else if(!strncmp(data, "Charging", 4))
		battery->status = CHARGING;
	
	else if(!strncmp(data, "Discharging", 4))
		battery->status = DISCHARGING;
	
	/* Calculating usage */
	battery->load = ((float) battery->charge_now / battery->charge_full) * 100;
	
	return battery;
}
