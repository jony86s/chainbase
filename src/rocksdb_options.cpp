/**
 *  @file rocksdb_options.cpp
 *  @copyright defined in eosio/LICENSE.txt
 */

#include <chainrocks/rocksdb_options.hpp>

namespace chainrocks {
   rocksdb_options::rocksdb_options() {
      _init_general_options();
      _init_flush_options();
      _init_read_options();
      _init_write_options();
   }

   const rocksdb::Options& rocksdb_options::general_options() const {
      return _general_options;
   }

   const rocksdb::FlushOptions& rocksdb_options::flush_options() const {
      return _flush_options;
   }

   const rocksdb::ReadOptions& rocksdb_options::read_options() const {
      return _read_options;
   }

   const rocksdb::WriteOptions& rocksdb_options::write_options() const {
      return _write_options;
   }
      
   void rocksdb_options::_init_general_options() {
      _general_options.create_if_missing = true;
      _general_options.paranoid_checks = false;
      _general_options.IncreaseParallelism();
      _general_options.OptimizeLevelStyleCompaction();
      _general_options.write_buffer_size = 1ULL*64ULL*1024ULL*1024ULL*1024ULL;
   }

   void rocksdb_options::_init_flush_options()
   {
   }

   void rocksdb_options::_init_read_options()
   {
   }

   void rocksdb_options::_init_write_options() {
      _write_options.disableWAL = true;
   }
}
