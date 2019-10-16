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
    : _prev_total_ticks{}
    , _prev_idle_ticks{}
{
}

void system_metrics::total_vm() {
#ifdef __APPLE__
    struct statfs my_stats;
    if (statfs("/", &my_stats) == KERN_SUCCESS) {
        std::cout << "my_stats.f_bsize: " << my_stats.f_bsize << '\n';
        std::cout << "my_stats.f_bfree: " << my_stats.f_bfree << '\n';
        std::cout << "my_stats.f_bsize*stats.f_bfree: " << (my_stats.f_bsize * my_stats.f_bfree) << '\n';
    }
#endif // __APPLE__

#ifdef __linux__
// ...
#endif // __linux__
}

void system_metrics::total_vm_used_by_proc() {
#ifdef __APPLE__
    struct task_basic_info my_task_info;
    mach_msg_type_number_t my_task_info_count = TASK_BASIC_INFO_COUNT;
    if (task_info(mach_task_self(),
                  TASK_BASIC_INFO,
                  reinterpret_cast<task_info_t>(&my_task_info),
                  &my_task_info_count) == KERN_SUCCESS)
    {
        std::cout << "my_task_info.virtual_size: "  << my_task_info.virtual_size  << '\n';
        std::cout << "my_task_info.resident_size: " << my_task_info.resident_size << '\n';
    }
#endif // __APPLE__

#ifdef __linux__
// ...
#endif // __linux__
}

uint64_t system_metrics::total_vm_usage() {
#ifdef __APPLE__
    struct xsw_usage my_vmusage;
    size_t size{sizeof(my_vmusage)};
    if (sysctlbyname("vm.swapusage", &my_vmusage, &size, NULL, 0) == KERN_SUCCESS) {
        std::cout << "my_vmusage.xsu_total: " << my_vmusage.xsu_total << '\n';
        std::cout << "my_vmusage.xsu_avail: " << my_vmusage.xsu_avail << '\n';
        std::cout << "my_vmusage.xsu_used: "  << my_vmusage.xsu_used  << '\n';
    }
#endif // __APPLE__

#ifdef __linux__
    // More accurate to pass back:
    // Current Total Physical RAM Usage + (Total VM Usage - Current Total Physical RAM Usage)
    
    struct sysinfo mem_info;
    sysinfo (&mem_info);
    
    uint64_t vm_used = (uint64_t)mem_info.totalram - (uint64_t)mem_info.freeram;

    //Add other values in next statement to avoid int overflow on right hand side.
    vm_used += (uint64_t)mem_info.totalswap - (uint64_t)mem_info.freeswap;
    vm_used *= (uint64_t)mem_info.mem_unit;

    return vm_used;
#endif // __linux__    
}

uint64_t system_metrics::total_ram() {
#ifdef __APPLE__
    int management_information_base[2]{CTL_HW, HW_MEMSIZE};
    size_t ram;
    size_t size = sizeof(uint64_t);
    if (sysctl(management_information_base, 2, &ram, &size, NULL, 0) == KERN_SUCCESS) {
        return ram;
    }
    else {
        return 0;
    }
#endif // __APPLE__

#ifdef __linux__
// ...
#endif // __linux__
}

double system_metrics::total_ram_currently_used() {
#ifdef __APPLE__
    vm_size_t page_size;
    vm_statistics64_data_t vm_stats;
    mach_port_t mach_port{mach_host_self()};
    mach_msg_type_number_t count{sizeof(vm_stats) / sizeof(natural_t)};

    if (host_page_size(mach_port, &page_size) == KERN_SUCCESS &&
        host_statistics64(mach_port, HOST_VM_INFO, reinterpret_cast<host_info64_t>(&vm_stats), &count) == KERN_SUCCESS)
    {
        uint64_t used_memory{(vm_stats.active_count   +
                            vm_stats.inactive_count +
                            vm_stats.wire_count)    * page_size};
        return (static_cast<double>(used_memory) / total_ram());
    }
    else {
        return 0;
    }
#endif // __APPLE__

#ifdef __linux__
// ...
#endif // __linux__
}
