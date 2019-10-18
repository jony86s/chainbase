/**
 *  @file rocksdb_options.hpp
 *  @copyright defined in eosio/LICENSE.txt
 */

#pragma once

#include <rocksdb/options.h> // rocksdb::Options|FlushOptions|ReadOptions|WriteOptions

namespace chainrocks {
    /**
     * Implementation of the options available to test/tweak in a
     * `rockdb_backend` instance.
     */
    class rocksdb_options {
    public:
        /**
         * Constructor; normal operation.
         */
        rocksdb_options();

        /**
         * Destructor; normal operation.
         */
        ~rocksdb_options() = default;

        /**
         * Deleted copy, assignment, and move constructors. We do not
         * want the ability to copy the options around.
         */
        rocksdb_options(const rocksdb_options&) = delete;
        rocksdb_options& operator= (const rocksdb_options&) = delete;
        rocksdb_options(rocksdb_options&&) = delete;
        rocksdb_options& operator= (rocksdb_options&&) = delete;

        /**
         * Returns the general options of RocksDB.
         */
        const rocksdb::Options& general_options() const;

        /**
         * Returns the flush options of RocksDB.
         */
        const rocksdb::FlushOptions& flush_options() const;

        /**
         * Returns the read options of RocksDB.
         */
        const rocksdb::ReadOptions& read_options() const;

        /**
         * Returns the write options of RocksDB.
         */
        const rocksdb::WriteOptions& write_options() const;

    private:
        /**
         * General options of RocksDB.
         */
        rocksdb::Options _general_options;

        /**
         * Flush options of RocksDB.
         */
        rocksdb::FlushOptions _flush_options;

        /**
         * Read options of RocksDB.
         */
        rocksdb::ReadOptions _read_options;

        /**
         * Write options of RocksDB.
         */
        rocksdb::WriteOptions _write_options;

        /**
         * Helper function to initialize the general options of RocksDB.
         */
        void _init_general_options();

        /**
         * Helper function to initialize the flush options of RocksDB.
         */
        void _init_flush_options();

        /**
         * Helper function to initialize the read options of RocksDB.
         */
        void _init_read_options();

        /**
         * Helper function to initialize the write options of RocksDB.
         */
        void _init_write_options();
    };   
}
