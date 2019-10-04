/**
 *  @file common.hpp
 *  @copyright defined in eosio/LICENSE.txt
 */

#pragma once

#include <optional>

/**
 * Type alias to refer to the nature of the type of data this
 * key/value store implementation is dealing with.
 */
using byte_vector = std::optional<std::vector<uint8_t>>;
