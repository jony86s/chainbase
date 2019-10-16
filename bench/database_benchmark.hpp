/**
 *  @file database_benchmark.hpp
 *  @copyright defined in eosio/LICENSE.txt
 */

#pragma once

#include "chainbase_interface.hpp"       // chainbase_interface
#include "chainrocks_interface.hpp"      // chainrocks_interface
#include "clocker.hpp"                   // clocker
#include "common.hpp"                    // window::narrow
#include "database_benchmark_driver.hpp" // database_benchmark
#include "logger.hpp"                    // logger

/**
 * Clocking facility.
 */
std::unique_ptr<clocker> clockerman = std::make_unique<clocker>(1);

/**
 * Logging facility.
 */
std::unique_ptr<logger> loggerman = std::make_unique<logger>();
