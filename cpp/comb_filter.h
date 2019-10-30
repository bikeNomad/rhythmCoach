/**
 * @file comb_filter.h
 */

#pragma once

#include "delay_line.h"
#include <array>

template<class ItemType, class InputItemType, unsigned N>
class CombFilterbank {
 public:
  CombFilterbank() :delay_line_(), accumulator_(), num_items_(0) {}

  void add_item(InputItemType item) {
    delay_line_.add_item(item);
    for (unsigned i = 0; i < N; i++) {
      accumulator_[i] += delay_line_.at_delay(i);
    }
  }

  ItemType at_delay(unsigned delay) const {
    return accumulator_[delay];
  }

  unsigned num_items() const { return num_items_; }

  void clear() {
    delay_line_.clear();
    accumulator_.fill(empty_item_);
    num_items_ = 0;
  }

 protected:
  DelayLine<InputItemType, N> delay_line_;
  std::array<ItemType, N> accumulator_;
  unsigned num_items_;
  static constexpr ItemType empty_item_ = {0};
};
