/**
 *  @file generated_data.cpp
 *  @copyright defined in eosio/LICENSE.txt
 */

#include <iostream> // std::cout|flush

#include "generated_data.hpp"

generated_data::generated_data(unsigned int seed,
                               uint64_t num_of_accounts,
                               uint64_t num_of_swaps,
                               uint64_t max_value_size)
{
    _def_rand_engine.seed(seed);
    _uniform_int_dist = std::uniform_int_distribution<uint64_t>{0, std::numeric_limits<uint64_t>::max()};
    _num_of_accounts = num_of_accounts;
    _num_of_swaps = num_of_swaps;
    _max_value_size = max_value_size;
    _generate_values();
}

const uint64_t generated_data::num_of_accounts() const {
    return _num_of_accounts;
}

const uint64_t generated_data::num_of_swaps() const {
    return _num_of_swaps;
}

const std::vector<arbitrary_datum>& generated_data::accounts() const {
    return _accounts;
}

const std::vector<arbitrary_datum>& generated_data::values() const {
    return _values;
}

const std::vector<uint64_t>& generated_data::swaps0() const {
    return _swaps0;
}

const std::vector<uint64_t>& generated_data::swaps1() const {
    return _swaps1;
}

void generated_data::_generate_values() {
    clockerman->reset_clocker();

    std::cout << "Generating values...\n" << std::flush;
    loggerman->print_progress(1,0);

    for (uint64_t i{}; i < _num_of_accounts; ++i) {
        uint64_t        account{_uniform_int_dist(_def_rand_engine)};
        arbitrary_datum value{_max_value_size, _uniform_int_dist(_def_rand_engine)%(_max_value_size)};

        // For additional entropy.
        for (uint64_t i{}; i < 8; ++i) {
            value[i] = _uid(_dre)%(_max_value_value+1);
        }
       
        _accounts.push_back(account);
        _values.push_back  (value);

        if (UNLIKELY(clockerman->should_log())) {
            loggerman->print_progress(i, _num_of_accounts);
            clockerman->update_clocker();
        }
    }

    for (uint64_t i{}; i < _num_of_swaps; ++i) {
        _swaps0.push_back(_generate_value()%_num_of_accounts);
        _swaps1.push_back(_generate_value()%_num_of_accounts);

        if (UNLIKELY(clockerman->should_log())) {
            loggerman->print_progress(i, _num_of_accounts);
            clockerman->update_clocker();
        }
    }

    loggerman->print_progress(1,1);
    std::cout << "done.\n" << std::flush;
}

uint64_t generated_data::_generate_value() {
    return _uid(_dre);
}
