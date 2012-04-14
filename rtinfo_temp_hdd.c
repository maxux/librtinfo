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
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include "misc.h"
#include "rtinfo.h"

int __rtinfo_internal_hddtemp_connect() {
	int sockfd = -1, connresult;
	struct sockaddr_in server_addr;
	struct hostent *he;
	
	/* Resolving name */
	if((he = gethostbyname(LIBRTINFO_HDDTEMP_HOST)) == NULL)
		perror("[-] hddtemp gethostbyname");
	
	bcopy(he->h_addr, &server_addr.sin_addr, he->h_length);

	server_addr.sin_family = AF_INET; 
	server_addr.sin_port = htons(LIBRTINFO_HDDTEMP_PORT);

	/* Creating Socket */
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0)
		perror("[-] hddtemp socket");

	/* Init Connection */
	connresult = connect(sockfd, (struct sockaddr *) &server_addr, sizeof(server_addr));
	
	if(connresult < 0)
		perror("[-] hddtemp connect");
	
	return sockfd;
}

uint16_t __rtinfo_internal_hddtemp_parse(char *buffer, unsigned int *peak) {
	unsigned int value = 0, this;
	unsigned int disks = 0, skip = 0;
	char *str = buffer;
	
	if(*buffer != '|') {
		if(LIBRTINFO_DEBUG)
			printf("[-] librtinfo hddtemp parser: wrong data\n");
			
		return 0;
	}
	
	while(*str) {
		if(*str == '|')
			skip++;
		
		str++;
	
		if(skip == 3) {
			/* Ignoring UNK and ??? response */
			if(strncmp(str, "???", 3) && strncmp(str, "UNK", 3)) {
				this = atoi(str);
				
				if(this > *peak)
					*peak = this;
					
				value += this;
				disks++;
				
			} else if(LIBRTINFO_DEBUG) printf("[-] librtinfo hddtemp parser: error: %s\n", str);
			
			if(!(str = strstr(str, "||")))
				break;
			
			str += 1;	/* Skip double pipe (||) */
			skip = 0;
		}
	}
	
	return (disks > 0) ? value / disks : 0;
}

rtinfo_temp_hdd_t * rtinfo_get_temp_hdd(rtinfo_temp_hdd_t *hddtemp) {
	int sockfd;
	unsigned int peak = 0;
	char buffer[1024];
	size_t rlen;
	
	if((sockfd = __rtinfo_internal_hddtemp_connect()) < 0) {
		hddtemp->hdd_average = 0;
		return hddtemp;
	}
	
	rlen = recv(sockfd, buffer, sizeof(buffer), MSG_WAITALL);
	buffer[rlen] = '\0';
	
	if(LIBRTINFO_DEBUG)
		printf("[+] librtinfo: hddtemp: %s\n", buffer);
	
	hddtemp->hdd_average = __rtinfo_internal_hddtemp_parse(buffer, &peak);
	hddtemp->peak 	     = peak;
	
	close(sockfd);
	
	return hddtemp;
}
