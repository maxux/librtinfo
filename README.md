# librtinfo

This is a small library (less than 1000 lines of C code) able to build easy-to-use data structures containing system information

# Monitoring

This library can monitor (only Linux is really supported):
- Basic overall cpu usage
- Basic overall memory (ram and swap) usage
- Battery status if any
- Disks usage (bytes read/written, read/write rate)
- System load average
- Network usage (interface, state, speed, current rate, address, ...)
- HDD and CPU tempeature
- Current system time and uptime

# Dependencies

There is no dependencies except default linux headers

# How to use it

First of all, please check [example/librtinfo-test.c](https://github.com/maxux/librtinfo/blob/master/example/librtinfo-test.c)
which shows you how to use library internals.

## Data representation
All data structures are formatted like: `rtinfo_{type}_t` (eg: `rtinfo_memory_t`)
Here is a basic list of available structures:
```c
- rtinfo_cpu_t
    rtinfo_cpu_dev_t
    rtinfo_cpu_time_t
- rtinfo_disk_t
    rtinfo_disk_dev_t
    rtinfo_disk_sectors_t
- rtinfo_memory_t
- rtinfo_loadagv_t
- rtinfo_battery_t
    info_battery_status_t
- rtinfo_network_t
    rtinfo_network_if_t
    rtinfo_network_byte_t
- rtinfo_uptime_t
- rtinfo_temp_cpu_t
- rtinfo_temp_hdd_t
```


## Functions representation

Functions to manage structures are formatted like: `rtinfo_{action}_{type}` (eg: `rtinfo_get_memory`)

Some functions (like cpu, network and disks) require initialization: `rtinfo_init_{type}` (eg: `rtinfo_init_cpu`)

## Arguments

Init functions need an integer as argument and return a pointer to a valid structure.
Getter functions need an pointer to the data structure to use.
`mk` functions need some different arguments, please read `rtinfo.h` for more information.

# Compilation

Just compile your code using `-lrtinfo`

# Bugs

Feel free to sumbit any pull requests to improve this project

# Contributors
Thanks to:
[@nado](https://github.com/nado),
[@bendardenne](https://github.com/bendardenne),
[@wget](https://github.com/wget),
[@zaibon](https://github.com/zaibon)
