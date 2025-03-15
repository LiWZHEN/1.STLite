#ifndef SJTU_PRIORITY_QUEUE_HPP
#define SJTU_PRIORITY_QUEUE_HPP

#include <cstddef>
#include <functional>
#include "exceptions.hpp"

namespace sjtu {
  /**
   * @brief a container like std::priority_queue which is a heap internal.
   * **Exception Safety**: The `Compare` operation might throw exceptions for certain data.
   * In such cases, any ongoing operation should be terminated, and the priority queue should be restored to its original state before the operation began.
   */
  template<typename T, class Compare = std::less<T> >
  class priority_queue {
  public:
    struct node {
      T *value = nullptr;
      node *left = nullptr;
      node *right = nullptr;
      size_t distance = 0;

      node() = default;

      explicit node(const T &v) {
        value = new T(v);
      }
    };

    node *root = nullptr;
    size_t num = 0;
    Compare ls;

    /**
     * @brief default constructor
     */
    priority_queue() {
      root = nullptr;
      num = 0;
    }

    void copy(node *target, node *source) {
      target->value = new T(*(source->value));
      target->distance = source->distance;
      if (source->left) {
        target->left = new node;
        copy(target->left, source->left);
      }
      if (source->right) {
        target->right = new node;
        copy(target->right, source->right);
      }
    }

    /**
     * @brief copy constructor
     * @param other the priority_queue to be copied
     */
    priority_queue(const priority_queue &other) {
      num = other.num;
      if (num == 0) {
        return;
      }
      root = new node;
      copy(root, other.root);
    }

    void clear(node *&start, size_t &n) {
      if (start == nullptr) {
        return;
      }
      if (start->left) {
        clear(start->left, n);
        start->left = nullptr;
      }
      if (start->right) {
        clear(start->right, n);
        start->right = nullptr;
      }
      delete start->value;
      start->value = nullptr;
      delete start;
      start = nullptr;
      n = 0;
    }

    /**
     * @brief deconstructor
     */
    ~priority_queue() {
      if (root == nullptr) {
        return;
      }
      clear(root, num);
    }

    /**
     * @brief Assignment operator
     * @param other the priority_queue to be assigned from
     * @return a reference to this priority_queue after assignment
     */
    priority_queue &operator=(const priority_queue &other) {
      if (this == &other) {
        return *this;
      }
      if (num) {
        clear(root, num);
      }
      num = other.num;
      if (num == 0) {
        root = nullptr;
        return *this;
      }
      if (root == nullptr) {
        root = new node;
      }
      copy(root, other.root);
      return *this;
    }

    /**
     * @brief get the top element of the priority queue.
     * @return a reference of the top element.
     * @throws container_is_empty if empty() returns true
     */
    const T &top() const {
      if (num == 0) {
        throw container_is_empty();
      }
      return *(root->value);
    }

    /**
     * @brief return the number of elements in the priority queue.
     * @return the number of elements.
     */
    size_t size() const {
      return num;
    }

    /**
     * @brief check if the container is empty.
     * @return true if it is empty, false otherwise.
     */
    bool empty() const {
      return num == 0;
    }

    void merging(node *&branch, node *&add, bool &flag) {
      if (branch == nullptr) {
        branch = add;
        add = nullptr;
        return;
      }
      if (add == nullptr) {
        return;
      }
      try {
        if (ls(*(branch->value), *(add->value))) {
          node *tmp = branch;
          branch = add;
          add = tmp;
        }
      } catch (...) {
        flag = false;
        size_t tmp;
        clear(add, tmp);
        throw exception();
        return;
      }

      if (branch->right == nullptr) {
        branch->right = add;
        if (branch->left == nullptr) {
          branch->left = branch->right;
          branch->right = nullptr;
          branch->distance = 0;
        } else {
          if (branch->left->distance < branch->right->distance) {
            node *tmp = branch->left;
            branch->left = branch->right;
            branch->right = tmp;
          }
          branch->distance = branch->right->distance + 1;
        }
        return;
      }
      if (add == nullptr) {
        return;
      }
      try {
        if (ls(*(branch->right->value), *(add->value))) {
          node *tmp = branch->right;
          branch->right = add;
          add = tmp;
        }
      } catch (...) {
        flag = false;
        size_t tmp;
        clear(add, tmp);
        throw exception();
        return;
      }
      merging(branch->right, add, flag);
      if (branch->left->distance < branch->right->distance) {
        node *tmp = branch->left;
        branch->left = branch->right;
        branch->right = tmp;
      }
      branch->distance = branch->right->distance + 1;
    }

    /**
     * @brief merge another priority_queue into this one.
     * The other priority_queue will be cleared after merging.
     * The complexity is at most O(logn).
     * @param other the priority_queue to be merged.
     */
    void merge(priority_queue &other) {
      // num += other.num;
      if (other.num == 0) {
        return;
      }
      if (num == 0) {
        root = other.root;
        num = other.num;
        other.root = nullptr;
        other.num = 0;
        return;
      }
      node *branch, *add;
      try {
        if (ls(*(root->value), *(other.root->value))) {
          branch = other.root;
          add = root;
        } else {
          branch = root;
          add = other.root;
        }
        num += other.num;
      } catch (...) {
        throw runtime_error(); // todo
      }
      bool valid = true;
      merging(branch, add, valid);
      root = branch;
      other.root = nullptr;
      other.num = 0;
    }

    /**
     * @brief push new element to the priority queue.
     * @param e the element to be pushed
     */
    void push(const T &e) {
      if (root == nullptr) {
        root = new node;
        root->value = new T(e);
        num = 1;
        return;
      }
      node *add = new node;
      add->value = new T(e);
      bool valid = true;
      try {
        merging(root, add, valid);
      } catch (...) {
        throw runtime_error();
      }
      if (valid) {
        ++num;
      }
    }

    /**
     * @brief delete the top element from the priority queue.
     * @throws container_is_empty if empty() returns true
     */
    void pop() {
      if (num == 0) {
        throw container_is_empty();
      }

      --num;
      node *l = root->left, *r = root->right;

      if (r == nullptr) {
        root->left = nullptr;
        root->right = nullptr;
        delete root->value;
        root->value = nullptr;
        delete root;

        root = l;
        return;
      }

      node *branch, *add;
      try {
        if (ls(*(l->value), *(r->value))) {
          branch = r;
          add = l;
        } else {
          branch = l;
          add = r;
        }
      } catch (...) {
        ++num;
        throw runtime_error();
        return;
      }

      root->left = nullptr;
      root->right = nullptr;
      delete root->value;
      root->value = nullptr;
      delete root;
      root = nullptr;

      bool valid = true;
      merging(branch, add, valid);
      root = branch;
    }
  };
}

#endif
