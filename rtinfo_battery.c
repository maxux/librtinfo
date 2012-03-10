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
#include <stropts.h>
#include <unistd.h>
#include "misc.h"
#include "rtinfo.h"

rtinfo_battery_t * rtinfo_get_battery(rtinfo_battery_t *battery) {
	FILE *fp;
	char data[32];

	/* Checking for battery presence */
	fp = fopen(BATTERY_PATH "/present", "r");
	if(!fp) {
		perror(BATTERY_PATH);
		exit(1);
	}
	
	fclose(fp);

	/* Reading current charge */
	if(!file_get(BATTERY_PATH "/charge_now", data, sizeof(data)))
		return NULL;
	
	battery->charge_now = atol(data);
	
	/* Reading full_charge */
	if(!file_get(BATTERY_PATH "/charge_full", data, sizeof(data)))
		return NULL;
	
	battery->charge_full = atol(data);
	
	/* Reading status */
	if(!file_get(BATTERY_PATH "/status", data, sizeof(data)))
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
