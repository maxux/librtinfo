#ifndef __SYSINFO_H
	#define __SYSINFO_H
	
	/* User defines */
	#define BATTERY_NAME		"BAT1"
	#define UPDATE_INTERVAL		1000000
	
	/* System defines */
	#define MEMORY_FILE	"/proc/meminfo"
	#define LOADAVG_FILE	"/proc/loadavg"
	#define CPU_FILE	"/proc/stat"
	#define NET_FILE	"/proc/net/dev"
	#define BATTERY_PATH	"/sys/class/power_supply/" BATTERY_NAME
	
	#include <stdint.h>
	
	/* CPU Structures */
	typedef struct rtinfo_cpu_node_t {
		uint64_t time_total;			/* Total CPU Time */
		uint64_t time_idle;			/* Idle CPU Time */
		
	} rtinfo_cpu_node_t;
	
	/* Note: first node (index 0) is the sum of all the CPUs */
	/*       'nbcpu' will always be (real numbers of cpu) + 1 for the sum */
	typedef struct rtinfo_cpu_t {
		unsigned char usage;			/* CPU Usage (in percent) */
		struct rtinfo_cpu_node_t current	__attribute__ ((packed));	/* Instant time values */
		struct rtinfo_cpu_node_t previous	__attribute__ ((packed));	/* Previous time values */
		
	} rtinfo_cpu_t;
	
	/* Memory (RAM/SWAP) Structure */
	typedef struct rtinfo_memory_t {
		uint64_t ram_total;		/* RAM Total (in kB) */
		uint64_t ram_used;		/* RAM Used [aka -/+ buffers/cache] (in kB) */
		uint64_t swap_total;		/* SWAP Total (in kB) */
		uint64_t swap_free;		/* SWAP Free (in kB) */
		
	} rtinfo_memory_t;
	
	/* Load Average Structure */
	typedef struct rtinfo_loadagv_t {
		float load[3];		/* Load average [0-3] respectivly for 1, 5 and 15 min ago */
		
	} rtinfo_loadagv_t;
	
	/* Battery Structure */
	typedef enum info_battery_status_t {
		FULL,
		CHARGING,
		DISCHARGING
		
	} rtinfo_battery_status_t;
	
	typedef struct rtinfo_battery_t {
		uint32_t charge_full;	/* Battery full charge value (dependent unit) */
		uint32_t charge_now;	/* Battery current charge value (dependent unit) */
		char load;		/* Battery current load (in percent) */
		
		enum info_battery_status_t status;	/* Battery current status */
		
	} rtinfo_battery_t;
	
	/* Network Structures */
	typedef struct rtinfo_network_node_t {
		uint64_t up;		/* Bytes transmitted by interface */
		uint64_t down;		/* Bytes received by interface */
		
	} rtinfo_network_node_t;
	
	typedef struct rtinfo_network_t {
		char *name;		/* Interface name */
		struct rtinfo_network_node_t current;	/* Number of bytes transfered over the interface */
		struct rtinfo_network_node_t previous;	/* Copy of previous bytes transfered over the interface */
		int64_t up_rate;		/* Upload rate (in b/s) */
		int64_t down_rate;		/* Download rate (in b/s) */
		char ip[16];		/* IP Address in char */
		
	} rtinfo_network_t;

	typedef struct rtinfo_network_legacy_t {
		char name[32];		/* Interface name */
		struct rtinfo_network_node_t current;	/* Number of bytes transfered over the interface */
		int64_t up_rate;		/* Upload rate (in b/s) */
		int64_t down_rate;		/* Download rate (in b/s) */
		char ip[16];		/* IP Address in char */
		
	} rtinfo_network_legacy_t;

	/* Functions prototypes */
	/* Initialize CPU structure (required to use CPU) */
	rtinfo_cpu_t * rtinfo_init_cpu(int *nbcpu);
	
	/* Update CPU structure */
	rtinfo_cpu_t * rtinfo_get_cpu(rtinfo_cpu_t *cpu, int nbcpu);
	
	/* Calculate the CPU Usage in percent for each CPU */
	rtinfo_cpu_t * rtinfo_mk_cpu_usage(rtinfo_cpu_t *cpu, int nbcpu);
	
	/* Write structure with current values */
	rtinfo_memory_t * rtinfo_get_memory(rtinfo_memory_t *memory);
	rtinfo_loadagv_t * rtinfo_get_loadavg(rtinfo_loadagv_t *load);
	rtinfo_battery_t * rtinfo_get_battery(rtinfo_battery_t *battery);
	
	/* Initialize Network structure (required to use network) */
	rtinfo_network_t * rtinfo_init_network(int *nbiface);
	rtinfo_network_t * rtinfo_get_network(rtinfo_network_t *net, int nbiface);
	rtinfo_network_t * rtinfo_get_network_ipv4(rtinfo_network_t *net, int nbiface);
	rtinfo_network_t * rtinfo_mk_network_usage(rtinfo_network_t *net, int nbiface, int timewait);
	
	/* Return a (struct tm) pointer to the current local time */
	struct tm * rtinfo_get_time();
	
	float rtinfo_version();
#endif
