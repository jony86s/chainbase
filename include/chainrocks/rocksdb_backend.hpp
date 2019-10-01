/**
 *  @file rocksdb_backend.hpp
 *  @copyright defined in eosio/LICENSE.txt
 */

#pragma once

#include <boost/filesystem.hpp> // boost::filesystem::path

#include <rocksdb/db.h>          // rocksdb::DB
#include <rocksdb/write_batch.h> // rocksdb::WriteBatch

#include <rocksdb_datum.hpp>   // chainrocks::rocksdb_datum
#include <rocksdb_options.hpp> // chainrocks::rocksdb_options

namespace chainrocks {
   /**
    * Implementation of the RocksDB backend wrapper.
    *
    * The data structure representing a RocksDB database itself. It
    * has the ability to introduce/modify (by `put`) new key/value
    * pairs to `_state`, as well as removed them (by `remove`).
    */
   class rocksdb_backend {
   public:
      /**
       * Constructor; normal operation.
       */
      rocksdb_backend(const boost::filesystem::path& database_dir);

      /**
       * Destructor; normal operation.
       */
      ~rocksdb_backend();

      /**
       * Deleted copy, assignment, and move constructors. We do not
       * want the ability to copy the database around (at the moment).
       */
      rocksdb_backend(const rocksdb_backend&) = delete;
      rocksdb_backend& operator= (const rocksdb_backend&) = delete;
      rocksdb_backend(rocksdb_backend&&) = delete;
      rocksdb_backend& operator= (rocksdb_backend&&) = delete;

      /**
       * Returns the underlying pointer to the RocksDB database.
       */
      rocksdb::DB* db();

      /**
       * Returns the object holding the customized options; either
       * specified by the user or according to the RocksDB defaults.
       */
      rocksdb_options& options();

      /**
       * Put a key/value pair into the RocksDB database.
       */
      void put(const rocksdb_datum& key, const rocksdb_datum& value);

      /**
       * Remove a key/value pair in the RocksDB database.
       */
      void remove(const rocksdb_datum& key);

      /**
       * Get a key/value pair in the RocksDB database.
       */
      void get(const rocksdb_datum& key, std::string &value);

      /**
       * Check if said key/value pair exists in the RocksDB
       * database. May optionally store the value of the key/value
       * pair in the `std::string` tmp.
       */
      bool does_key_exist(const rocksdb_datum& key, std::string tmp = {});

      /**
       * Perform a batch write; defferring to realize the operation
       * until `write_batch` has been called.
       */
      void put_batch(const rocksdb_datum& key, const rocksdb_datum& value);

      /**
       * Perform a batch remove; defferring to realize the operation
       * until `write_batch` has been called.
       */
      void remove_batch(const rocksdb_datum& key);

      /**
       * Perform all batch operations; realizing all batch operations.
       */
      void write_batch();

   private:
      /**
       * Pointer to the underlying RocksDB database.
       */
      rocksdb::DB* _databaseman;

      /**
       * Instance of underlying RocksDB object used to track the
       * status of the database after the exeuction of arbitrary
       * operations.
       */
      rocksdb::Status _status;

      /**
       * Necessary object upon which RocksDB uses to perform batch
       * operations.
       */
      rocksdb::WriteBatch _write_batchman;

      /**
       * Instance of the underlying RocksDB options which can be
       * either user-specifed or according to the RocksDB defaults.
       */
      rocksdb_options _options;

      /**
       * Path for which to store the underlying RocksDB database.
       */
      boost::filesystem::path _data_dir;

      /**
       * Helper function to check the status of the RocksDB database
       * after a RocksDB operation is performed.
       */
      inline void _check_status() const;
   };
}
