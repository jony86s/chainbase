/**
 *  @file undo_state.cpp
 *  @copyright defined in eosio/LICENSE.txt
 */

#include <chainrocks/undo_state.hpp> // chainrocks::undo_state

namespace chainrocks {
   undo_state::undo_state()
      : _modified_values{}
      , _removed_values{}
      , _new_keys{}
      , _revision{}
   {
   }
   
   std::map<std::vector<uint8_t>, std::vector<uint8_t>>& undo_state::modified_values() {
      return _modified_values;
   }

   std::map<std::vector<uint8_t>, std::vector<uint8_t>>& undo_state::removed_values() {
      return _removed_values;
   }
      
   std::set<std::vector<uint8_t>>& undo_state::new_keys() {
      return _new_keys;
   }

   int64_t& undo_state::revision() {
      return _revision;
   }
}
