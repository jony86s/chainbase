/**
 *  @file database_benchmark_interface.hpp
 *  @copyright defined in eosio/LICENSE.txt
 */

#pragma once

#include <iostream> // std::cout|flush
#include <limits>   // std::numeric_limits

#include <boost/filesystem.hpp>      // boost::filesystem::unique_path|remove_all
#include <boost/program_options.hpp> // boost::program_options::options_description

#include "common.hpp"         // generated_data|window
#include "generated_data.hpp" // generated_data
#include "system_metrics.hpp" // system_metrics

/**
 * Implementation of the benchmark.
 *
 * The benchmark involves a series of three steps: 1) Generate the
 * specified amount of random data to be used as both data and as what
 * shall be done to said data (note that the random number generator
 * may be seeded for a deterministic re-run of the benchmark).  2) Fill the
 * given database up with the generated data to simulate an
 * environment that already has working data in place to be used.  3)
 * Perform the specified amount of transfers/swaps on the given
 * data. The transfer/swap operation was chosen because of how
 * fundamental the operation is.
 */
template<typename Database>
class database_benchmark {
public:
    /**
     * Constructor; construct an instance of a `database_benchmark';
     * with `window' being the time window in which to gather the
     * results of the benchmark.
     */
    database_benchmark(window window, int argc, char** argv);

    /**
     * Destructor; normal operation.
     */
    ~database_benchmark();

    /**
     * Set the program options for which the user can specify a
     * customizable option.
     */
    void set_program_options(boost::program_options::options_description& cli);

    /**
     * Execute the benchmark, given the criteria provided by the user.
     */
    void execute_benchmark();

private:

    /**
     * Holds the underlying database interface that has been correctly
     * implemented to able to integrate with this benchmark.
     */
    Database _database{boost::filesystem::unique_path()};

    /**
     * Holds the randomly generated data.
     */
    generated_data _gen_data;

    /**
     * Holds the data strucutre that can act upon the system; gathering
     * system metrics if need be.
     */
    system_metrics _system_metrics;

    /**
     * Holds the time window which the user has specified to user.
     */
    window _window;

private:

    /**
     * Helper function to aid in the construction of a benchmark instance.
     */
    void _initial_database_state();

    /**
     * Helper function to aid in the execution of the benchmark.
     */
    void _execution_loop();

    /**
     * Helper function to aid in calculating the expanding window
     * benchmark measurement.
     */
    uint64_t _expanding_window_metric(uint64_t tps);

    /**
     * Helper function to aid in calculating the narrow window
     * benchmark measurement.
     */
    uint64_t _narrow_window_metric(uint64_t tps);

    /**
     * Helper function to aid in calculating the rolling window
     * benchmark measurement.
     */
    uint64_t _rolling_window_metric(uint64_t tps);
};
