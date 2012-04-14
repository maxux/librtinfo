#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <signal.h>
#include <ncurses.h>
#include <rtinfo.h>
#include <inttypes.h>

#define UPDATE_INTERVAL		1000000

int main(void) {
	rtinfo_memory_t memory;
	rtinfo_loadagv_t loadavg;
	rtinfo_cpu_t *cpu;
	
	rtinfo_uptime_t uptime;
	rtinfo_temp_cpu_t temp_cpu;
	rtinfo_temp_hdd_t temp_hdd;
	
	rtinfo_battery_t battery;
	
	rtinfo_network_t *net;
	
	int nbcpu, nbiface, i;
	struct tm * timeinfo;
	
	/* Init Lib */
	printf("[+] Initializing (librtinfo %.2f)...\n", rtinfo_version());
	net = rtinfo_init_network(&nbiface);
	cpu = rtinfo_init_cpu(&nbcpu);

	/* Working */
	/* You should while(...) { here */
	
	/* Pre-reading data */
	printf("[+] Pre-reading data (cpu/network)\n");
	rtinfo_get_cpu(cpu, nbcpu);
	rtinfo_get_network(net, nbiface);

	/* Sleeping */
	printf("[+] Waiting...\n");
	usleep(UPDATE_INTERVAL);
	
	/* Reading CPU */
	printf("[+] New read...\n");
	rtinfo_get_cpu(cpu, nbcpu);
	rtinfo_mk_cpu_usage(cpu, nbcpu);
	
	printf("[+] Data Dump:\n\n");
	for(i = 0; i < nbcpu; i++)
		printf("[ ] CPU %d: %d%%\n", i, cpu[i].usage);
	
	/* Reading Network */
	rtinfo_get_network(net, nbiface);
	rtinfo_mk_network_usage(net, nbiface, UPDATE_INTERVAL / 1000);
	
	printf("[I] ---\n");
	for(i = 0; i < nbiface; i++)
		printf("[ ] Network %d: %-8s | %-15s | %llu bytes/s | %llu bytes/s\n", i, net[i].name, net[i].ip, net[i].up_rate, net[i].down_rate);
	
	/* Reading Memory */
	printf("[I] ---\n");
	if(!rtinfo_get_memory(&memory))
		return 1;
	
	printf("[ ] RAM : Total: %llu ko - Used: %llu ko\n", memory.ram_used, memory.ram_total);
	printf("[ ] SWAP: Total: %llu ko - Free: %llu ko\n", memory.swap_total, memory.swap_free);
	
	/* Reading Load Average */
	printf("[I] ---\n");
	if(!rtinfo_get_loadavg(&loadavg))
		return 1;
	
	printf("[ ] Load Average: %.2f %.2f %.2f\n", loadavg.load[0], loadavg.load[1], loadavg.load[2]);
	
	/* Reading Battery State */
	printf("[I] ---\n");
	/* Use 'NULL' to search dynamically battery present */
	if(!rtinfo_get_battery(&battery, NULL))
		return 1;
	
	printf("[ ] Battery: Load: %d (-1 means an error)\n", battery.load);
	printf("[ ] Battery: Full Now: %u / Charhe Now: %u / Status: %d\n", battery.charge_now, battery.charge_full, battery.status);
	
	printf("[I] ---\n");
	if(!rtinfo_get_uptime(&uptime))
		return 1;
	
	printf("[ ] Uptime: %u seconds\n", uptime.uptime);
	
	/* Reading Time Info */
	timeinfo = rtinfo_get_time();
	printf("[ ] Time  : %02d:%02d:%02d\n", timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec);
	
	printf("[I] ---\n");
	if(!rtinfo_get_temp_cpu(&temp_cpu))
		return 1;
	
	printf("[ ] CoreTemp Average: %d°C\n", temp_cpu.cpu_average);
	printf("[ ] CPU Critical    : %d°C\n", temp_cpu.critical);
	
	printf("[I] ---\n");
	if(!rtinfo_get_temp_hdd(&temp_hdd))
		return 1;
	
	printf("[ ] HDD Temp Average: %u°C (highest: %u°C)\n", temp_hdd.hdd_average, temp_hdd.peak);
	
	/* You should close your while here */
	
	return 0;
}
