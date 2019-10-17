/**
 *  @file system_metrics.hpp
 *  @copyright defined in eosio/LICENSE.txt
 */

#pragma once

/**
 * Implementation of benchmark's system-metric measuring facilities.
 *
 * As of late, only specific metrics for __APPLE__ computers are used.
 *  The current set of the most useful metrics are as follows:
 *  `total_vm_currently_used' (determines how much virtual memory is
 *  currently in use by the system), `total_vm_used_by_proc'
 *  (determines how much virtual memory is currently in use by the
 *  current process), `total_ram_currently_used' (determines how much
 *  RAM is currently in use by the system), `get_cpu_load (determines
 *  the current load the CPU is experiencing)'.
 */
class system_metrics {
public:
    /**
     * Constructor; normal operation.
     */
    system_metrics();

    /**
     * Print the total virtual memory currently used on the machine.
     */
    uint64_t total_vm_usage();
};
