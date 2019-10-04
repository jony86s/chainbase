/**
 *  @file database.cpp
 *  @copyright defined in eosio/LICENSE.txt
 */

#include <iostream>    // std::cout
#include <string>      // std::move
#include <type_traits> // std::is_pointer
#include <utility>     // std::move

#include <boost/throw_exception.hpp> // BOOST_THROW_EXCEPTION

#include <chainrocks/database.hpp> // chainrocks::database

#include <rocksdb/iterator.h> // rocksdb::WriteBatch

namespace chainrocks {
   database::database(const boost::filesystem::path& database_dir)
      : _state{database_dir}
      , _stack{}
      , _revision{}
   {
   }

   const rocksdb_backend& database::state() const {
      return _state;
   }

   std::deque<undo_state>& database::stack() {
      return _stack;
   }

   void database::put(const std::vector<uint8_t>& key, const std::vector<uint8_t>& value) {
      _on_put(key, value);
      put_batch(key, value);
   }

   void database::remove(const std::vector<uint8_t>& key) {
      _on_remove(key);
      remove_batch(key);
   }

   void database::get(const std::vector<uint8_t> key, std::string &value) {
      _state.get(key, value);
   }
      
   bool database::does_key_exist(const std::vector<uint8_t> key, std::string tmp) {
      return _state.does_key_exist(key, tmp);
   }

   void database::put_batch(const std::vector<uint8_t> key, const std::vector<uint8_t>& value) {
      _state.put_batch(key, value);
   }

   void database::remove_batch(const std::vector<uint8_t> key) {
      _state.remove_batch(key);
   }

   void database::write_batch() {
      _state.write_batch();
   }

   void database::undo() {
      if(!_enabled()) {
         return;
      }

      _undo_new_keys(std::move(_stack.back()));
      _undo_modified_values(std::move(_stack.back()));
      _undo_removed_values(std::move(_stack.back()));

      _stack.pop_back();
      --_revision;
   }

   void database::undo_all() {
      while (_enabled()) {
         undo();
      }
   }

   void database::commit() {
      while (_stack.size()) {
         _stack.clear();
      }
   }

   void database::squash() {
      if(!_enabled()) {
         return;
      }

      if(_stack.size() == 1) {
         _stack.pop_front();
         return;
      }

      auto& head_minus_one = _stack[_stack.size()-2];

      _squash_new_keys(std::move(_stack.back()), head_minus_one);
      _squash_modified_values(std::move(_stack.back()), head_minus_one);
      _squash_removed_values(std::move(_stack.back()), head_minus_one);

      _stack.pop_back();
      --_revision;
   }

   void database::print_state() {
      std::cout << "_db: ";
      rocksdb::Iterator* iter{_state.db()->NewIterator(_state.options().read_options())};
      for (iter->SeekToFirst(); iter->Valid(); iter->Next()) {
         std::cout << iter->key().ToString() << ':' << iter->value().ToString() << ' ';
      }
      std::cout << '\n';
      assert(iter->status().ok()); // Check for any errors found during the scan
      delete iter;
   }

   std::map<std::string,std::string> database::get_as_map() {
      std::map<std::string,std::string> ret;
      rocksdb::Iterator* iter{_state.db()->NewIterator(_state.options().read_options())};
      for (iter->SeekToFirst(); iter->Valid(); iter->Next()) {
         ret[iter->key().ToString()] = iter->value().ToString();
      }
      assert(iter->status().ok()); // Check for any errors found during the scan
      delete iter;
      return ret;
   }

   database::session database::start_undo_session(bool enabled) {
      if (enabled) {
         _stack.emplace_back();
         _stack.back().revision() = ++_revision;
         return session{*this, _revision};
      } else {
         return session{*this, -1};
      }
   }

   void database::_on_create(const std::vector<uint8_t>& key, const std::vector<uint8_t>& value) {
      if (!_enabled()) {
         return;
      }
      
      auto& head = _stack.back();
      head.new_keys().insert(key);
   }

   void database::_on_put(const std::vector<uint8_t>& key, const std::vector<uint8_t>& value) {
      if (!_enabled()) {
         return;
      }

      auto& head = _stack.back();

      if (!_state.does_key_exist(key)) {
         _on_create(key, value);
         return;
      }
      else {
         std::string tmp;
         _state.get(key, tmp);
         head.modified_values().emplace(key, std::vector<uint8_t>(tmp.cbegin(), tmp.cend()));
      }
   }

   void database::_on_remove(const std::vector<uint8_t>& key) {
      if (!_enabled()) {
         return;
      }
      else {
         auto& head = _stack.back();

         if (head.removed_values().find(key) != head.removed_values().cend()) {
            BOOST_THROW_EXCEPTION(std::runtime_error{"on_remove"});
         }

         std::string tmp{};
         _state.get(key, tmp);
         head.removed_values()[key] = std::vector<uint8_t>(tmp.cbegin(), tmp.cend());
      }
   }

   void database::_undo_new_keys(undo_state&& head) {
      for (auto&& key : head.new_keys()) {
         _state.remove_batch(key);
      }
   }

   void database::_undo_modified_values(undo_state&& head) {
      for (auto&& modified_value : head.modified_values()) {
         _state.put_batch(modified_value.first, modified_value.second);
      }
   }

   void database::_undo_removed_values(undo_state&& head) {
      for (auto&& removed_value : head.removed_values()) {
         _state.put_batch(removed_value.first, removed_value.second);
      }
   }

   void database::_squash_new_keys(undo_state&& head, undo_state& head_minus_one) {
      for (auto&& key : head.new_keys()) {
         head_minus_one.new_keys().insert(std::move(key));
      }
   }

   void database::_squash_modified_values(undo_state&& head, undo_state& head_minus_one) {
      for (auto&& value : head.modified_values()) {
         if (head_minus_one.new_keys().find(value.first) != head_minus_one.new_keys().cend()) {
            continue;
         }
         if (head_minus_one.modified_values().find(value.first) != head_minus_one.modified_values().cend()) {
            continue;
         }
         assert(head_minus_one.removed_values().find(value.first) == head_minus_one.removed_values().cend());
         head_minus_one.modified_values()[value.first] = std::move(value.second);
      }
   }

   void database::_squash_removed_values(undo_state&& head, undo_state& head_minus_one) {
      for (auto&& value : head.removed_values()) {
         if (head_minus_one.new_keys().find(value.first) != head_minus_one.new_keys().cend()) {
            head_minus_one.new_keys().erase(std::move(value.first));
            continue;
         }

         auto iter{head_minus_one.modified_values().find(value.first)};
         if (iter != head_minus_one.modified_values().cend()) {
            head_minus_one.removed_values()[iter->first] = std::move(iter->second);
            head_minus_one.modified_values().erase(std::move(value.first));
            continue;
         }
         assert(head_minus_one.removed_values().find(value.first) == head_minus_one.removed_values().cend());
         head_minus_one.removed_values()[value.first] = std::move(value.second);
      }
   }

   bool database::_enabled() const {
      return _stack.size();
   }

   database::session::~session() {
      if(_apply) {
         _db.undo();
      }
   }

   database::session::session(database::session&& s)
      : _db{s._db}
      , _apply{s._apply}
      , _revision{}
   {
      s._apply = false;
   }

   database::session& database::session::operator= (database::session&& s) {
      if (this == &s) {
         return *this;
      }
      
      if (_apply) {
         _db.undo();
      }
         
      _apply = s._apply;
      s._apply = false;
      return *this;
   }

   void database::session::push() {
      _apply = false;
   }

   void database::session::undo() {
      if (_apply) {
         _db.undo();
         _apply = false;
      }
   }

   void database::session::squash() {
      if (_apply) {
         _db.squash();
         _apply = false;
      }
   }

   int64_t database::session::revision() const {
      return _revision;
   }

   database::session::session(database& db, int64_t revision)
      : _db{db}
      , _revision{revision}
   {
      if(_revision == -1) {
         _apply = false;
      }
   }
}
