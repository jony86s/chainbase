/**
 *  @file logger.cpp
 *  @copyright defined in eosio/LICENSE.txt
 */

#include <algorithm> // std::max_element|min_element|sort
#include <iomanip>   // std::setw
#include <iostream>  // std::cout|flush
#include <numeric>   // std::accumulate

#include "logger.hpp"

logger::logger()
    : _data_file{(boost::filesystem::current_path() /= std::string{"/data.csv"}).string()}
    , _metrics_file{(boost::filesystem::current_path() /= std::string{"/metrics.csv"}).string()}
      
{
    _tps.reserve(10000);
    _total_vm_usage.reserve(10000);
}

void logger::print_progress(uint64_t n, uint64_t m) {
    if (m == 0) {
        std::cout << '[' << std::setw(3) << 0 << "%]\n";
        return;
    }
    std::cout << '[' << std::setw(3) << (static_cast<uint64_t>((static_cast<double>(n)/m)*100.0)) << "%]\n";
}

void logger::flush_all() {
    for (uint64_t i{}; i < _tps.size(); ++i) {
        _data_file << std::setw(10) << _total_vm_usage[i] << '\t';
        _data_file << std::setw(10) << _tps[i]            << '\n';
    }

    _flush_metrics();
}

void logger::_flush_metrics() {
    std::sort(_tps.begin(), _tps.end());
    
    ptrdiff_t quartile;
    double vm_usage_mode;
    double tps_min;
    double tps_max;
    double tps_median;
    double tps_25th_percentile;
    double tps_75th_percentile;
    double tps_mean;

    quartile = _tps.size()/4;
    vm_usage_mode = std::accumulate(_total_vm_usage.begin(), _total_vm_usage.end(), 0) / static_cast<double>(_total_vm_usage.size());
    tps_min = *std::min_element(_tps.cbegin(), _tps.cend());
    tps_max = *std::max_element(_tps.cbegin(), _tps.cend());
    tps_median = *(_tps.cbegin() + (_tps.size()) / 2);
    tps_25th_percentile = *(_tps.cbegin()+quartile);
    tps_75th_percentile = *(_tps.crbegin()+quartile);
    tps_mean = std::accumulate(_tps.cbegin(), _tps.cend(), 0) / static_cast<double>(_tps.size());

    _metrics_file << std::setw(10) << vm_usage_mode       << "\t"; // x
    _metrics_file << std::setw(10) << tps_min             << "\t"; // whisker_min
    _metrics_file << std::setw(10) << tps_25th_percentile << "\t"; // box_min
    _metrics_file << std::setw(10) << tps_median          << "\t"; // median
    _metrics_file << std::setw(10) << tps_75th_percentile << "\t"; // box_max
    _metrics_file << std::setw(10) << tps_max             << "\t"; // whisker_max
    _metrics_file << std::setw(10) << tps_mean            << "\n"; // extra
}

void logger::log_total_vm_usage(const uint64_t& n) {
    _total_vm_usage.push_back(n);
}

void logger::log_tps(const uint64_t& n) {
    _tps.push_back(n);
}
