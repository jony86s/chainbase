#include <set>    // std::set
#include <map>    // std::map
#include <vector> // std::vector

namespace chainrocks {
   /**
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
       * 
       */
      undo_state();

   private:
      /**
       * Mapping to hold any modifications made to `_state`.
       */
      std::map<std::vector<uint8_t>, std::vector<uint8_t>> _modified_values;

      /**
       * Mapping to hold any removed values from `_state`.
       */
      std::map<std::vector<uint8_t>, std::vector<uint8_t>> _removed_values;

      /**
       * Set representing new keys that have been edded to `_state`.
       */
      std::set<std::vector<uint8_t>> _new_keys;

      /**
       * The unique revision number held by each `undo_state` object.
       * Note that this revision number will never be less than or
       * equal to, to that of the `index` it is associated with.
       */
      int64_t _revision{};
   };
}
