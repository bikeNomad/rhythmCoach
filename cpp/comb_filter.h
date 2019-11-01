/**
 * @file comb_filter.h
 */

#pragma once

#include "delay_line.h"
#include <array>

/**
 * @param ItemType the type of the comb filter accumulator elements
 * @param InputItemType the type of the delay line elements
 * @param N the size of the delay line and accumulator
 */
template<class ItemType, class InputItemType, unsigned N>
class CombFilterbank {
 public:
  CombFilterbank() :delay_line_(), accumulator_(), num_items_(0) { clear(); }

  /** @return quality factor */
  float add_item(InputItemType item) {
    float retval = 0.0;
    delay_line_.add_item(item);
    if (item > 0) {
      for (unsigned i = 0; i < N; i++) {
       retval += (accumulator_[i] += delay_line_[i]);
      }
      num_items_++;
      retval /= num_items_;
    }
    return retval;
  }

  ItemType operator[](unsigned delay) const {
    return accumulator_[delay];
  }

  unsigned num_items() const { return num_items_; }

  void clear() {
    delay_line_.clear();
    for (unsigned i = 0; i < N; i++) {
      accumulator_[i] = empty_item_;
    }
    num_items_ = 0;
  }

  float normalized_at(unsigned delay) const {
    return static_cast<float>(accumulator_[delay]) / num_items_;
  }

 protected:
  DelayLine<InputItemType, N> delay_line_;
  std::array<ItemType,N> accumulator_;
  unsigned num_items_;
  static constexpr ItemType empty_item_ = {0};
};
