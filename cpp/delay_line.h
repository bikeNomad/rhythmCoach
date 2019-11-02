/**
 * @file delay_line.h
 * @brief Simple delay line
 */

#pragma once
#include <iterator>

template <class T, unsigned N>
class DelayLineIterator;

template <class T, unsigned N>
class DelayLine {
 public:
  typedef DelayLine self_type;
  typedef DelayLine container_type;
  typedef unsigned size_type;

  class const_iterator {
   public:
    typedef const_iterator self_type;
    typedef T value_type;
    typedef T& reference;
    typedef T* pointer;
    typedef int difference_type;
    typedef std::forward_iterator_tag iterator_category;
    const_iterator(container_type const* ctr, size_type posn)
        : container(ctr), pos(posn) {}
    self_type operator++() {
      self_type i = *this;
      pos++;
      return i;
    }
    self_type operator++(int) {
      pos++;
      return *this;
    }
    reference operator*() { return (*container)[pos]; }
    const pointer operator->() { return &(*container)[pos]; }
    bool operator==(const self_type& rhs) {
      return pos == rhs.pos && container == rhs.container;
    }
    bool operator!=(const self_type& rhs) {
      return pos != rhs.pos || container != rhs.container;
    }

   private:
    container_type const* const container;
    unsigned pos;
  };

  DelayLine() : items_(), head_(0), empty_item_(0) { clear(); }

  constexpr unsigned size() { return N; }

  T const& operator[](unsigned delay) const {
    if (delay >= N) {
      return empty_item_;
    }
    int index = head_ - delay - 1;
    if (index < 0) {
      index += N;
    }
    return items_[index];
  }

  void add_item(T item) {
    items_[head_++] = item;
    if (head_ >= N) {
      head_ -= N;
    }
  }

  void clear() {
    head_ = 0;
    for (unsigned i = 0; i < N; i++) {
      items_[i] = empty_item_;
    }
  }

  const_iterator cbegin() { return const_iterator(this, 0); }
  const_iterator cend() { return const_iterator(this, N); }

 protected:
  T items_[N];
  unsigned head_;
  const T empty_item_;
};
