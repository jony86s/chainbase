#include <deque>  // std::deque
#include <vector> // std::vector

#include <boost/filesystem.hpp> // boost::filesystem::path

#include "rocksdb_backend.hpp" // chainrocks::rocksdb_backend
#include "undo_state.hpp"      // chainrocks::undo_state

namespace chainrocks {
   /**
    *
    */
   class database {
   public:
      /**
       *
       */
      database(const boost::filesystem::path& database_dir);

      /**
       *
       */
      ~database();

      /**
       *
       */
      database(const database&) = delete;
      database& operator= (const database&) = delete;
      database(database&&) = delete;
      database& operator= (database&&) = delete;

      /**
       *
       */
      const auto& state() const;

      /**
       *
       */
      const auto& stack() const;

      /**
       * Add a new value to `_state` or modify an existing value.
       */
      void put(const std::vector<uint8_t>& key, const std::vector<uint8_t>& value);

      /**
       * Remove a value from `_state`.
       */
      void remove(const std::vector<uint8_t>& key);

      /**
       * Get a value from `_state`.
       */
      void get(const std::vector<uint8_t> key, std::string &value);

      /**
       * Check if a specific key exists in the database.
       */
      bool does_key_exist(const std::vector<uint8_t> key, std::string tmp = {});

      /**
       * Writes to batchman.
       */
      void put_batch(const std::vector<uint8_t> key, const std::vector<uint8_t>& value);

      /**
       * Remove from batchman.
       */
      void remove_batch(const std::vector<uint8_t> key);

      /**
       * Write to the actual database.
       */
      void write_batch();

      /**
       * Undo any combination of the following actions done to the
       * `_state`: adding key/value pairs, modifying key/value pairs,
       * and removing keys (key/value pairs).
       */
      void undo();

      /**
       * After each `undo_state` is acted on appropriately by a call
       * to `undo`, it shall get popped off of the `_undo_stack`.
       * Therefore, a call to `undo_all` will continually undo and
       * pop states off of the stack until `_undo_stack` is
       * empty. The analagous function to this is `commit`; which
       * clears the `_undo_stack`, and does not act upon any of the
       * pushed `undo_state` objects.
       */
      void undo_all();

      /**
       * Commit all `undo_state`s to the `_state` by effectively not
       * acting upon any of the `undo_state` objects on the `_stack`.
       */
      void commit();

      /**
       * All possible combinations of squashing two `undo_state` objects together:
       *
       * Case 1:                                                             /// Case 1':
       *   1.  State 0:     S = {}, U = []                                   ///   1.  State 0:     S = {(K,V)}, U = []
       *   2.  Operation 1: (start_undo_session)                             ///   2.  Operation 1: (start_undo_session)
       *   3.  State 1:     S = {}, U = [{}]                                 ///   3.  State 1:     S = {(K,V)}, U = [{}]
       *   4.  Operation 2: ()                                               ///   4.  Operation 2: ()
       *   5.  State 2:     S = {}, U = [{}]                                 ///   5.  State 2:     S = {(K,V)}, U = [{}]
       *   6.  Operation 3: (start_undo_session)                             ///   6.  Operation 3: (start_undo_session)
       *   7.  State 3:     S = {}, U = [{},{}]                              ///   7.  State 3:     S = {(K,V)}, U = [{},{}]
       *   8.  Operation 4: ()                                               ///   8.  Operation 4: ()
       *   9.  State 4:     S = {}, U = [{},{}]                              ///   9.  State 4:     S = {(K,V)}, U = [{},{}]
       *   10. Operation 5: (squash)                                         ///   10. Operation 5: (squash)
       *   11. State 5:     S = {}, U = [{}]                                 ///   11. State 5:     S = {(K,V)}, U = [{}]
       *   12. Operation 6: (undo)                                           ///   12. Operation 6: (undo)
       *   13. State 6:     S = {}, U = []                                   ///   13. State 6:     S = {(K,V)}, U = []
       *                                                                     ///
       * Case 2:                                                             /// Case 2':
       *   1.  State 0:     S = {}, U = []                                   ///   1.  State 0:     S = {(K,V)}, U = []
       *   2.  Operation 1: (start_undo_session)                             ///   2.  Operation 1: (start_undo_session)
       *   3.  State 1:     S = {}, U = [{}]                                 ///   3.  State 1:     S = {(K,V)}, U = [{}]
       *   4.  Operation 2: (put,K,V)                                        ///   4.  Operation 2: (put,K,V')
       *   5.  State 2:     S = {(K,V)}, U = [{(new,K)}]                     ///   5.  State 2:     S = {(K,V')}, U = [{(update, K,V)}]
       *   6.  Operation 3: (start_undo_session)                             ///   6.  Operation 3: (start_undo_session)
       *   7.  State 3:     S = {(K,V)}, U = [{(new,K)}, {}]                 ///   7.  State 3:     S = {(K,V')}, U = [{(modified, K,V)}, {}]
       *   8.  Operation 4: ()                                               ///   8.  Operation 4: ()
       *   9.  State 4:     S = {(K,V)}, U = [{(new,K)}, {}]                 ///   9.  State 4:     S = {(K,V')}, U = [{(modified, K,V)}, {}]
       *   10. Operation 5: (squash)                                         ///   10. Operation 5: (squash)
       *   11. State 5:     S = {(K,V)}, U = [{(new,K)}]                     ///   11. State 5:     S = {(K,V')}, U = [{(modified, K,V)}]
       *   12. Operation 6: (undo)                                           ///   12. Operation 6: (undo)
       *   13. State 6:     S = {}, U = []                                   ///   13. State 6:     S = {(K,V)}, U = []
       *                                                                     ///
       * Case 3:                                                             /// Case 3':
       *   1.  State 0:     S = {}, U = []                                   ///   1.  State 0:     S = {K,V}, U = []
       *   2.  Operation 1: (start_undo_session)                             ///   2.  Operation 1: (start_undo_session)
       *   3.  State 1:     S = {}, U = [{}]                                 ///   3.  State 1:     S = {K,V}, U = [{}]
       *   4.  Operation 2: ()                                               ///   4.  Operation 2: ()
       *   5.  State 2:     S = {}, U = [{}]                                 ///   5.  State 2:     S = {}, U = [{}]
       *   6.  Operation 3: (start_undo_session)                             ///   6.  Operation 3: (start_undo_session)
       *   7.  State 3:     S = {}, U = [{}, {}]                             ///   7.  State 3:     S = {}, U = [{}, {}]
       *   8.  Operation 4: (put,K,V)                                        ///   8.  Operation 4: (put,K,V')
       *   9.  State 4:     S = {(K,V)}, U = [{}, {(new, K)}]                ///   9.  State 4:     S = {(K,V')}, U = [{}, {(modified,K,V)}]
       *   10. Operation 5: (squash)                                         ///   10. Operation 5: (squash)
       *   11. State 5:     S = {(K,V)}, U = [{(new, K)}]                    ///   11. State 5:     S = {(K,V')}, U = [{(modified,K,V)}]
       *   12. Operation 6: (undo)                                           ///   12. Operation 6: (undo)
       *   13. State 6:     S = {}, U = []                                   ///   13. State 6:     S = {K,V}, U = []
       *                                                                     ///
       * Case 4:                                                             /// Case 4':
       *   1.  State 0:     S = {}, U = []                                   ///   1.  State 0:     S = {K,V}, U = []
       *   2.  Operation 1: (start_undo_session)                             ///   2.  Operation 1: (start_undo_session)
       *   3.  State 1:     S = {}, U = [{}]                                 ///   3.  State 1:     S = {}, U = [{}]
       *   4.  Operation 2: (put,K,V)                                        ///   4.  Operation 2: (put,K,V')
       *   5.  State 2:     S = {(K,V)}, U = [{(new,K)}]                     ///   5.  State 2:     S = {(K,V')}, U = [{(modified,K,V)}]
       *   6.  Operation 3: (start_undo_session)                             ///   6.  Operation 3: (start_undo_session)
       *   7.  State 3:     S = {(K,V)}, U = [{(new,K)}, {}]                 ///   7.  State 3:     S = {(K,V')}, U = [{(modified,K,V)}, {}]
       *   8.  Operation 4: (put,K,V')                                       ///   8.  Operation 4: (put,K,V'')
       *   9.  State 4:     S = {(K,V')}, U = [{(new,K)}, {(modified,K,V)}]  ///   9.  State 4:     S = {(K, V'')}, U = [{(modified,K,V)}, {(modified,K,V')}]
       *   10. Operation 5: (squash)                                         ///   10. Operation 5: (squash)
       *   11. State 5:     S = {(K,V')}, U = [{(new,K)}]                    ///   11. State 5:     S = {(K, V'')}, U = [{(modified,K,V)}]
       *   12. Operation 6: (undo)                                           ///   12. Operation 6: (undo)
       *   13. State 6:     S = {}, U = []                                   ///   13. State 6:     S = {K,V}, U = []
       *                                                                     ///
       * Case 5:                                                             /// Case 5':
       *   1.  State 0:     S = {}, U = []                                   ///   1.  State 0:     S = {(K,V)}, U = []
       *   2.  Operation 1: (start_undo_session)                             ///   2.  Operation 1: (start_undo_session)
       *   3.  State 1:     S = {}, U = [{}]                                 ///   3.  State 1:     S = {(K,V)}, U = [{}]
       *   4.  Operation 2: (remove,K)                                       ///   4.  Operation 2: (remove,K)
       *   5.  State 2:     IMPOSSIBLE                                       ///   5.  State 2:     S = {}, U = [{(removed,K,V)}]
       *   6.  Operation 3: IMPOSSIBLE                                       ///   6.  Operation 3: (start_undo_session)
       *   7.  State 3:     IMPOSSIBLE                                       ///   7.  State 3:     S = {}, U = [{(removed,K,V)}, {}]
       *   8.  Operation 4: IMPOSSIBLE                                       ///   8.  Operation 4: ()
       *   9.  State 4:     IMPOSSIBLE                                       ///   9.  State 4:     S = {}, U = [{(removed,K,V)}, {}]
       *   10. Operation 5: IMPOSSIBLE                                       ///   10. Operation 5: (squash)
       *   11. State 5:     IMPOSSIBLE                                       ///   11. State 5:     S = {}, U = [{(removed,K,V)}]
       *   12. Operation 6: IMPOSSIBLE                                       ///   12. Operation 6: (undo)
       *   13. State 6:     IMPOSSIBLE                                       ///   13. State 6:     S = {(K,V)}, U = []
       *                                                                     ///
       * Case 6:                                                             /// Case 6':
       *   1.  State 0:     S = {}, U = []                                   ///   1.  State 0:     S = {(K,V)}, U = []
       *   2.  Operation 1: (start_undo_session)                             ///   2.  Operation 1: (start_undo_session)
       *   3.  State 1:     S = {}, U = [{}]                                 ///   3.  State 1:     S = {(K,V)}, U = [{}]
       *   4.  Operation 2: ()                                               ///   4.  Operation 2: ()
       *   5.  State 2:     S = {}, U = [{}]                                 ///   5.  State 2:     S = {(K,V)}, U = [{}]
       *   6.  Operation 3: (start_undo_session)                             ///   6.  Operation 3: (start_undo_session)
       *   7.  State 3:     S = {}, U = [{}, {}]                             ///   7.  State 3:     S = {(K,V)}, U = [{}, {}]
       *   8.  Operation 4: (remove,K)                                       ///   8.  Operation 4: (remove,K)
       *   9.  State 4:     IMPOSSIBLE                                       ///   9.  State 4:     S = {}, U = [{}, {(removed,K,V)}]
       *   10. Operation 5: IMPOSSIBLE                                       ///   10. Operation 5: (squash)
       *   11. State 5:     IMPOSSIBLE                                       ///   11. State 5:     S = {}, U = [{(removed,K,V)}]
       *   12. Operation 6: IMPOSSIBLE                                       ///   12. Operation 6: (undo)
       *   13. State 6:     IMPOSSIBLE                                       ///   13. State 6:     S = {(K,V)}, U = []
       *                                                                     ///
       *
       * Case 7:                                                             /// Case 7':
       *   1.  State 0:     S = {}, U = []                                   ///   1.  State 0:     S = {(K,V)}, U = []
       *   2.  Operation 1: (start_undo_session)                             ///   2.  Operation 1: (start_undo_session)
       *   3.  State 1:     S = {}, U = [{}]                                 ///   3.  State 1:     S = {(K,V)}, U = [{}]
       *   4.  Operation 2: (remove,K)                                       ///   4.  Operation 2: (remove,K)
       *   5.  State 2:     IMPOSSIBLE                                       ///   5.  State 2:     S = {}, U = [{(removed,K,V)}]
       *   6.  Operation 3: IMPOSSIBLE                                       ///   6.  Operation 3: (start_undo_session)
       *   7.  State 3:     IMPOSSIBLE                                       ///   7.  State 3:     S = {}, U = [{(removed,K,V)}, {}]
       *   8.  Operation 4: IMPOSSIBLE                                       ///   8.  Operation 4: (remove,K)
       *   9.  State 4:     IMPOSSIBLE                                       ///   9.  State 4:     IMPOSSIBLE
       *   10. Operation 5: IMPOSSIBLE                                       ///   10. Operation 5: IMPOSSIBLE
       *   11. State 5:     IMPOSSIBLE                                       ///   11. State 5:     IMPOSSIBLE
       *   12. Operation 6: IMPOSSIBLE                                       ///   12. Operation 6: IMPOSSIBLE
       *   13. State 6:     IMPOSSIBLE                                       ///   13. State 6:     IMPOSSIBLE
       *                                                                     ///
       * Case 8:                                                             /// Case 8':
       *   1.  State 0:     S = {}, U = []                                   ///   1.  State 0:     S = {K,V}, U = []
       *   2.  Operation 1: (start_undo_session)                             ///   2.  Operation 1: (start_undo_session)
       *   3.  State 1:     S = {}, U = [{}]                                 ///   3.  State 1:     S = {K,V}, U = [{}]
       *   4.  Operation 2: (put,K,V)                                        ///   4.  Operation 2: (put,K,V')
       *   5.  State 2:     S = {(K,V)}, U = [{(new,K)}]                     ///   5.  State 2:     S = {(K,V')}, U = [{(modified,K,V)}]
       *   6.  Operation 3: (start_undo_session)                             ///   6.  Operation 3: (start_undo_session)
       *   7.  State 3:     S = {(K,V)}, U = [{(new, K)}, {}]                ///   7.  State 3:     S = {(K,V)}, U = [{(modified,K,V)}, {}]
       *   8.  Operation 4: (del,K)                                          ///   8.  Operation 4: (del,K)
       *   9.  State 4:     S = {}, U = [{(new,K)}, {(removed,K,V)}]         ///   9.  State 4:     S = {}, U = [{(modified,K,V)}, {(removed,K,V')}]
       *   10. Operation 5: (squash)                                         ///   10. Operation 5: (squash)
       *   11. State 5:     S = {}, U = [{}]                                 ///   11. State 5:     S = {}, U = [{modifed,K,V}]
       *   12. Operation 6: (undo)                                           ///   12. Operation 6: (undo)
       *   13. State 6:     S = {}, U = []                                   ///   13. State 6:     S = {(K,V)}, U = []
       *                                                                     ///
       * Case 9:                                                             /// Case 9':
       *   1.  State 0:     S = {}, U = []                                   ///   1.  State 0:     S = {(K,V)}, U = []
       *   2.  Operation 1: (start_undo_session)                             ///   2.  Operation 1: (start_undo_session)
       *   3.  State 1:     S = {}, U = [{}]                                 ///   3.  State 1:     S = {(K,V)}, U = [{}]
       *   4.  Operation 2: (remove,K)                                       ///   4.  Operation 2: (remove,K)
       *   5.  State 2:     IMPOSSIBLE                                       ///   5.  State 2:     S = {}, U = [{(removed,K,V)}]
       *   6.  Operation 3: IMPOSSIBLE                                       ///   6.  Operation 3: (start_undo_session)
       *   7.  State 3:     IMPOSSIBLE                                       ///   7.  State 3:     S = {}, U = [{(removed,K,V)}, {}]
       *   8.  Operation 4: IMPOSSIBLE                                       ///   8.  Operation 4: (put,K,V')
       *   9.  State 4:     IMPOSSIBLE                                       ///   9.  State 4:     S = {(K,V')}, U = [{(removed,K,V)}, {(new,K)}]
       *   10. Operation 5: IMPOSSIBLE                                       ///   10. Operation 5: (squash)
       *   11. State 5:     IMPOSSIBLE                                       ///   11. State 5:     S = {(K,V')}, U = [{(modifed,K,V)}]
       *   12. Operation 6: IMPOSSIBLE                                       ///   12. Operation 6: (undo)
       *   13. State 6:     IMPOSSIBLE                                       ///   13. State 6:     S = {(K,V)}, U = []
       */

      /**
       * Sqaushing is the act of taking the first two recent
       * `undo_state`s and combining them into one.
       */
      void squash();

      /**
       * A `session` is allowed to hold only one `index`. And is
       * allowed to specify if its given `index` is eligible to acted
       * upon be `undo` or `squash` via the boolean value `_apply`.
       * Note that the session be explicitly told not to do so, by
       * calling the method `push`.
       */
      class session {
      public:
         /**
          *
          */
         session(const session&) = delete;
         session& operator= (const session&) = delete;

         /**
          *
          */
         session(session&& s);

         /**
          *
          */
         session& operator= (session&& s);

         /**
          * RAII functionality; upon the destruction of `session` it
          * shall be determined whether or not the `_index` is acted
          * upon by `undo`.
          */
         ~session();

         /**
          *
          */
         void push();

         /**
          *
          */
         void undo();

         /**
          *
          */
         void squash();

         /**
          *
          */
         int64_t revision() const;

      private:
         /**
          *
          */
         friend class database;

         /**
          * Note that it is not possible to directly construct a
          * session object because this responsibility shall be left
          * to the method `start_undo_session`.
          */
         session(database& db, int64_t revision);

         /**
          * The given `index` for this session to hold.
          */
         database& _state;

         /**
          * The predicate determining whether or not this `session`
          * The predicate determining whether or not this `session`
          * is elligle to be acted upon by either `undo` or `squash`.
          */
         bool _apply{true};

         /**
          * The unique revision number held by each `session` object.
          * Note that this `_revision` number and the `_revision`
          * number of the held `index` will be exactly identical to
          * eachother.
          */
         int64_t _revision{};
      };

      /**
       * Probably the most interested method in the chainbase
       * repository. It can be essentially thought of as one of two
       * ideas: the start of a block (containing transactions) in a
       * blockchain, or the start of a transaction (containing
       * actions) within a block. Each of which have the ability to
       * `commit` if valid or to `undo` if invalid due to the nature
       * of these data structures.
       */
      session start_undo_session(bool enabled);

   private:
      /**
       * Update the mapping `_new_keys` of the most recently created
       * `undo_state` object.
       */
      void _on_create(const std::vector<uint8_t>& key, const std::vector<uint8_t>& value);

      /**
       * Update the mapping `_modified_values` of the most recently
       * created `undo_state` object.
       */
      void _on_put(const std::vector<uint8_t>& key, const std::vector<uint8_t>& value);

      /**
       * Update the mapping `_removed_values` of the most recently
       * created `undo_state` object.
       */
      void _on_remove(const std::vector<uint8_t>& key);

      /**
       * Effectively erase any new key/value pairs introduced to
       * `_state`.
       */
      void _undo_new_keys(undo_state&& head);

      /**
       * Effectively replace any modified key/value pairs with its
       * previous value to `_state`.
       */
      void _undo_modified_values(undo_state&& head);

      /**
       * Effectively reintroduce any removed key/value pairs from
       * `_state` back into `_state`.
       */
      void _undo_removed_values(undo_state&& head);

      /**
       * Effectively squash `_new_keys` of the previous two
       * `undo_state` objects on the `_stack` together.
       */
      void _squash_new_keys(undo_state&& head, undo_state& head_minus_one);

      /**
       * Effectively squash `_modifed_values` of the previous two
       * `undo_state` objects on the `_stack` together.
       */
      void _squash_modified_values(undo_state&& head, undo_state& head_minus_one);

      /**
       * Effectively squash `_removed_values` of the previous two
       * `undo_state` objects on the `_stack` together.
       */
      void _squash_removed_values(undo_state&& head, undo_state& head_minus_one);

      /**
       * If the `_stack` is not empty, then it is eligible to be
       * acted upon by `undo` or `squash`.
       */
      bool _enabled() const;

   private:
      /**
       * The current state of the `index` object.
       */
      rocksdb_backend _state;

      /**
       * Stack to hold multiple `undo_state` objects to keep track of
       * the modifications made to `_state`.
       */
      std::deque<undo_state> _stack;

      /**
       * The unique revision number held by each `index` object.
       */
      int64_t _revision;
   };
}
