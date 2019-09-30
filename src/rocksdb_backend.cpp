/**
 *  @file rocksdb_backend.cpp
 *  @copyright defined in eosio/LICENSE.txt
 */

#include <string>

#include <chainrocks/rocksdb_backend.hpp>

namespace chainrocks {
   rocksdb_backend::rocksdb_backend(const boost::filesystem::path& database_dir)
      : _data_dir{database_dir}
   {
      _status = rocksdb::DB::Open(_options.general_options(), _data_dir.string().c_str(), &_databaseman);
   }

   rocksdb_backend::~rocksdb_backend() {
      _databaseman->Close();
      delete _databaseman;
   }

   rocksdb::DB* rocksdb_backend::db() { return _databaseman; }

   rocksdb_options& rocksdb_backend::options() { return _options; }
      
   void rocksdb_backend::put(const rocksdb_datum& key, const rocksdb_datum& value) {
      _status = _databaseman->Put(_options.write_options(), static_cast<std::string>(key), static_cast<std::string>(value));
   }

   void rocksdb_backend::remove(const rocksdb_datum& key) {
      _status = _databaseman->Delete(_options.write_options(), static_cast<std::string>(key));
   }

   void rocksdb_backend::get(const rocksdb_datum& key, std::string &value) {
      _status = _databaseman->Get(_options.read_options(), static_cast<std::string>(key), &value);
   }

   bool rocksdb_backend::does_key_exist(const rocksdb_datum& key, std::string tmp) {
      bool ret{_databaseman->KeyMayExist(_options.read_options(), static_cast<std::string>(key), &tmp)};
      return ret;
   }

   void rocksdb_backend::put_batch(const rocksdb_datum& key, const rocksdb_datum& value) {
      _status = _write_batchman.Put(static_cast<std::string>(key), static_cast<std::string>(value));
   }

   void rocksdb_backend::remove_batch(const rocksdb_datum& key) {
      _status = _write_batchman.Delete(static_cast<std::string>(key));
   }

   void rocksdb_backend::write_batch() {
      _status = _databaseman->Write(_options.write_options(), &_write_batchman);
   }
}
