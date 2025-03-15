#ifndef SJTU_VECTOR_HPP
#define SJTU_VECTOR_HPP

#include "exceptions.hpp"

#include <climits>
#include <cstddef>

namespace sjtu {
  /**
   * a data container like std::vector
   * store data in a successive memory and support random access.
   */
  template<typename T>
  class vector {
  public:
    size_t size_ = 0;
    size_t capacity_ = 0;
    T** vct = nullptr;
    size_t beg_ind = 0;
    /**
     * TODO
     * a type for actions of the elements of a vector, and you should write
     *   a class named const_iterator with same interfaces.
     */
    /**
     * you can see RandomAccessIterator at CppReference for help.
     */
    class const_iterator;

    class iterator {
      // The following code is written for the C++ type_traits library.
      // Type traits is a C++ feature for describing certain properties of a type.
      // For instance, for an iterator, iterator::value_type is the type that the
      // iterator points to.
      // STL algorithms and containers may use these type_traits (e.g. the following
      // typedef) to work properly. In particular, without the following code,
      // @code{std::sort(iter, iter1);} would not compile.
      // See these websites for more information:
      // https://en.cppreference.com/w/cpp/header/type_traits
      // About value_type: https://blog.csdn.net/u014299153/article/details/72419713
      // About iterator_category: https://en.cppreference.com/w/cpp/iterator
    public:
      using difference_type = std::ptrdiff_t;
      using value_type = T;
      using pointer = T *;
      using reference = T &;
      using iterator_category = std::output_iterator_tag;

    private:
      /**
       * TODO add data members
       *   just add whatever you want.
       */
      vector* v = nullptr;
    public:
      T** ptr = nullptr; // point to T*, i.e.element
      iterator() = default;
      iterator(T** p, vector* v) : v(v), ptr(p) {}
      /**
       * return a new iterator which pointer n-next elements
       * as well as operator-
       */
      iterator operator+(const int &n) const {
        //TODO
        if (ptr == nullptr) {
          return iterator();
        }
        return iterator((ptr - v->vct + n) % v->capacity_ + v->vct, v);
      }

      iterator operator-(const int &n) const {
        //TODO
        if (ptr == nullptr) {
          return iterator();
        }
        return iterator((ptr - v->vct - n + v->capacity_) % v->capacity_ + v->vct, v);
      }

      // return the distance between two iterators,
      // if these two iterators point to different vectors, throw invalid_iterator.
      int operator-(const iterator &rhs) const {
        //TODO
        if (v != rhs.v) {
          throw invalid_iterator();
        }
        return ptr - rhs.ptr;
      }
      int operator-(const const_iterator &rhs) const {
        //TODO
        if (v != rhs.v) {
          throw invalid_iterator();
        }
        return ptr - rhs.ptr;
      }

      iterator &operator+=(const int &n) {
        //TODO
        ptr = v->vct + (ptr - v->vct + n) % v->capacity_;
        return *this;
      }

      iterator &operator-=(const int &n) {
        //TODO
        ptr = (ptr - v->vct - n + v->capacity_) % v->capacity_ + v->vct;
        return *this;
      }

      /**
       * TODO iter++
       */
      iterator operator++(int) {
        if (ptr - v->vct == v->capacity_ - 1) {
          T** tmp = ptr;
          ptr = v->vct;
          return iterator(tmp, v);
        }
        return iterator(ptr++, v);
      }

      /**
       * TODO ++iter
       */
      iterator &operator++() {
        if (ptr - v->vct == v->capacity_ - 1) {
          ptr = v->vct;
        } else {
          ++ptr;
        }
        return *this;
      }

      /**
       * TODO iter--
       */
      iterator operator--(int) {
        if (ptr == v->vct) {
          ptr = v->vct + v->capacity_ - 1;
          return iterator(v->vct, v);
        }
        return iterator(ptr--, v);
      }

      /**
       * TODO --iter
       */
      iterator &operator--() {
        if (ptr == v->vct) {
          ptr = v->vct + v-> capacity_ - 1;
        } else {
          --ptr;
        }
        return *this;
      }

      /**
       * TODO *it
       */
      T &operator*() const {
        return **ptr;
      }

      /**
       * an operator to check whether two iterators are same (pointing to the same memory address).
       */
      bool operator==(const iterator &rhs) const {
        return ptr == rhs.ptr;
      }

      bool operator==(const const_iterator &rhs) const {
        return ptr == rhs.ptr;
      }

      /**
       * some other operator for iterator.
       */
      bool operator!=(const iterator &rhs) const {
        return ptr != rhs.ptr;
      }

      bool operator!=(const const_iterator &rhs) const {
        return ptr != rhs.ptr;
      }
    };

    /**
     * TODO
     * has same function as iterator, just for a const object.
     */
    class const_iterator {
    public:
      using difference_type = std::ptrdiff_t;
      using value_type = T;
      using pointer = T *;
      using reference = T &;
      using iterator_category = std::output_iterator_tag;

    private:
      /*TODO*/
      const vector* v = nullptr;

    public:
      const T** ptr = nullptr;
      const_iterator() = default;
      const_iterator(const T** p, const vector* v) : v(v), ptr(p) {}
      /**
       * return a new iterator which pointer n-next elements
       * as well as operator-
       */
      const_iterator operator+(const int &n) const {
        //TODO
        if (ptr == nullptr) {
          return const_iterator();
        }
        return const_iterator(ptr + n, v);
      }

      const_iterator operator-(const int &n) const {
        //TODO
        if (ptr == nullptr) {
          return const_iterator();
        }
        return const_iterator(ptr - n, v);
      }

      // return the distance between two iterators,
      // if these two iterators point to different vectors, throw invalid_iterator.
      int operator-(const const_iterator &rhs) const {
        //TODO
        if (v != rhs.v) {
          throw invalid_iterator();
        }
        return ptr - rhs.ptr;
      }
      int operator-(const iterator &rhs) const {
        //TODO
        if (v != rhs.v) {
          throw invalid_iterator();
        }
        return ptr - rhs.ptr;
      }

      const_iterator &operator+=(const int &n) {
        //TODO
        ptr += n;
        return *this;
      }

      const_iterator &operator-=(const int &n) {
        //TODO
        ptr -= n;
        return *this;
      }

      /**
       * TODO iter++
       */
      const_iterator operator++(int) {
        return const_iterator(ptr++, v);
      }

      /**
       * TODO ++iter
       */
      const_iterator &operator++() {
        ++ptr;
        return *this;
      }

      /**
       * TODO iter--
       */
      const_iterator operator--(int) {
        return const_iterator(ptr--, v);
      }

      /**
       * TODO --iter
       */
      const_iterator &operator--() {
        --ptr;
        return *this;
      }

      /**
       * TODO *it
       */
      const T &operator*() const {
        return **ptr;
      }

      /**
       * an operator to check whether two iterators are same (pointing to the same memory address).
       */
      bool operator==(const iterator &rhs) const {
        return ptr == rhs.ptr;
      }

      bool operator==(const const_iterator &rhs) const {
        return ptr == rhs.ptr;
      }

      /**
       * some other operator for iterator.
       */
      bool operator!=(const iterator &rhs) const {
        return ptr != rhs.ptr;
      }

      bool operator!=(const const_iterator &rhs) const {
        return ptr != rhs.ptr;
      }
    };

    /**
     * TODO Constructs
     * At least two: default constructor, copy constructor
     */
    vector() {
      capacity_ = 256;
      vct = new T*[capacity_];
      beg_ind = 0;
      /*for (size_t i = 0; i < 256; ++i) {
        vct[i] = nullptr;
      }*/
    } // initially new a space of size of 256
    vector(const vector &other) : size_(other.size_), capacity_(other.capacity_) {
      vct = new T*[capacity_];
      beg_ind = 0;
      for (size_t i = 0; i < size_; ++i) {
        vct[i] = new T(other[i]); // new an object with other's value
      }
      /*for (size_t i = size_; i < capacity_; ++i) {
        vct[i] = nullptr;
      }*/
    }

    /**
     * TODO Destructor
     */
    ~vector() {
      clear();
      delete[] vct;
      vct = nullptr;
    }

    /**
     * TODO Assignment operator
     */
    vector &operator=(const vector &other) {
      if (this == &other) {
        return *this;
      }
      clear();
      delete[] vct;
      vct = new T*[other.capacity_];
      beg_ind = 0;
      for (size_t i = 0; i < other.size_; ++i) {
        vct[i] = new T(other[i]);
      }
      size_ = other.size_;
      capacity_ = other.capacity_;
      return *this;
    }

    // expand the space
    void DoubleSpace() {
      T** new_vct = new T*[capacity_ * 2];
      for (size_t i = 0; i < size_; ++i) {
        new_vct[i] = new T(**((beg_ind + i) % capacity_ + vct));
      }
      /*for (size_t i = size_; i < capacity_ * 2; ++i) {
        new_vct[i] = nullptr;
      }*/
      capacity_ *= 2;
      const size_t tmp_size = size_;
      clear();
      size_ = tmp_size;
      delete[] vct;
      vct = new_vct;
      beg_ind = 0;
    }

    /**
     * assigns specified element with bounds checking
     * throw index_out_of_bound if pos is not in [0, size)
     */
    T &at(const size_t &pos) {
      if (pos >= size_) {
        throw index_out_of_bound();
      }
      return **((beg_ind + pos) % capacity_ + vct);
    }

    const T &at(const size_t &pos) const {
      if (pos >= size_) {
        throw index_out_of_bound();
      }
      return **((beg_ind + pos) % capacity_ + vct);
    }

    /**
     * assigns specified element with bounds checking
     * throw index_out_of_bound if pos is not in [0, size)
     * !!! Pay attentions
     *   In STL this operator does not check the boundary, but I want you to do.
     */
    T &operator[](const size_t &pos) {
      if (pos >= size_) {
        throw index_out_of_bound();
      }
      return **((beg_ind + pos) % capacity_ + vct);
    }

    const T &operator[](const size_t &pos) const {
      if (pos >= size_) {
        throw index_out_of_bound();
      }
      return **((beg_ind + pos) % capacity_ + vct);
    }

    /**
     * access the first element.
     * throw container_is_empty if size == 0
     */
    const T &front() const {
      if (size_ == 0) {
        throw container_is_empty();
      }
      return *vct[beg_ind];
    }

    /**
     * access the last element.
     * throw container_is_empty if size == 0
     */
    const T &back() const {
      if (size_ == 0) {
        throw container_is_empty();
      }
      return (*this)[size_ - 1];
    }

    /**
     * returns an iterator to the beginning.
     */
    iterator begin() {
      return iterator(vct + beg_ind, this);
    }

    const_iterator begin() const {
      return const_iterator(const_cast<const T**>(vct + beg_ind), const_cast<const vector*>(this));
    }

    const_iterator cbegin() const {
      return const_iterator(const_cast<const T**>(vct + beg_ind), const_cast<const vector*>(this));
    }

    /**
     * returns an iterator to the end.
     */
    iterator end() {
      return iterator((beg_ind + size_) % capacity_ + vct, this);
    }

    const_iterator end() const {
      return const_iterator(const_cast<const T**>((beg_ind + size_) % capacity_ + vct), const_cast<const vector*>(this));
    }

    const_iterator cend() const {
      return const_iterator(const_cast<const T**>((beg_ind + size_) % capacity_ + vct), const_cast<const vector*>(this));
    }

    /**
     * checks whether the container is empty
     */
    bool empty() const {
      return size_ == 0;
    }

    /**
     * returns the number of elements
     */
    size_t size() const {
      return size_;
    }

    /**
     * clears the contents
     */
    void clear() {
      if (vct == nullptr) {
        return;
      }
      for (size_t i = 0; i < size_; ++i) {
        delete vct[(beg_ind + i) % capacity_]; // *((beg_ind + i) % capacity_ + vct);
        //vct[(beg_ind + i) % capacity_] = nullptr;
      }
      beg_ind = 0;
      size_ = 0;
    }

    /**
     * inserts value at index ind.
     * after inserting, this->at(ind) == value
     * returns an iterator pointing to the inserted value.
     * throw index_out_of_bound if ind > size (in this situation ind can be size because after inserting the size will increase 1.)
     */
    iterator insert(const size_t &ind, const T &value) {
      if (ind > size_) {
        throw index_out_of_bound();
      }
      if (size_ == capacity_) {
        T** new_vct = new T*[capacity_ * 2];
        for (size_t i = 0; i < ind; ++i) {
          new_vct[i] = new T(*vct[(beg_ind + i) % capacity_]); // new T(**((beg + i - vct) % capacity_ + vct));
        }
        new_vct[ind] = new T(value);
        for (size_t i = ind; i < size_; ++i) {
          new_vct[i + 1] = new T(*vct[(beg_ind + i) % capacity_]);
        }
        capacity_ *= 2;
        /*for (size_t i = size_ + 1; i < capacity_; ++i) {
          new_vct[i] = nullptr;
        }*/
        const size_t tmp_size = size_ + 1;
        clear();
        size_ = tmp_size;
        delete[] vct;
        vct = new_vct;
        beg_ind = 0;
        return iterator(vct + (beg_ind + ind) % capacity_, this);
      }
      if (ind < size_ / 2) {
        vct[(beg_ind - 1 + capacity_) % capacity_] = new T(value);
        for (size_t i = 0; i < ind; ++i) {
          *vct[(beg_ind + i - 1 + capacity_) % capacity_] = *vct[(beg_ind + i) % capacity_];
        }
        beg_ind = (beg_ind - 1 + capacity_) % capacity_;
        *vct[(beg_ind + ind) % capacity_] = value;
        ++size_;
        return iterator(vct + (beg_ind + ind) % capacity_, this);
      }
      vct[(beg_ind + size_) % capacity_] = new T(value);
      for (size_t i = size_; i > ind; --i) {
        *vct[(beg_ind + i) % capacity_] = *vct[(beg_ind + i - 1) % capacity_];
      }
      *vct[(beg_ind + ind) % capacity_] = value;
      ++size_;
      return iterator(vct + (beg_ind + ind) % capacity_, this);
    }

    /**
     * inserts value before pos
     * returns an iterator pointing to the inserted value.
     */
    iterator insert(iterator pos, const T &value) {
      const size_t ind = (pos - begin() + capacity_) % capacity_;
      if (ind > size_) {
        throw index_out_of_bound();
      }
      return insert(ind, value);
    }

    /**
     * removes the element with index ind.
     * return an iterator pointing to the following element.
     * throw index_out_of_bound if ind >= size
     */
    iterator erase(const size_t &ind) {
      if (ind >= size_) {
        throw index_out_of_bound();
      }
      if (size_ == 1) {
        clear();
        return end();
      }
      if (ind < size_ / 2) {
        for (size_t i = ind; i > 0; --i) {
          *vct[(beg_ind + i) % capacity_] = *vct[(beg_ind + i - 1) % capacity_];
        }
        delete vct[beg_ind];
        vct[beg_ind] = nullptr;
        beg_ind = (beg_ind + 1) % capacity_;
        --size_;
        return iterator(vct + beg_ind, this);
      }
      for (size_t i = ind + 1; i < size_; ++i) {
        *vct[(beg_ind + i - 1) % capacity_] = *vct[(beg_ind + i) % capacity_];
      }
      --size_;
      delete vct[(beg_ind + size_) % capacity_];
      vct[(beg_ind + size_) % capacity_] = nullptr;
      return iterator(vct + (beg_ind + size_) % capacity_, this);
    }

    /**
     * removes the element at pos.
     * return an iterator pointing to the following element.
     * If the iterator pos refers the last element, the end() iterator is returned.
     */
    iterator erase(iterator pos) {
      const size_t ind = (pos - begin() + capacity_) % capacity_;
      if (ind >= size_) {
        throw index_out_of_bound();
      }
      if (size_ == 1) {
        clear();
        return end();
      }
      return erase(ind);
    }

    /**
     * adds an element to the end.
     */
    void push_back(const T &value) {
      if (size_ == capacity_) {
        DoubleSpace();
      }
      vct[(beg_ind + (size_++)) % capacity_] = new T(value);
    }

    /**
     * remove the last element from the end.
     * throw container_is_empty if size() == 0
     */
    void pop_back() {
      if (size_ == 0) {
        throw container_is_empty();
      }
      if (size_ == 1) {
        clear();
      } else {
        delete vct[(beg_ind + size_ - 1) % capacity_];
        //vct[(beg_ind + size_ - 1) % capacity_] = nullptr;
        --size_;
      }
    }
  };
}

#endif
