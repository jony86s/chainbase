/**
 *  @file bench_tps.cpp
 *  @copyright defined in eosio/LICENSE.txt
 */

#include <exception> // std::exception|runtime_error
#include <iostream>  // std::cout|flush
#include <memory>    // std::unique_ptr

#include "database_benchmark.hpp" // database_benchmark|window::narrow_window

/**
 * Implementation of the benchmark.
 *
 * The important part is this line:
 * `database_benchmark<chainbase_interface> dt{window::narrow_window};'
 * Here the user is allowed to specify the underlying database backend
 * he/she has implemented. In this case we use the `chainrocks'
 * interface, which connects to the `chainrocks' key-value store
 * backend. After which, for the benchmark's argument, the user shall
 * specify how the benchmark should measured. These options include:
 *
 * 1) `window::expanding_window': Think average over all time spent.
 * 2) `window::narrow_window': Think intervals in between one second.
 * 3) `window::rolling_window': Think moving average.
 *
 * The output of which shall be a `data.csv' file, which can then be
 * fed into the `gnuplot' script provided in the `build/bench/'
 * directory; and there lie the results of the benchmark.
 *
 * Example Usage:
 * ./bench-tps -s42 -n1000000 -w100000 -v1024
 *
 * -s|--seed
 * -n|--num-of-accounts
 * -w|--num-of-swaps
 * -v|--max-value-size (in bytes)
 */
int main(int argc, char** argv) {
    try {
        database_benchmark<chainbase_interface> dt{window::narrow_window, argc, argv};
        dt.execute_benchmark();
    }
    catch(const std::runtime_error& e) {
        std::cout << "`std::runtime_error'\n" << std::flush;
        std::cout << e.what() << '\n' << std::flush;
    }
    catch(const std::exception& e) {
        std::cout << "`std::exception'\n" << std::flush;
        std::cout << e.what() << '\n' << std::flush;
    }
    catch(...) {
        std::cout << "Other\n" << std::flush;
    }

    return 0;
}
