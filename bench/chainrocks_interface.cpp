/**
 *  @file chainrocks_interface.cpp
 *  @copyright defined in eosio/LICENSE.txt
 */

#include <string> // std::string

#include "chainrocks_interface.hpp"
   
chainrocks_interface::chainrocks_interface(const boost::filesystem::path& database_dir)
    : _db{database_dir}
{
}

chainrocks_interface::~chainrocks_interface() = default;
   
void chainrocks_interface::put(uint64_t key, arbitrary_datum value, void* ctx) {
    _db.put_batch(key, value);
}

void chainrocks_interface::swap(const uint64_t rand_account0, const uint64_t rand_account1) {
    std::string string_value0;
    std::string string_value1;

    _db._state.get(rand_account0, string_value0);
    _db._state.get(rand_account1, string_value1);

    chainrocks::rocksdb_datum datum_value0{string_value0};
    chainrocks::rocksdb_datum datum_value1{string_value1};
         
    _db.put_batch(rand_account0, datum_value1);
    _db.put_batch(rand_account1, datum_value0);
}
   
void chainrocks_interface::write() {
    _db.write_batch();
}
