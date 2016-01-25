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
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include "misc.h"
#include "rtinfo.h"

unsigned int __rtinfo_internal_network_nbiface() {
	return 0;
}

rtinfo_network_if_t * __rtinfo_internal_network_getifbyname(rtinfo_network_t *net, char *ifname) {
	(void) net;
	(void) ifname;
	
	return NULL;
}

rtinfo_network_t * __rtinfo_internal_network_reordering(rtinfo_network_t *net) {
	return net;
}

rtinfo_network_t * rtinfo_init_network() {
	rtinfo_network_t *net;
	
	rtinfo_debug("[+] librtinfo: initializing network\n");
	
	if(!(net = (rtinfo_network_t*) malloc(sizeof(rtinfo_network_t))))
		return NULL;
	
	/* Counting number of interfaces availble */
	net->nbiface = 0;
	
	/* Allocating */
	if(!(net->net = (rtinfo_network_if_t*) calloc(net->nbiface, sizeof(rtinfo_network_if_t))))
		return NULL;
	
	/* Saving current malloc */
	net->netcount = net->nbiface;
	
	rtinfo_debug("[+] librtinfo: %u interfaces, %lu bytes\n", net->nbiface, net->nbiface * sizeof(rtinfo_network_if_t));
	
	return net;
}

void rtinfo_free_network(rtinfo_network_t *net) {
	unsigned int i;
	
	for(i = 0; i < net->netcount; i++)
		free(net->net[i].name);
	
	free(net->net);
	free(net);
}

/* For each interfaces, save old values, write on node */
rtinfo_network_t * rtinfo_get_network(rtinfo_network_t *net) {
	return net;
}

rtinfo_network_t * rtinfo_get_network_ipv4(rtinfo_network_t *net) {
	return net;
}

rtinfo_network_t * rtinfo_mk_network_usage(rtinfo_network_t *net, int timewait) {
	(void) timewait;
	unsigned int i;
	
	/* Network Usage: (current load - previous load) / timewait (milli sec) */
	for(i = 0; i < net->netcount; i++) {
		net->net[i].down_rate = 0;
		net->net[i].up_rate   = 0;
		
		if(net->net[i].down_rate < 0)
			net->net[i].down_rate = 0;
		
		if(net->net[i].up_rate < 0)
			net->net[i].up_rate = 0;
	}

	return net;
}
