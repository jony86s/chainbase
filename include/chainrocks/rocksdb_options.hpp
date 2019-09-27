#include <rocksdb/options.h> // rocksdb::Options|FlushOptions|ReadOptions|WriteOptions

namespace chainrocks {
   /**
    * Implementation of the options available to test/tweak in a
    * `rockdb_backend' instanse.
    */
   class rocksdb_options {
   public:
      /**
       * 
       */
      rocksdb_options();

      /**
       * 
       */
      ~rocksdb_options();

      /**
       * 
       */
      rocksdb_options(const rocksdb_options&) = delete;
      rocksdb_options& operator= (const rocksdb_options&) = delete;
      rocksdb_options(rocksdb_options&&) = delete;
      rocksdb_options& operator= (rocksdb_options&&) = delete;

      /**
       * 
       */
      const rocksdb::Options& general_options() const {
         return _general_options;
      }

      /**
       * 
       */
      const rocksdb::FlushOptions& flush_options() const {
         return _flush_options;
      }

      /**
       * 
       */
      const rocksdb::ReadOptions& read_options() const {
         return _read_options;
      }

      /**
       * 
       */
      const rocksdb::WriteOptions& write_options() const {
         return _write_options;
      }

   private:
      /**
       * 
       */
      rocksdb::Options      _general_options;

      /**
       * 
       */
      rocksdb::FlushOptions _flush_options;

      /**
       * 
       */
      rocksdb::ReadOptions  _read_options;

      /**
       * 
       */
      rocksdb::WriteOptions _write_options;

      /**
       * 
       */
      void _init_general_options();

      /**
       * 
       */
      void _init_flush_options();

      /**
       * 
       */
      void _init_read_options();

      /**
       * 
       */
      void _init_write_options();
   };   
}
