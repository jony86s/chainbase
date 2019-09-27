#include <utility> // std::move

#include <boost/throw_exception.hpp> // BOOST_THROW_EXCEPTION

#include "database.hpp" // chainrocks::database

namespace chainrocks {
   database::database(const boost::filesystem::path& database_dir)
      : _state{database_dir}
      , _stack{}
      , _revision{}
   {
   }

   database::~database()
   {
   }

   const auto& database::state() const {
      return _state;
   }

   const auto& database::stack() const {
      return _stack;
   }

   void database::put(const std::vector<uint8_t>& key, const std::vector<uint8_t>& value) {
      _on_put(key, value);
      _state.put(key, value);
   }

   void database::remove(const std::vector<uint8_t>& key) {
      _on_remove(key);
      _state.remove(key);
   }

   void database::get(const std::vector<uint8_t> key, std::string &value) {
      _state.get(key, value);
   }
      
   bool database::does_key_exist(const std::vector<uint8_t> key, std::string tmp = {}) {
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
   
   

   session database::start_undo_session(bool enabled) {
      if (enabled) {
         _stack.emplace_back();
         _stack.back()._revision = ++_revision;
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
      head._new_keys.insert(key);
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
         head._modified_values.emplace(key, std::vector<uint8_t>(tmp.cbegin(), tmp.cend()));
      }
   }

   void database::_on_remove(const std::vector<uint8_t>& key) {
      if (!_enabled()) {
         return;
      }
      else {
         auto& head = _stack.back();

         if (head._removed_values.find(key) != head._removed_values.cend()) {
            BOOST_THROW_EXCEPTION(std::runtime_error{"on_remove"});
         }

         std::string tmp{};
         _state.get(key, tmp);
         head._removed_values[key] = std::vector<uint8_t>(tmp.cbegin(), tmp.cend());
      }
   }

   void database::_undo_new_keys(undo_state&& head) {
      for (auto&& key : head._new_keys) {
         _state.remove(key);
      }
   }

   void database::_undo_modified_values(undo_state&& head) {
      for (auto&& modified_value : head._modified_values) {
         _state.put(modified_value.first, modified_value.second);
      }
   }

   void database::_undo_removed_values(undo_state&& head) {
      for (auto&& removed_value : head._removed_values) {
         _state.put(removed_value.first, removed_value.second);
      }
   }

   void database::_squash_new_keys(undo_state&& head, undo_state& head_minus_one) {
      for (auto&& key : head._new_keys) {
         head_minus_one._new_keys.insert(std::move(key));
      }
   }

   void database::_squash_modified_values(undo_state&& head, undo_state& head_minus_one) {
      for (auto&& value : head._modified_values) {
         if (head_minus_one._new_keys.find(value.first) != head_minus_one._new_keys.cend()) {
            continue;
         }
         if (head_minus_one._modified_values.find(value.first) != head_minus_one._modified_values.cend()) {
            continue;
         }
         assert(head_minus_one._removed_values.find(value.first) == head_minus_one._removed_values.cend());
         head_minus_one._modified_values[value.first] = std::move(value.second);
      }
   }

   void database::_squash_removed_values(undo_state&& head, undo_state& head_minus_one) {
      for (auto&& value : head._removed_values) {
         if (head_minus_one._new_keys.find(value.first) != head_minus_one._new_keys.cend()) {
            head_minus_one._new_keys.erase(std::move(value.first));
            continue;
         }

         auto iter{head_minus_one._modified_values.find(value.first)};
         if (iter != head_minus_one._modified_values.cend()) {
            head_minus_one._removed_values[iter->first] = std::move(iter->second);
            head_minus_one._modified_values.erase(std::move(value.first));
            continue;
         }
         assert(head_minus_one._removed_values.find(value.first) == head_minus_one._removed_values.cend());
         head_minus_one._removed_values[value.first] = std::move(value.second);
      }
   }

   bool database::_enabled() const {
      return _stack.size();
   }

   database::session::session(database::session&& s)
      : _state{s._state}
      , _apply{s._apply}
   {
      s._apply = false;
   }

   database::session::~session() {
      if(_apply) {
         _state.undo();
      }
   }

   session& database::session::operator= (database::session&& s) {
      if (this == &s) {
         return *this;
      }
      
      if (_apply) {
         _state.undo();
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
         _state.undo();
         _apply = false;
      }
   }

   void database::session::squash() {
      if (_apply) {
         _state.squash();
         _apply = false;
      }
   }

   int64_t database::session::revision() const {
      return _revision;
   }

   database::session::session(database& db, int64_t revision)
      : _state{db}
      , _revision{revision}
   {
      if(_revision == -1) {
         _apply = false;
      }
   }
}
