/**
 *  @file rocksdb_datum.cpp
 *  @copyright defined in eosio/LICENSE.txt
 */

#include <chainrocks/rocksdb_datum.hpp>

namespace chainrocks {
    rocksdb_datum::rocksdb_datum(const std::vector<uint8_t>& dat)
        : _datum{dat}
    {
    }

    rocksdb_datum::rocksdb_datum(const std::string& dat)
        : _datum{dat.cbegin(), dat.cend()}
    {
    }

    rocksdb_datum::operator std::vector<uint8_t>() const {
        return _datum;
    }

    rocksdb_datum::operator std::string() const {
        return std::string{_datum.cbegin(), _datum.cend()};
    }

    const std::vector<uint8_t>& rocksdb_datum::data() const {
        return _datum;
    }
}
