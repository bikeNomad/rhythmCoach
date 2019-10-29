/**
 * @file delay_line.h
 * @brief Simple delay line
 */

#pragma once
#include <array>

template<class ItemType, unsigned N>
class DelayLine {
 public:
  DelayLine() : items_(), head_(0), emptyItem_(static_cast<ItemType>(0)) { items_.fill(emptyItem_); }

  ItemType const & at_delay(unsigned delay) const {
    if (delay >= N) { return emptyItem_; }
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

 protected:
  std::array<ItemType, N> items_;
  unsigned head_;
  constexpr ItemType emptyItem_;
};
