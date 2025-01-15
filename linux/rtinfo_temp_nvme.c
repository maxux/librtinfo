/*
 * nvme temperature from kernel nvme-hwmon for librtinfo
 * Copyright (C) 2025  DANIEL Maxime <root@maxux.net>
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
#include <glob.h>
#include "misc.h"
#include "rtinfo.h"

typedef struct nvme_temp_t {
	char *path;
	double multiplier;

} nvme_temp_t;

nvme_temp_t __temp_nvme[] = {
	{
		.path = "/sys/class/nvme/nvme*/hwmon*/temp*_input",
		.multiplier = 0.001
	},
};

rtinfo_temp_hdd_t * rtinfo_get_temp_nvme(rtinfo_temp_hdd_t *hddtemp) {
	FILE *fp;
	glob_t globbuf;
	char data[32];
	size_t k, i;
	double value = 0, now = 0;
	unsigned int found = 0;

	if(hddtemp->last > time(NULL) - 180) {
		rtinfo_debug("[+] librtinfo: nvmetemp: skipping, update too soon (last fetch: %ld)\n", hddtemp->last);
		return hddtemp;
	}

	/* default value */
	hddtemp->peak = 0;
	hddtemp->hdd_average = 0;
	hddtemp->last = time(NULL);

	globbuf.gl_offs = 1;

	for(k = 0; k < sizeof(__temp_nvme) / sizeof(nvme_temp_t); k++) {
		glob(__temp_nvme[k].path, GLOB_NOSORT, NULL, &globbuf);

		for(i = 0; i < globbuf.gl_pathc; i++) {
			fp = fopen(globbuf.gl_pathv[i], "r");
			if(!fp) {
				perror(globbuf.gl_pathv[i]);
				return NULL;
			}

			if(fgets(data, sizeof(data), fp)) {
				now = atoi(data) * __temp_nvme[k].multiplier;
                value += now;

                if((uint16_t) now > hddtemp->peak)
                    hddtemp->peak = (uint16_t) now;
            }

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
		return hddtemp;

    /* Compute average */
	hddtemp->hdd_average = value / found;

	return hddtemp;
}
