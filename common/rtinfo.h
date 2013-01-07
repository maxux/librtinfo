#ifndef __SYSINFO_H
	#define __SYSINFO_H
	
	#define LIBRTINFO_DEBUG			0		/* Enable lib debug message */
	#define LIBRTINFO_HDDTEMP_HOST		"127.0.0.1" 	/* Should always be localhost */
	#define LIBRTINFO_HDDTEMP_PORT		7634
	
	/* System defines */
	#define LIBRTINFO_MEMORY_FILE	"/proc/meminfo"
	#define LIBRTINFO_LOADAVG_FILE	"/proc/loadavg"
	#define LIBRTINFO_CPU_FILE	"/proc/stat"
	#define LIBRTINFO_NET_FILE	"/proc/net/dev"
	#define LIBRTINFO_UPTIME_FILE	"/proc/uptime"
	#define LIBRTINFO_BATTERY_PATH	"/sys/class/power_supply/"
	
	#include <stdint.h>
	
	/* CPU Structures */
	typedef struct rtinfo_cpu_time_t {
		uint64_t time_total;			/* Total CPU Time */
		uint64_t time_idle;			/* Idle CPU Time */
		
	} rtinfo_cpu_node_t;
	
	/* Note: first node (index 0) is the sum of all the CPUs */
	/*       'nbcpu' will always be (real numbers of cpu) + 1 for the sum */
	typedef struct rtinfo_cpu_dev_t {
		unsigned char usage;                  /* CPU usage (in percent) */
		struct rtinfo_cpu_time_t current;     /* Instant time values  */
		struct rtinfo_cpu_time_t previous;    /* Previous time values */
		
	} rtinfo_cpu_dev_t;
	
	typedef struct rtinfo_cpu_t {
		unsigned int nbcpu;              /* CPU count */
		struct rtinfo_cpu_dev_t *dev;    /* CPU array data */
		
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
		DISCHARGING,
		BATTERY_ERROR,
		BATTERY_UNKNOWN
		
	} rtinfo_battery_status_t;
	
	typedef struct rtinfo_battery_t {
		uint32_t charge_full;	/* Battery full charge value (dependent unit) */
		uint32_t charge_now;	/* Battery current charge value (dependent unit) */
		int8_t load;		/* Battery current load (in percent) */
		
		enum info_battery_status_t status;	/* Battery current status */
		
	} rtinfo_battery_t;
	
	/* Network Structures */
	/* Network Byte Struct */
	typedef struct rtinfo_network_byte_t {
		uint64_t up;		/* Bytes transmitted by interface */
		uint64_t down;		/* Bytes received by interface */
		
	} rtinfo_network_byte_t;
	
	/* Interface Struct */
	typedef struct rtinfo_network_if_t {
		char *name;                     /* Interface name */
		struct rtinfo_network_byte_t current;   /* Number of bytes transfered over the interface */
		struct rtinfo_network_byte_t previous;  /* Copy of previous bytes transfered over the interface */
		struct rtinfo_network_byte_t raw;       /* Raw previous (used for x86 limitation bypass) */
		int64_t up_rate;                /* Upload rate (in b/s)    */
		int64_t down_rate;              /* Download rate (in b/s)  */
		char ip[16];                    /* IP Address in char      */
		uint16_t speed;                 /* Link speed in (in Mbps) */
		char enabled;                   /* Flag for enable/disable */
		
	} rtinfo_network_if_t;
	
	/* Global Network Struct */
	typedef struct rtinfo_network_t {
		unsigned int nbiface;              /* Current number of Interfaces */
		unsigned int netcount;             /* Size of the network pointer */
		struct rtinfo_network_if_t *net;   /* Network array data */
		
	} rtinfo_network_t;
	
	/* Uptime Structure */
	typedef struct rtinfo_uptime_t {
		uint32_t uptime;
		
	} rtinfo_uptime_t;
	
	/* System Temperatures */
	typedef struct rtinfo_temp_cpu_t {
		uint16_t critical;
		uint16_t cpu_average;
		
	} rtinfo_temp_cpu_t;
	
	/* HDD Temp */
	typedef struct rtinfo_temp_hdd_t {
		uint16_t peak;
		uint16_t hdd_average;
		
	} rtinfo_temp_hdd_t;

	/* Functions prototypes */
	
	/* Initialize CPU structure (required to use CPU) */
	rtinfo_cpu_t * rtinfo_init_cpu();
	
	/* Update CPU structure */
	rtinfo_cpu_t * rtinfo_get_cpu(rtinfo_cpu_t *cpu);
	
	/* Calculate the CPU Usage in percent for each CPU */
	rtinfo_cpu_t * rtinfo_mk_cpu_usage(rtinfo_cpu_t *cpu);
	
	/* Write structure with current values */
	rtinfo_memory_t   * rtinfo_get_memory(rtinfo_memory_t *memory);
	rtinfo_loadagv_t  * rtinfo_get_loadavg(rtinfo_loadagv_t *load);
	rtinfo_battery_t  * rtinfo_get_battery(rtinfo_battery_t *battery, char *name);
	rtinfo_temp_cpu_t * rtinfo_get_temp_cpu(rtinfo_temp_cpu_t *temp);
	rtinfo_temp_hdd_t * rtinfo_get_temp_hdd(rtinfo_temp_hdd_t *temp);
	
	/* Initialize Network structure (required to use network) */
	rtinfo_network_t * rtinfo_init_network();
	rtinfo_network_t * rtinfo_get_network(rtinfo_network_t *net);
	rtinfo_network_t * rtinfo_get_network_ipv4(rtinfo_network_t *net);
	rtinfo_network_t * rtinfo_mk_network_usage(rtinfo_network_t *net, int timewait);
	
	/* Return a (struct tm) pointer to the current local time */
	struct tm * rtinfo_get_time();
	
	/* Update uptime structure */
	rtinfo_uptime_t * rtinfo_get_uptime(rtinfo_uptime_t *uptime);
	
	float rtinfo_version();
#endif