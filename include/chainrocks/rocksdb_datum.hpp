#include <string> // std::string
#include <vector> // std::vector

namespace chainrocks {
   /**
    * 
    */
   class rocksdb_datum {
   public:
      /**
       * 
       */
      rocksdb_datum() = default;

      /**
       * 
       */
      rocksdb_datum(const std::vector<uint8_t>& dat);

      /**
       * 
       */
      rocksdb_datum(const std::string& dat);

      /**
       * 
       */
      operator std::vector<uint8_t>() const;

      /**
       * 
       */
      operator std::string() const;

      /**
       * 
       */
      const std::vector<uint8_t>& data() const;
      
   public:
      /**
       * 
       */
      std::vector<uint8_t> _datum;
   };
}
