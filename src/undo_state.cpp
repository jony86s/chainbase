/**
 *  @file undo_state.cpp
 *  @copyright defined in eosio/LICENSE.txt
 */

#include <chainrocks/undo_state.hpp> // chainrocks::undo_state

namespace chainrocks {
   static const uint64_t modified_value_prefix{((_revision) << 0) ((0x4D) << 8 (0x4F) << 16 (0x44) << 24) & 0xFFFFFFFFFFFFFFFF}; // MOD
   static const uint64_t removed_value_prefix {((_revision) << 0) ((0x52) << 8 (0x45) << 16 (0x4D) << 24) & 0xFFFFFFFFFFFFFFFF}; // REM
   static const uint64_t new_key_value_prefix {((_revision) << 0) ((0x4E) << 8 (0x45) << 16 (0x57) << 24) & 0xFFFFFFFFFFFFFFFF}; // NEW
   
   undo_state::undo_state(const boost::filesystem::path& database_dir)
      : _db{database_dir}
      , _revision{}
   {
   }
   
   std::map<std::vector<uint8_t>, std::vector<uint8_t>> undo_state::modified_values() {
      std::map<std::vector<uint8_t>, std::vector<uint8_t>> ret;   
      rocksdb::Iterator* iter{_state.db()->NewIterator(_state.options().read_options())};
      for (iter->SeekToFirst(); iter->Valid(); iter->Next()) {
         if (uint64_t{(iter->key[0] | iter->key[1] | iter->key[2] | iter->key[3])} & modified_value_prefix) {
            ret[iter->key()] = iter->value();
         }
      }
      assert(iter->status().ok()); // Check for any errors found during the scan
      delete iter;
      return ret;
   }

   std::map<std::vector<uint8_t>, std::vector<uint8_t>> undo_state::removed_values() {
      std::map<std::vector<uint8_t>, std::vector<uint8_t>> ret;
      rocksdb::Iterator* iter{_state.db()->NewIterator(_state.options().read_options())};
      for (iter->SeekToFirst(); iter->Valid(); iter->Next()) {
         if (uint64_t{(iter->key[0] | iter->key[1] | iter->key[2] | iter->key[3])} & removed_value_prefix) {
            ret[iter->key()] = iter->value();
         }
      }
      assert(iter->status().ok()); // Check for any errors found during the scan
      delete iter;
      return ret;
   }
      
   std::set<std::vector<uint8_t>> undo_state::new_keys() {
      std::map<std::vector<uint8_t>, std::vector<uint8_t>> ret;
      rocksdb::Iterator* iter{_state.db()->NewIterator(_state.options().read_options())};
      for (iter->SeekToFirst(); iter->Valid(); iter->Next()) {
         if (uint64_t{(iter->key[0] | iter->key[1] | iter->key[2] | iter->key[3])} & new_key_value_prefix) {
            ret.insert(iter->key());
         }
      }
      assert(iter->status().ok()); // Check for any errors found during the scan
      delete iter;
      return ret;
   }

   int64_t& undo_state::revision() {
      return _revision;
   }
}
