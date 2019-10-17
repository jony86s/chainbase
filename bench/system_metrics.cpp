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

#include "system_metrics.hpp"

system_metrics::system_metrics() = default;

uint64_t system_metrics::total_vm_usage() {
#ifdef __APPLE__
    struct xsw_usage vmusage;
    size_t size{sizeof(vmusage)};
    sysctlbyname("vm.swapusage", &vmusage, &size, NULL, 0);
    return vmusage.xsu_used;
#endif // __APPLE__

#ifdef __linux__
    struct sysinfo mem_info;
    sysinfo (&mem_info);
    uint64_t vm_used = (uint64_t)mem_info.totalram - (uint64_t)mem_info.freeram;
    vm_used += (uint64_t)mem_info.totalswap - (uint64_t)mem_info.freeswap;
    vm_used *= (uint64_t)mem_info.mem_unit;
    return vm_used;
#endif // __linux__    
}
