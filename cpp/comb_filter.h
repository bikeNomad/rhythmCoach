/**
 * @file comb_filter.h
 */

#pragma once

#include <algorithm>
#include <array>
#include <numeric>

#include "delay_line.h"

/**
 * @param ItemType the type of the comb filter accumulator elements
 * @param InputItemType the type of the delay line elements
 * @param N the size of the delay line and accumulator
 */
template <class ItemType, class InputItemType, unsigned N>
class CombFilterbank {
 public:
  CombFilterbank() : delay_line_(), accumulator_(), num_items_(0) { clear(); }

  typedef std::array<ItemType, N> accumulator_type;

  const accumulator_type &accumulator() const { return accumulator_; }

  /** @return array of floats, smoothed over ::window and normalized by num_items() */
  std::array<float, N> smoothed(unsigned window) const {
    std::array<float, N> retval;
    const unsigned half_window = window / 2;

    for (unsigned i = 0; i < half_window; i++) {
      retval[i] = 0.0;
    }
    for (unsigned left = 0; left < N - window; left++) {
      unsigned sum = std::accumulate(accumulator_.cbegin() + left,
                                     accumulator_.cbegin() + left + window,
                                     0.0);
      retval[left + half_window] = static_cast<float>(sum) / num_items();
    }
    for (unsigned i = N - half_window; i < N; i++) {
      retval[i] = 0.0;
    }
    return retval;
  }

  void add_item(InputItemType item) {
    delay_line_.add_item(item);
    if (item > 0) {
      for (unsigned i = 0; i < N; i++) {
        accumulator_[i] += delay_line_[i];
      }
      num_items_++;
    }
  }

  ItemType operator[](unsigned delay) const { return accumulator_[delay]; }

  unsigned num_items() const { return num_items_; }

  void clear() {
    delay_line_.clear();
    for (unsigned i = 0; i < N; i++) {
      accumulator_[i] = empty_item_;
    }
    num_items_ = 0;
  }

 protected:
  DelayLine<InputItemType, N> delay_line_;
  std::array<ItemType, N> accumulator_;
  unsigned num_items_;
  static constexpr ItemType empty_item_ = {0};
};
