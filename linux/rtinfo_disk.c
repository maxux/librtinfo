/*
 * cpu statistics usage support for librtinfo
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
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include "misc.h"
#include "rtinfo.h"

rtinfo_disk_dev_t *__rtinfo_internal_disk_getdevice(rtinfo_disk_t *disk, char *match) {
	unsigned int i;
	
	for(i = 0; i < disk->nbdisk; i++) {
		if(!strcmp(disk->dev[i].name, match))
			return disk->dev + i;
	}
	
	return NULL;
}

rtinfo_disk_t *__rtinfo_internal_disk_setsectors(rtinfo_disk_t *disk) {
	unsigned int i;
	char path[512], content[128];
	
	for(i = 0; i < disk->nbdisk; i++) {
		snprintf(path, sizeof(path), "/sys/block/%s/queue/hw_sector_size", disk->dev[i].name);
		
		if(!file_get(path, content, sizeof(content))) {
			disk->dev[i].sectorsize = 0;
			continue;
		}
		
		disk->dev[i].sectorsize = atoi(content);
	}
	
	return disk;
}

int __rtinfo_internal_disk_isblock(char *device) {
	char path[512];
	
	snprintf(path, sizeof(path), "/sys/block/%s/dev", device);
	return !access(path, F_OK);
}

int __rtinfo_internal_disk_nbdisk(char *prefix) {
	FILE *fp;
	char temp[32], data[256];
	char *match;
	unsigned int length, nbdisk = 0;
	
	if(!(fp = fopen(LIBRTINFO_DISK_FILE, "r")))
		__rtinfo_internal_diep(LIBRTINFO_DISK_FILE);

	/* Counting number of matching disk availble */
	while(fgets(data, sizeof(data), fp) != NULL) {
		match  = index_string(data, 2);
		length = field_length(data, 2);
		
		// buffer overrun protection
		if(length > sizeof(temp))
			continue;
		
		strncpy(temp, match, length);
		temp[length] = '\0';
		
		/* disk eligible */
		if(prefix && strncmp(temp, prefix, strlen(prefix)))
			continue;
		
		if(!__rtinfo_internal_disk_isblock(temp))
			continue;
		
		/* disk eligible */		
		nbdisk++;
	}
	
	fclose(fp);
	
	return nbdisk;
}

int __rtinfo_internal_disk_setnames(rtinfo_disk_t *disk, char *prefix) {
	FILE *fp;
	char temp[32], data[256];
	char *match;
	unsigned int length, nbdisk = 0;
	
	if(!(fp = fopen(LIBRTINFO_DISK_FILE, "r")))
		__rtinfo_internal_diep(LIBRTINFO_DISK_FILE);

	/* Counting number of matching disk availble */
	while(fgets(data, sizeof(data), fp) != NULL) {
		match  = index_string(data, 2);
		length = field_length(data, 2);
		
		// buffer overrun protection
		if(length > sizeof(temp))
			continue;
		
		strncpy(temp, match, length);
		temp[length] = '\0';
		
		/* disk eligible */
		if(prefix && strncmp(temp, prefix, strlen(prefix)))
			continue;
		
		if(!__rtinfo_internal_disk_isblock(temp)) 
			continue;
		
		disk->dev[nbdisk].name = strdup(temp);
		
		/* disk eligible */		
		nbdisk++;
	}
	
	fclose(fp);
	
	return nbdisk;
}



rtinfo_disk_t *rtinfo_init_disk(char *prefix) {
	rtinfo_disk_t *disk;
	unsigned int nbdisk = 0;
	
	/* detecting available disks */
	nbdisk = __rtinfo_internal_disk_nbdisk(prefix);
	
	/* Allocating */
	if(!(disk = (rtinfo_disk_t*) malloc(sizeof(rtinfo_disk_t))))
		return NULL;
	
	disk->nbdisk = nbdisk;
	
	if(!(disk->dev = (rtinfo_disk_dev_t*) calloc(disk->nbdisk, sizeof(rtinfo_disk_dev_t))))
		return NULL;
	
	/* nothing more to do */
	if(!nbdisk)
		return disk;
	
	/* apply names to all disk found */
	__rtinfo_internal_disk_setnames(disk, prefix);
	
	/* set sectors size to all disks found */
	__rtinfo_internal_disk_setsectors(disk);
	
	return disk;
}

void rtinfo_free_disk(rtinfo_disk_t *disk) {
	unsigned int i;
	
	for(i = 0; i < disk->nbdisk; i++)
		free(disk->dev[i].name);
	
	free(disk->dev);
	free(disk);
}

/* For each CPU, save old values, write on the info_cpu_node_t current value read from CPU_FILE */
rtinfo_disk_t *rtinfo_get_disk(rtinfo_disk_t *disk) {
	FILE *fp;
	char data[256], *match;
	unsigned short i = 0;
	unsigned int length;
	rtinfo_disk_dev_t *dev;
	char temp[32];

	if(!(fp = fopen(LIBRTINFO_DISK_FILE, "r")))
		__rtinfo_internal_diep(LIBRTINFO_DISK_FILE);

	while(fgets(data, sizeof(data), fp) != NULL && i < disk->nbdisk) {
		match  = index_string(data, 2);
		length = field_length(data, 2);
		
		strncpy(temp, match, length);
		temp[length] = '\0';
		
		/* device not useful for us */
		if(!(dev = __rtinfo_internal_disk_getdevice(disk, temp)))
			continue;
		
		/* saving previous data */
		disk->dev[i].previous = disk->dev[i].current;
		
		// unpadding
		match = index_string(data, 0);		
		disk->dev[i].current.read = indexll(match, 5) * disk->dev[i].sectorsize;
		disk->dev[i].current.written  = indexll(match, 9) * disk->dev[i].sectorsize;

		i++;
	}

	fclose(fp);
	
	return disk;
}

rtinfo_disk_t *rtinfo_mk_disk_usage(rtinfo_disk_t *disk, int timewait) {
	unsigned int i;
	
	for(i = 0; i < disk->nbdisk; i++) {
		disk->dev[i].read_speed  = (disk->dev[i].current.read - disk->dev[i].previous.read) / (timewait / 1000.0);
		disk->dev[i].write_speed = (disk->dev[i].current.written - disk->dev[i].previous.written) / (timewait / 1000.0);
	}
		
	return disk;
}
