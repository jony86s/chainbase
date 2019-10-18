/**
 *  @file database_benchmark_driver.hpp
 *  @copyright defined in eosio/LICENSE.txt
 */

#include <boost/program_options.hpp> // boost::program_options::options_description|variables_map

#include "database_benchmark_interface.hpp" // database_benchmark

template<typename Database>
database_benchmark<Database>::database_benchmark(window window, int argc, char** argv)
    : _window{window}
{
    unsigned int seed{};
    uint64_t num_of_accounts{};
    uint64_t num_of_swaps{};
    uint64_t max_value_size{};
    
    boost::program_options::options_description cli{
        "bench-tps; A Base Layer Transactional Database Benchmarking Tool\n" \
        "Usage:\n" \
        "bench-tps -s|--seed 42 \\\n" \
        "          -n|--num-of-accounts 100000 \\\n" \
        "          -w|--num-of-swaps 100000 \\\n" \
        "          -v|--max-value-size 1024 \n"};

    cli.add_options()
        ("seed,s",
         boost::program_options::value<unsigned int>(&seed)->default_value(42),
         "Seed value for the random number generator.")
         
        ("num-of-accounts,n",
         boost::program_options::value<uint64_t>(&num_of_accounts)->default_value(10000),
         "Number of unique individual accounts with a corresponding value; key/value pair.")
         
        ("num-of-swaps,w",
         boost::program_options::value<uint64_t>(&num_of_swaps)->default_value(3000000),
         "Number of swaps to perform during the benchmark.")
         
        ("max-value-size,v",
         boost::program_options::value<uint64_t>(&max_value_size)->default_value(1024),
         "Maximum value byte vector size.")
         
        ("help,h", "Print this help message and exit.");

    boost::program_options::variables_map vmap;
    boost::program_options::store(boost::program_options::parse_command_line(argc, argv, cli), vmap);
    boost::program_options::notify(vmap);
      
    if (vmap.count("help") > 0) {
        cli.print(std::cerr);
        exit(EXIT_SUCCESS);
    }

    _gen_data = generated_data{seed, num_of_accounts, num_of_swaps, max_value_size};
}

template<typename Database>
database_benchmark<Database>::~database_benchmark() = default;

template<typename Database>
void database_benchmark<Database>::execute_benchmark() {
    _initial_database_state();
    _execution_loop();
    loggerman->flush_all();
}

template<typename Database>
void database_benchmark<Database>::_initial_database_state() {
    clockerman->reset_clocker();

    std::cout << "Filling initial database state...\n" << std::flush;
    loggerman->print_progress(1,0);
      
    for (uint64_t i{}; i < _gen_data.num_of_accounts(); ++i) {
        _database.put(_gen_data.accounts()[i], _gen_data.values()[i]);
       
        if (UNLIKELY(clockerman->should_log())) {
            loggerman->print_progress(i, _gen_data.num_of_accounts());
            clockerman->update_clocker();
        }
    }
    _database.write();
    
    loggerman->print_progress(1,1);
    std::cout << "done.\n" << std::flush;
}

template<typename Database>
void database_benchmark<Database>::_execution_loop() {
    clockerman->reset_clocker();
    uint64_t transactions_per_second{};

    std::cout << "Benchmarking...\n" << std::flush;
    loggerman->print_progress(1,0);
      
    for (uint64_t i{}; i < _gen_data.num_of_swaps(); ++i) {
        const uint64_t rand_key0{_gen_data.accounts()[_gen_data.swaps0()[i]]};
        const uint64_t rand_key1{_gen_data.accounts()[_gen_data.swaps1()[i]]};
        
        _database.swap(rand_key0, rand_key1);
       
        if (UNLIKELY(clockerman->should_log())) {
            switch (_window) {
                case window::expanding_window:
                    loggerman->log_tps(_expanding_window_metric(transactions_per_second));
                    loggerman->log_total_vm_usage(_system_metrics.total_vm_usage());
                    clockerman->update_clocker();
                    break;
                case window::narrow_window:
                    loggerman->log_tps(_narrow_window_metric(transactions_per_second));
                    loggerman->log_total_vm_usage(_system_metrics.total_vm_usage());
                    clockerman->update_clocker();
                    transactions_per_second = 0;
                    break;
                case window::rolling_window:
                    loggerman->log_tps(_rolling_window_metric(transactions_per_second));
                    loggerman->log_total_vm_usage(_system_metrics.total_vm_usage());
                    transactions_per_second = 0;
                    clockerman->update_clocker();
                    break;
                default:
                    throw std::runtime_error{"database_benchmark::should_log()"};
                    break;
            }
            loggerman->print_progress(i, _gen_data.num_of_swaps());
        }
      
        transactions_per_second += 2;
    }

    loggerman->print_progress(1,1);
    std::cout << "done.\n" << std::flush;
}

template<typename Database>
uint64_t database_benchmark<Database>::_expanding_window_metric(uint64_t tps) {
    // Total TPS divided by all time spent.
    return tps/clockerman->expanding_window();
}

template<typename Database>
uint64_t database_benchmark<Database>::_narrow_window_metric(uint64_t tps) {
    // TPS within a 1 second time-frame.
    return tps/clockerman->narrow_window();
}

template<typename Database>
uint64_t database_benchmark<Database>::_rolling_window_metric(uint64_t tps) {
    // TODO: Implement this feature.
    // TPS averaged over a given time-frame.
    return clockerman->rolling_window(tps/clockerman->narrow_window());
}
