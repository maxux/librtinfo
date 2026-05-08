#include "rtinfo.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/types.h>
#include <time.h>
#include <math.h>
#include "misc.h"
#include "rtinfo.h"

//
// MSR Offsets and Masks
//
#define MSR_RAPL_POWER_UNIT      0x606

#define MSR_PKG_RAPL_POWER_LIMIT 0x610
#define MSR_PKG_ENERGY_STATUS    0x611
#define MSR_PKG_POWER_INFO       0x614

#define MSR_ENERGY_STATUS_MASK   0xffffffff

#define RAPL_POWER_UNIT_OFFSET   0x00
#define RAPL_POWER_UNIT_MASK     0x0f
#define RAPL_ENERGY_UNIT_OFFSET  0x08
#define RAPL_ENERGY_UNIT_MASK    0x1f
#define RAPL_TIME_UNIT_OFFSET    0x10
#define RAPL_TIME_UNIT_MASK      0xf

#define RAPL_THERMAL_SPEC_POWER_OFFSET  0x0
#define RAPL_THERMAL_SPEC_POWER_MASK    0x7fff
#define RAPL_MINIMUM_POWER_OFFSET       0x10
#define RAPL_MINIMUM_POWER_MASK         0x7fff
#define RAPL_MAXIMUM_POWER_OFFSET       0x20
#define RAPL_MAXIMUM_POWER_MASK         0x7fff
#define RAPL_MAXIMUM_TIME_WINDOW_OFFSET 0x30
#define RAPL_MAXIMUM_TIME_WINDOW_MASK   0x3f


//
// CPU / Sockets detection and allocation
//
typedef struct cpu_sockets_t {
    // Number of sockets found
    size_t count;
    char **cpu_dev_msr;

} cpu_sockets_t;

static cpu_sockets_t cpu_sockets_detect() {
    char checkpath[256], buffer[64];
    int fd;

    cpu_sockets_t sockets = {
        .count = 0,
        .cpu_dev_msr = NULL,
    };

    // FIXME: Stupid naive way to discover packages IDs
    for(int i = 0; i < 1024; i++) {
        snprintf(checkpath, sizeof(checkpath), "/sys/devices/system/cpu/cpu%d/topology/physical_package_id", i);
        // printf("[+] checking: %s\n", checkpath);

        if((fd = open(checkpath, O_RDONLY)) < 0) {
            // Nothing more to read if the file does not exists
            if(errno == ENOENT)
                return sockets;

            // Unexpected error
            __rtinfo_internal_diep(checkpath);
        }

        if(read(fd, buffer, sizeof(buffer)) < 0)
            __rtinfo_internal_diep(checkpath);

        close(fd);

        // Parse physical_package_id value
        size_t pkgid = strtoul(buffer, NULL, 10);
        if(pkgid + 1 > sockets.count) {
            sockets.count += 1;

            // Link a new physical_package_id to a msr device filename
            if(!(sockets.cpu_dev_msr = (char **) realloc(sockets.cpu_dev_msr, sockets.count * sizeof(char *))))
                __rtinfo_internal_diep("rapl: realloc");

            // Allocate related /dev/cpu/*/msr filename
            snprintf(checkpath, sizeof(checkpath), "/dev/cpu/%d/msr", i);
            if(!(sockets.cpu_dev_msr[pkgid] = strdup(checkpath))) {
                __rtinfo_internal_diep("rapl: strdup");
            }
        }
    }

    return sockets;
}

//
// RAPL Parsed Data
//
typedef struct rapl_pkg_power_info_t {
	double thermal_spec_power;
	double minimum_power;
	double maximum_power;
	double time_window;

} rapl_pkg_power_info_t;

typedef struct rapl_units_t {
	double power_units;
	double energy_units;
	double time_units;

} rapl_units_t;

//
// Main CPU Package Handler
//
typedef struct rapl_pkg_t {
    // MSR device file descriptor
    int fd;
    size_t count;

    // Resolved info
    rapl_pkg_power_info_t info;
    rapl_units_t units;

    // Raw MSR values
    uint64_t raw_units;
    uint64_t raw_pwrinfo;
    uint64_t raw_energy;

    double previous_measure;
    double current_measure;
    struct timespec previous_clock;
    struct timespec current_clock;

} rapl_pkg_t;

//
// RAPL Parsing and Debug
//
static rapl_units_t *rapl_parse_units(uint64_t raw, rapl_units_t *ru) {
	ru->power_units = pow(0.5, (double) ((raw >> RAPL_POWER_UNIT_OFFSET) & RAPL_POWER_UNIT_MASK));

	ru->energy_units = pow(0.5, (double) ((raw >> RAPL_ENERGY_UNIT_OFFSET) & RAPL_ENERGY_UNIT_MASK));

	ru->time_units = pow(0.5, (double) ((raw >> RAPL_TIME_UNIT_OFFSET) & RAPL_TIME_UNIT_MASK));

    return ru;
}

static rapl_pkg_power_info_t *rapl_parse_power_info(uint64_t raw, rapl_units_t *ru, rapl_pkg_power_info_t *pinfo) {
    pinfo->thermal_spec_power = (double) ((raw >> RAPL_THERMAL_SPEC_POWER_OFFSET) & RAPL_THERMAL_SPEC_POWER_MASK);
    pinfo->thermal_spec_power = ru->power_units * pinfo->thermal_spec_power;

    pinfo->minimum_power = (double) ((raw >> RAPL_MINIMUM_POWER_OFFSET) & RAPL_MINIMUM_POWER_MASK);
    pinfo->minimum_power = ru->power_units * pinfo->minimum_power;

    pinfo->maximum_power = (double) ((raw >> RAPL_MAXIMUM_POWER_OFFSET) & RAPL_MAXIMUM_POWER_MASK);
    pinfo->maximum_power = ru->power_units * pinfo->maximum_power;

    pinfo->time_window = (double) ((raw >> RAPL_MAXIMUM_TIME_WINDOW_OFFSET) & RAPL_MAXIMUM_TIME_WINDOW_MASK);
    pinfo->time_window = ru->time_units * pinfo->time_window;

    return pinfo;
}

static void rapl_dump_units(rapl_units_t *ru, rapl_pkg_power_info_t *pinfo) {
    rtinfo_debug("[=] ---------------------------------\n");

	rtinfo_debug("[.] power units: %.3f watt\n", ru->power_units);
	rtinfo_debug("[.] energy units: %.8f jouls\n", ru->energy_units);
	rtinfo_debug("[.] time units: %.8f seconds\n", ru->time_units);

	rtinfo_debug("[.] package thermal spec: %.3f watt\n", pinfo->thermal_spec_power);
	rtinfo_debug("[.] package minimum power: %.3f watt\n", pinfo->minimum_power);
	rtinfo_debug("[.] package maximum power: %.3f watt\n", pinfo->maximum_power);
	rtinfo_debug("[.] package maximum time window: %.3f seconds\n", pinfo->time_window);

    rtinfo_debug("[=] ---------------------------------\n");
}

//
// Time Helpers
//
static double timespec_diff(const struct timespec *end, const struct timespec *begin) {
    return (end->tv_sec - begin->tv_sec) + (end->tv_nsec - begin->tv_nsec) / 1000000000.0;
}

//
// MSR Helpers
//
static uint64_t *msr_read_raw(int fd, size_t offset, uint64_t *value) {
    ssize_t rlen;

    if((rlen = pread(fd, value, sizeof(uint64_t), offset)) != sizeof(uint64_t)) {
        rtinfo_perror("pread");
        return NULL;
    }

    return value;
}

// FIXME: memory not cleaned on allocation error
rtinfo_rapl_t *rtinfo_init_rapl_intel() {
    rtinfo_rapl_t *rapl;
    rapl_pkg_t **pkgs;

    // Allocate root object
    if(!(rapl = calloc(1, sizeof(rtinfo_rapl_t)))) {
        // diep("calloc: rtinfo_rapl_t");
        return NULL;
    }

    // Detect available CPU Packages on the host
    cpu_sockets_t sockets = cpu_sockets_detect();

    if(sockets.count == 0) {
        rtinfo_debug("[-] could not detect cpu packages");
        return NULL;
    }

    rapl->zones = sockets.count;
    if(!(rapl->pkgs = calloc(rapl->zones, sizeof(double)))) {
        // diep("calloc: rapl: zones");
        return NULL;
    }

    // Allocate list of file descriptors to open MSR device
    if(!(pkgs = calloc(sockets.count, sizeof(rapl_pkg_t *)))) {
        // diep("calloc: rapl_pkg_t list");
        return NULL;
    }

    // Initializing internal Packages
    for(size_t pkgid = 0; pkgid < sockets.count; pkgid++) {
        rtinfo_debug("[+] package found: %lu: %s\n", pkgid, sockets.cpu_dev_msr[pkgid]);

        if(!(pkgs[pkgid] = calloc(1, sizeof(rapl_pkg_t)))) {
            // diep("calloc: rapl_pkg_t");
            return NULL;
        }

        rapl_pkg_t *pkg = pkgs[pkgid];

        // Open a file descriptor on the CPU Package MSR device
        if((pkg->fd = open(sockets.cpu_dev_msr[pkgid], O_RDONLY)) < 0) {
            // diepi("open", sockets.cpu_dev_msr[pkgid]);
            return NULL;
        }


        // Fetching internal Power Units
        if(!msr_read_raw(pkg->fd, MSR_RAPL_POWER_UNIT, &pkg->raw_units)) {
            // diep("msr_read_raw: MSR_RAPL_POWER_UNIT");
            return NULL;
        }

        // Fetching internal Power Info
        if(!msr_read_raw(pkg->fd, MSR_PKG_POWER_INFO, &pkg->raw_pwrinfo)) {
            // diep("msr_read_raw: MSR_PKG_POWER_INFO");
            return NULL;
        }

        // Parsing and dumping data
        rapl_parse_units(pkg->raw_units, &pkg->units);
        rapl_parse_power_info(pkg->raw_pwrinfo, &pkg->units, &pkg->info);

        rapl_dump_units(&pkg->units, &pkg->info);
    }

    for(size_t i = 0; i < rapl->zones; i++) {
        free(sockets.cpu_dev_msr[i]);
    }

    free(sockets.cpu_dev_msr);

    rapl->kntxt = (void **) pkgs;

    return rapl;
}

rtinfo_rapl_t *rtinfo_get_rapl_intel(rtinfo_rapl_t *rapl) {
    if(!rapl)
        return NULL;

    rapl_pkg_t **pkgs = (rapl_pkg_t **)  rapl->kntxt;

    // Continuous Power Reading
    for(size_t pkgid = 0; pkgid < rapl->zones; pkgid++) {
        rapl_pkg_t *pkg = pkgs[pkgid];

        pkg->previous_measure = pkg->current_measure;
        pkg->previous_clock = pkg->current_clock;

        if(!msr_read_raw(pkg->fd, MSR_PKG_ENERGY_STATUS, &pkg->raw_energy)) {
            // diep("msr_read_raw: MSR_PKG_ENERGY_STATUS");
            continue;
        }

        if(clock_gettime(CLOCK_MONOTONIC, &pkg->current_clock) < 0) {
            // diep("clock_gettime");
            continue;
        }

        pkg->current_measure = (double) (pkg->raw_energy & MSR_ENERGY_STATUS_MASK);
        pkg->current_measure = (pkg->current_measure * pkg->units.energy_units);
    }

    return rapl;
}

rtinfo_rapl_t *rtinfo_mk_rapl_usage(rtinfo_rapl_t *rapl) {
    if(!rapl)
        return NULL;

    rapl_pkg_t **pkgs = (rapl_pkg_t **)  rapl->kntxt;

    // Continuous Power Reading
    for(size_t pkgid = 0; pkgid < rapl->zones; pkgid++) {
        rapl_pkg_t *pkg = pkgs[pkgid];

        double timediff = timespec_diff(&pkg->current_clock, &pkg->previous_clock);
        rapl->pkgs[pkgid] = (pkg->current_measure - pkg->previous_measure);
        rapl->pkgs[pkgid] = rapl->pkgs[pkgid] / timediff;
    }

    return rapl;
}

void rtinfo_free_rapl(rtinfo_rapl_t *rapl) {
    if(!rapl)
        return;

    rapl_pkg_t **pkgs = (rapl_pkg_t **) rapl->kntxt;

    for(size_t i = 0; i < rapl->zones; i++) {
        close(pkgs[i]->fd);
        free(pkgs[i]);
    }

    free(rapl->pkgs);
    free(rapl->kntxt);
    free(rapl);
}
