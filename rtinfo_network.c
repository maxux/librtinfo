/*
 * network interfaces statistics support for librtinfo
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
#include <stropts.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <linux/netdevice.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include "misc.h"
#include "rtinfo.h"

rtinfo_network_t * rtinfo_init_network(int *nbiface) {
	FILE *fp;
	char data[256];
	rtinfo_network_t *net;
	int i;
	
	fp = fopen(LIBRTINFO_NET_FILE, "r");

	if(!fp) {
		perror(LIBRTINFO_NET_FILE);
		exit(1);
	}

	/* Counting number of interfaces availble */
	*nbiface = 0;
	while(fgets(data, sizeof(data), fp) != NULL) {
		/* Skip header */
		if(!strncmp(data, "Inter-|", 7))
			continue;
		
		if(!strncmp(data, " face |", 7))
			continue;
		
		*nbiface += 1;
	}
	
	/* Allocating */
	net = (rtinfo_network_t*) malloc(sizeof(rtinfo_network_t) * *nbiface);
	if(!net)
		return NULL;
	
	rewind(fp);
	
	/* Initializing values */
	i = 0;
	while(fgets(data, sizeof(data), fp) != NULL) {
		/* Skip header */
		if(!strncmp(data, "Inter-|", 7))
			continue;
		
		if(!strncmp(data, " face |", 7))
			continue;
		
		net[i].name = getinterfacename(data);
		
		net[i].current.up   = 0;
		net[i].current.down = 0;
		
		net[i].previous.up   = 0;
		net[i].previous.down = 0;
		
		i++;
	}
	
	return net;
}

/* For each interfaces, save old values, write on node */
rtinfo_network_t * rtinfo_get_network(rtinfo_network_t *net, int nbiface) {
	FILE *fp;
	char data[256];
	short i = 0;

	fp = fopen(LIBRTINFO_NET_FILE, "r");

	if(!fp) {
		perror(LIBRTINFO_NET_FILE);
		exit(1);
	}

	while(fgets(data, sizeof(data), fp) != NULL && i < nbiface) {
		/* Skip header */
		if(!strncmp(data, "Inter-|", 7))
			continue;
		
		if(!strncmp(data, " face |", 7))
			continue;
		
		/* Saving previous data */
		net[i].previous = net[i].current;
		
		net[i].current.up   = indexll(data, 10);
		net[i].current.down = indexll(data, 2);

		i++;
	}

	fclose(fp);
	
	rtinfo_get_network_ipv4(net, nbiface);
	
	return net;
}

rtinfo_network_t * rtinfo_get_network_ipv4(rtinfo_network_t *net, int nbiface) {
	int sockfd;
	struct ifconf ifconf;
	struct ifreq ifr[50];
	int ifs;
	int i, j;
	char ip[INET_ADDRSTRLEN];
	struct sockaddr_in *s_in;

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if(sockfd < 0)
		diep("socket");

	ifconf.ifc_buf = (char *) ifr;
	ifconf.ifc_len = sizeof(ifr);

	if(ioctl(sockfd, SIOCGIFCONF, &ifconf) == -1)
		diep("ioctl");

	ifs = ifconf.ifc_len / sizeof(ifr[0]);
	
	/* Reset IP */
	for(j = 0; j < nbiface; j++)
		*(net[j].ip) = '\0';
				
	for(i = 0; i < ifs; i++) {
		s_in = (struct sockaddr_in *) &ifr[i].ifr_addr;

		if(!inet_ntop(AF_INET, &s_in->sin_addr, ip, sizeof(ip)))
			diep("inet_ntop");

		for(j = 0; j < nbiface; j++) {
			if(!strcmp(ifr[i].ifr_name, net[j].name)) {
				strcpy(net[j].ip, ip);
				break;
			}
		}
	}
	
	close(sockfd);

	return net;
}

rtinfo_network_t * rtinfo_mk_network_usage(rtinfo_network_t *net, int nbiface, int timewait) {
	int i;
	
	/* Network Usage: (current load - previous load) / timewait (milli sec) */
	for(i = 0; i < nbiface; i++) {
		net[i].down_rate = ((net[i].current.down - net[i].previous.down) / (timewait / 1000));
		net[i].up_rate   = ((net[i].current.up - net[i].previous.up) / (timewait / 1000));
		
		if(net[i].down_rate < 0)
			net[i].down_rate = 0;
		
		if(net[i].up_rate < 0)
			net[i].up_rate = 0;
	}

	return net;
}
