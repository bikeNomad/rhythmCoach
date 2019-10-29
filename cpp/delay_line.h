/**
 * @file delay_line.h
 * @brief Simple delay line
 */

#pragma once
#include <array>

template<class ItemType, unsigned N>
class DelayLine {
 public:
  DelayLine() : items_(), head_(0) { clear(); }

  ItemType const & at_delay(unsigned delay) const {
    if (delay >= N) { return empty_item_; }
    int index = head_ - delay - 1;
    if (index < 0) {
      index += N;
    }
    return items_.at(index);
  }

  void add_item(ItemType item) {
    items_.at(head_++) = item;
    if (head_ >= N) {
      head_ -= N;
    }
  }

  void clear() {
    head_ = 0;
    items_.fill(empty_item_);
  }

 protected:
  std::array<ItemType, N> items_;
  unsigned head_;
  static constexpr ItemType empty_item_ = {0};
};
