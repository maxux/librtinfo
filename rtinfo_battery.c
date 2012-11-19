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

rtinfo_battery_status_t __rtinfo_internal_battery_getstatus(char *data) {
	if(!strncmp(data, "Full", 4))
		return FULL;
	
	else if(!strncmp(data, "Charging", 8))
		return CHARGING;
	
	else if(!strncmp(data, "Discharging", 11))
		return DISCHARGING;

	else if(!strncmp(data, "Unknown", 7))
		return BATTERY_UNKNOWN;
	
	return BATTERY_ERROR;
}

rtinfo_battery_t * rtinfo_get_battery(rtinfo_battery_t *battery, char *name) {
	FILE *fp;
	char data[256];
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
		globfree(&globbuf);
	
	/* Init Path: BATTERY_PATH/name */
	} else sprintf(init_path, "%s/%s", LIBRTINFO_BATTERY_PATH, name);
	
	rtinfo_debug("[+] Init Path: %s\n", init_path);

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
	sprintf(path, "%s/uevent", init_path);
	
	if(!(fp = fopen(path, "r"))) {
		perror(path);
		battery->load = -1;
		return battery;
	}
	
	while(fgets(data, sizeof(data), fp)) {
		if(!strncmp(data, "POWER_SUPPLY_STATUS", 19)) {
			battery->status = __rtinfo_internal_battery_getstatus(data + 20);
			continue;
		}
		
		if(!strncmp(data, "POWER_SUPPLY_CHARGE_FULL=", 25) || !strncmp(data, "POWER_SUPPLY_ENERGY_FULL=", 25)) {
			battery->charge_full = atol(data + 25);
			continue;
		}
		
		if(!strncmp(data, "POWER_SUPPLY_CHARGE_NOW", 23) || !strncmp(data, "POWER_SUPPLY_ENERGY_NOW", 23)) {
			battery->charge_now = atol(data + 24);
			continue;
		}
	}
	
	fclose(fp);
	
	/* Calculating usage */
	battery->load = ((float) battery->charge_now / battery->charge_full) * 100;
	
	return battery;
}
