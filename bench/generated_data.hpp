/**
 *  @file generated_data.hpp
 *  @copyright defined in eosio/LICENSE.txt
 */

#pragma once

#include <random> // std::default_random_engine|uniform_int_distribution
#include <vector> // std::vector

#include "common.hpp" // arbitrary_datum

/**
 * Implementation of benchmark's random data generation facility.
 *
 * Random numbers are generated with a uniform distribution by using
 * the standard's random number generation facilities (this is subject
 * to be cusomizable in the future). It's important to note here, that
 * the user is able to choose how large or small he/she wishes the
 * keys/values to be. But not only can the size be specified, but the
 * filler value can also be specified. The filler value is important
 * to note and must have an appropriate degree of randomness to it. If
 * this weren't so, some database implementations may take advantage
 * of the lack of entropy.
 */
class generated_data {
public:
    /**
     * Used to generate the randomly generated data for the benchmark
     * if it has not yet done so via the constructor.
     */
    generated_data(unsigned int seed,
                   uint64_t num_of_accounts,
                   uint64_t num_of_swaps,
                   uint64_t max_value_size);

    /**
     * Return the number of accounts in the accounts vector.
     */
    const uint64_t num_of_accounts() const;
   
    /**
     * Return the number of swaps in the swaps vector.
     */
    const uint64_t num_of_swaps() const;

    /**
     * Return the value for any one value of a key/value pair.
     */
    const uint64_t max_value_size() const;

    /**
     * Return a reference to the vector holding the randomly generated
     * accounts (`arbitrary_datum').
     */
    const std::vector<arbitrary_datum>& accounts() const;

    /**
     * Return a reference to the vector holding the randomly generated
     * values (`arbitrary_datum').
     */
    const std::vector<arbitrary_datum>& values() const;

    /**
     * Return a reference to the vector holding the randomly generated
     * values (`arbitrary_datum').
     */
    const std::vector<uint64_t>& swaps0() const;

    /**
     * Return a reference to the vector holding the randomly generated
     * values (`arbitrary_datum').
     */
    const std::vector<uint64_t>& swaps1() const;

private:
    /**
     * Holds the random number to generate the arbitrary data for the
     * benchmark.
     */
    std::default_random_engine _def_rand_engine;

    /**
     * Holds the distribution used for generating the random data.
     * TODO: Make this a customizable user option in the future. 
     */
    std::uniform_int_distribution<uint64_t> _uniform_int_dist;

    /**
     * Holds the number of accounts the benchmark will work with.
     */
    uint64_t _num_of_accounts;

    /**
     * Holds the number of swap operations the benchmark will work
     * with.
     */
    uint64_t _num_of_swaps;

    /**
     * Holds the maximum value length (in bytes) that the benchmark
     * will work with.
     */
    uint64_t _max_value_size;

    /**
     * Holds the `arbitrary_data' relating to accounts.
     */
    std::vector<uint64_t> _accounts;

    /**
     * Holds the `arbitrary_data' relating to values.
     */
    std::vector<arbitrary_datum> _values;

    /**
     * Holds the indices deciding which first account to swap.
     * TODO: Combine the two swap vectors.
     */
    std::vector<uint64_t> _swaps0;

    /**
     * Holds the indices deciding which second account to swap.
     * TODO: Combine the two swap vectors.
     */
    std::vector<uint64_t> _swaps1;

private:

    /**
     * Helper function for generating the random data.
     */
    void _generate_values();

    /**
     * Helper function for generating the random data, individually.
     */
    uint64_t _generate_value();
};
