/**
 *  @file undo_state.hpp
 *  @copyright defined in eosio/LICENSE.txt
 */

#pragma once

#include <set>    // std::set
#include <map>    // std::map
#include <vector> // std::vector

#include "rocksdb_backend.hpp"

namespace chainrocks {
   /**
    * Implementation of an object with undo state.
    *
    * Holds the current undo state of a particular session.  For
    * example: whenever `start_undo_session` gets called, a new
    * `session` object is returned, and with that an `undo_state`
    * object gets pushed onto the `_undo_stack` signifying that
    * particular session with its own unique `_revision` number. In
    * turn, whenever the current `_state` is modified, these changes
    * get recorded in the `undo_state` object. Then if a user chooses
    * to undo whatever changes they've made, the information is
    * readily available to revert back to the original state of
    * `_state` safely due to this data structure.
    */
   class undo_state {
   public:
      /**
       * Constructor; normal operation.
       */
      undo_state(const boost::filesystem::path& database_dir);

      /**
       * Returns the modified values in this undo state.
       */
      std::map<std::vector<uint8_t>, std::vector<uint8_t>> modified_values();

      /**
       * Returns the removed values in this undo state.
       */
      std::map<std::vector<uint8_t>, std::vector<uint8_t>> removed_values();

      /**
       * Returns the new keys added in this undo state.
       */
      std::set<std::vector<uint8_t>> new_keys();

      /**
       * Returns the revision number of this undo state.
       */
      int64_t& revision();

   private:
      /**
       * Database to hold any modifications made to `_state`.
       */
      rocksdb_backend _db;

      /**
       * The unique revision number held by each `undo_state` object.
       */
      int64_t _revision;
   };
}
