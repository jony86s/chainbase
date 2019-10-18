/**
 *  @file system_metrics.cpp
 *  @copyright defined in eosio/LICENSE.txt
 */

#ifdef __APPLE__
#include <mach/mach.h>
#include <sys/mount.h>
#include <sys/sysctl.h>
#endif // __APPLE__

#ifdef __linux__
#include "sys/sysinfo.h"
#include "sys/types.h"
#endif // __linux__

#include "common.hpp" // megabyte
#include "system_metrics.hpp"

system_metrics::system_metrics() = default;

uint64_t system_metrics::total_vm_usage() {
#ifdef __APPLE__
    vm_size_t page_size;
    mach_port_t mach_port;
    mach_msg_type_number_t count;
    vm_statistics64_data_t vm_stats;

    mach_port = mach_host_self();
    count = sizeof(vm_stats) / sizeof(natural_t);
    if (KERN_SUCCESS == host_page_size(mach_port, &page_size) &&
        KERN_SUCCESS == host_statistics64(mach_port, HOST_VM_INFO, (host_info64_t)&vm_stats, &count))
    {
        uint64_t used_memory{((uint64_t)vm_stats.active_count +
                              (uint64_t)vm_stats.inactive_count +
                              (uint64_t)vm_stats.wire_count)};
        used_memory *= (uint64_t)page_size;
        return used_memory/megabyte;
    }
    return 0;
#endif // __APPLE__

#ifdef __linux__
    struct sysinfo mem_info;
    sysinfo (&mem_info);
    uint64_t vm_used = (uint64_t)mem_info.totalram - (uint64_t)mem_info.freeram;
    vm_used += (uint64_t)mem_info.totalswap - (uint64_t)mem_info.freeswap;
    vm_used *= (uint64_t)mem_info.mem_unit;
    return vm_used/megabyte;
#endif // __linux__    
}
