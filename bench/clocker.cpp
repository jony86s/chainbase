/**
 *  @file clocker.cpp
 *  @copyright defined in eosio/LICENSE.txt
 */

#include <chrono>  // std::chrono::high_resolution_clock|time_point
#include <numeric> // std::accumulate

#include "clocker.hpp"

clocker::clocker(size_t interval_in_seconds)
   : _rolling_average{std::make_unique<rolling_average>()}
   , _interval_in_seconds{interval_in_seconds*1000}
{
}

void clocker::reset_clocker() {
   _original_time = _retrieve_time();
   _old_time = _original_time;
   _new_time = _old_time;
}

bool clocker::should_log() {
   _new_time = _retrieve_time();

   if ((_new_time - _old_time) > _interval_in_seconds) {
      return true;
   }
   else {
      return false;
   }
}

void clocker::update_clocker() {
   _old_time = _new_time;
}

size_t clocker::seconds_since_start_of_benchmark() {
   return ((_new_time - _original_time)/1000);
}

size_t clocker::narrow_window() {
   return ((_new_time - _old_time)/1000);
}

long long clocker::_retrieve_time() {
   std::chrono::time_point tp{std::chrono::high_resolution_clock::now()};
   std::chrono::milliseconds d{std::chrono::time_point_cast<std::chrono::milliseconds>(tp).time_since_epoch()};
   long long ms{std::chrono::duration_cast<std::chrono::milliseconds>(d).count()};
   return ms;
}

void clocker::rolling_average::push_term(size_t term) {
   _last_five_terms.push_front(term);
   _last_five_terms.pop_back();
}

size_t clocker::rolling_average::get_rolling_average() {
   return (std::accumulate(_last_five_terms.cbegin(),_last_five_terms.cend(),0) / 5);
}
