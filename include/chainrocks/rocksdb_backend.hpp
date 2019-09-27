#include <boost/filesystem.hpp> // boost::filesystem::path

#include <rocksdb/db.h>          // rocksdb::DB
#include <rocksdb/write_batch.h> // rocksdb::WriteBatch

#include "rocksdb_datum.hpp"   // chainrocks::rocksdb_datum
#include "rocksdb_options.hpp" // chainrocks::rocksdb_options

namespace chainrocks {
   /**
    * The data structure representing a RocksDB database itself. It
    * has the ability to introduce/modify (by `put`) new key/value
    * pairs to `_state`, as well as removed them (by `remove`).
    */
   class rocksdb_backend {
   public:
      /**
       * 
       */
      rocksdb_backend(const boost::filesystem::path& database_dir);

      /**
       * 
       */
      ~rocksdb_backend();

      /**
       * 
       */
      rocksdb_backend(const rocksdb_backend&) = delete;
      rocksdb_backend& operator= (const rocksdb_backend&) = delete;
      rocksdb_backend(rocksdb_backend&&) = delete;
      rocksdb_backend& operator= (rocksdb_backend&&) = delete;

      /**
       * 
       */
      rocksdb::DB* db();

      /**
       * 
       */
      rocksdb_options& options();

      /**
       * 
       */
      void put(const rocksdb_datum& key, const rocksdb_datum& value);

      /**
       * 
       */
      void remove(const rocksdb_datum& key);

      /**
       * 
       */
      void get(const rocksdb_datum& key, std::string &value);

      /**
       * 
       */
      bool does_key_exist(const rocksdb_datum& key, std::string tmp = {});

      /**
       * 
       */
      void put_batch(const rocksdb_datum& key, const rocksdb_datum& value);

      /**
       * 
       */
      void remove_batch(const rocksdb_datum& key);

      /**
       * 
       */
      void write_batch();

   private:
      /**
       * 
       */
      rocksdb::DB* _databaseman;

      /**
       * 
       */
      rocksdb::WriteBatch _write_batchman;

      /**
       * 
       */
      rocksdb_options _options;

      /**
       * 
       */
      boost::filesystem::path _data_dir;

      /**
       * 
       */
      inline void _check_status() const;
   };
}
