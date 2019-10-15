/**
 *  @file system_metrics.cpp
 *  @copyright defined in eosio/LICENSE.txt
 */

#include <iostream> // std::cout

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

system_metrics::system_metrics()
{
}

size_t system_metrics::total_vm_usage() {
#ifdef __APPLE__
#endif // __APPLE__

#ifdef __linux__
    // More accurate to pass back:
    // Current Total Physical RAM Usage + (Total VM Usage - Current Total Physical RAM Usage)
    
    struct sysinfo mem_info;
    sysinfo (&mem_info);
   
    size_t vm_used = (size_t)mem_info.totalram - (size_t)mem_info.freeram;
    
    //Add other values in next statement to avoid int overflow on right hand side.
    vm_used += (size_t)mem_info.totalswap - (size_t)mem_info.freeswap;
    vm_used *= (size_t)mem_info.mem_unit;

    return vm_used;
#endif // __linux__
}
