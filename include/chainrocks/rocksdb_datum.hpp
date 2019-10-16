/**
 *  @file rocksdb_datum.hpp
 *  @copyright defined in eosio/LICENSE.txt
 */

#pragma once

#include <string> // std::string
#include <vector> // std::vector

namespace chainrocks {
   /**
    * Implementation of the data abstraction for which
    * `rocksdb_backend` will operate on.
    *
    * Because RocksDB is a general key/value store database we shall
    * make sure that only the type of datum that actually pertains to
    * our use-case is used when storing/retrieving the said datum; a
    * `std::vector<uint8_t>`.
    */
   class rocksdb_datum {
   public:
      /**
       * Constructor; normal operation.
       */
      rocksdb_datum() = default;

      /**
       * Constructor; normal operation.
       */
      rocksdb_datum(const std::vector<uint8_t>& dat);

      /**
       * Constructor; normal operation.
       */
      rocksdb_datum(const std::string& dat);

      /**
       * Returns the `std::vector<uint8_t>` representation of the
       * underlying data.
       */
      operator std::vector<uint8_t>() const;

      /**
       * Returns the `std::string` representation of the underlying
       * data.
       */
      operator std::string() const;

      /**
       * Returns the underlying data.
       */
      const std::vector<uint8_t>& data() const;
      
   public:
      /**
       * The underlying data that RocksDB will operate upon.
       */
      std::vector<uint8_t> _datum;
   };
}
