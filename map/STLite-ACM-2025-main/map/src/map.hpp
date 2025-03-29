/**
 * implement a container like std::map
 */
#ifndef SJTU_MAP_HPP
#define SJTU_MAP_HPP

// only for std::less<T>
#include <functional>
#include <cstddef>
#include "utility.hpp"
#include "exceptions.hpp"

namespace sjtu {
  class key_already_exist : public exception {
    /*_________________________*/
  };

  template<
    class Key,
    class T,
    class Compare = std::less<Key> >
  class map {
  public:

    /**
     * the internal type of data.
     * it should have a default constructor, a copy constructor.
     * You can use sjtu::map as value_type by typedef.
     */
    typedef pair<const Key, T> value_type;

    struct node {
      value_type *vt = nullptr;
      node *left = nullptr;
      node *right = nullptr;
      node *parent = nullptr;
      int height = 0;
    };

    node *root = nullptr;
    int num = 0;
    node *end_node = nullptr;
    Compare ls;

  private:
    int ComputeHeight(node *n) {
      if (n == nullptr) {
        return -1;
      }
      return n->height;
    }

    void UpdateHeight(node *n) {
      if (n != nullptr) {
        n->height = std::max(ComputeHeight(n->left), ComputeHeight(n->right)) + 1;
      }
    }

    int ComputeBalance(node *n) {
      if (n == nullptr) {
        return 0;
      }
      return ComputeHeight(n->left) - ComputeHeight(n->right);
    }

    void RRotate(node *n) {
      node *origin_parent = n->parent, *origin_l = n->left, *origin_lr = n->left->right;
      n->parent = origin_l;
      n->left = origin_lr;
      origin_l->parent = origin_parent;
      origin_l->right = n;
      if (origin_lr) {
        origin_lr->parent = n;
      }
      if (origin_parent == nullptr) {
        root = origin_l;
      } else if (origin_parent->left == n) {
        origin_parent->left = origin_l;
      } else {
        origin_parent->right = origin_l;
      }
      UpdateHeight(n);
      UpdateHeight(origin_l);
      /*node *update = origin_l;
      while (update->parent) {
        update = update->parent;
        UpdateHeight(update);
      }*/
    }

    void LRotate(node *n) {
      node *origin_parent = n->parent, *origin_r = n->right, *origin_rl = n->right->left;
      n->parent = origin_r;
      n->right = origin_rl;
      origin_r->left = n;
      origin_r->parent = origin_parent;
      if (origin_rl) {
        origin_rl->parent = n;
      }
      if (origin_parent == nullptr) {
        root = origin_r;
      } else if (origin_parent->left == n) {
        origin_parent->left = origin_r;
      } else {
        origin_parent->right = origin_r;
      }
      UpdateHeight(n);
      UpdateHeight(origin_r);
      /*node *update = origin_r;
      while (update->parent) {
        update = update->parent;
        UpdateHeight(update);
      }*/
    }

    /**
     * remember to set branch's parent's child pointer nullptr
     */
    void Clear(node *branch) {
      if (branch == nullptr) {
        return;
      }
      if (branch->left) {
        Clear(branch->left);
        branch->left = nullptr;
      }
      if (branch->right) {
        Clear(branch->right);
        branch->right = nullptr;
      }
      delete branch->vt;
      delete branch;
    }

    /**
     * target is already a nullptr
     */
    void Copy(node *&target, node *source, node *parent) {
      if (source == nullptr) {
        return;
      }
      target = new node;
      target->vt = new value_type(*source->vt);
      target->height = source->height;
      target->parent = parent;
      if (source->left) {
        Copy(target->left, source->left, target);
      }
      if (source->right) {
        Copy(target->right, source->right, target);
      }
    }

    /**
     * only insert, update height and num, not necessarily balanced.
     * throw key_already_exist if key already exist
     */
    node *Insert(Key key, T value) {
      if (root == nullptr) {
        root = new node;
        root->height = 0;
        root->vt = new value_type({key, value});
        ++num;
        return root;
      }
      node *ptr = root;
      node *parent = root;
      bool l_son = false;
      while (ptr) {
        if (ls(key, ptr->vt->first)) {
          parent = ptr;
          ptr = ptr->left;
          l_son = true;
        } else if (ls(ptr->vt->first, key)) {
          parent = ptr;
          ptr = ptr->right;
          l_son = false;
        } else {
          throw key_already_exist();
        }
      }
      if (l_son) {
        parent->left = new node;
        ptr = parent->left;
        ptr->vt = new value_type({key, value});
        ptr->parent = parent;
      } else {
        parent->right = new node;
        ptr = parent->right;
        ptr->vt = new value_type({key, value});
        ptr->parent = parent;
      }
      /*node *updater = ptr;
      while (updater) {
        UpdateHeight(updater);
        updater = updater->parent;
      }*/
      ptr->height = 0;
      ++num;
      return ptr;
    }

    void KeepBalance(node *p) {
      if (p == nullptr) {
        return;
      }
      UpdateHeight(p);
      if (ComputeBalance(p) > 1) { // LX
        if (ComputeBalance(p->left) < 0) { // LR
          LRotate(p->left);
          RRotate(p);
        } else { // LL
          RRotate(p);
        }
        KeepBalance(p->parent->parent);
      } else if (ComputeBalance(p) < -1) { // RX
        if (ComputeBalance(p->right) > 0) { // RL
          RRotate(p->right);
          LRotate(p);
        } else { // RR
          LRotate(p);
        }
        KeepBalance(p->parent->parent);
      } else {
        KeepBalance(p->parent);
      }
    }

  public:
    /**
     * see BidirectionalIterator at CppReference for help.
     *
     * if there is anything wrong throw invalid_iterator.
     *     like it = map.begin(); --it;
     *       or it = map.end(); ++end();
     */
    class const_iterator;

    class iterator {
    public:
      node *ptr = nullptr;
      map *mp = nullptr;

      iterator(node *ptr = nullptr, map *mp = nullptr) : ptr(ptr), mp(mp) {
        // TODO
      }

      iterator(const iterator &other) {
        // TODO
        ptr = other.ptr;
        mp = other.mp;
      }

      /**
       * TODO iter++
       */
      iterator operator++(int) {
        if (ptr == mp->end_node) {
          throw invalid_iterator();
        }
        iterator tmp = *this;
        if (ptr->right) {
          ptr = ptr->right;
          while (ptr->left) {
            ptr = ptr->left;
          }
        } else { // the largest in current branch
          bool valid = false;
          while (ptr->parent) {
            if (ptr == ptr->parent->right) {
              ptr = ptr->parent;
            } else {
              ptr = ptr->parent;
              valid = true;
              break;
            }
          }
          if (!valid) {
            ptr = mp->end_node;
          }
        }
        return tmp;
      }

      /**
       * TODO ++iter
       */
      iterator &operator++() {
        if (ptr == mp->end_node) {
          throw invalid_iterator();
        }
        if (ptr->right) {
          ptr = ptr->right;
          while (ptr->left) {
            ptr = ptr->left;
          }
        } else { // the largest in current branch
          bool valid = false;
          while (ptr->parent) {
            if (ptr == ptr->parent->right) {
              ptr = ptr->parent;
            } else {
              ptr = ptr->parent;
              valid = true;
              break;
            }
          }
          if (!valid) {
            ptr = mp->end_node;
          }
        }
        return *this;
      }

      /**
       * TODO iter--
       */
      iterator operator--(int) {
        iterator tmp = *this;
        if (ptr == mp->end_node) {
          if (mp->root == nullptr) {
            throw invalid_iterator();
          }
          ptr = mp->root;
          while (ptr->right) {
            ptr = ptr->right;
          }
        } else {
          if (ptr->left) {
            ptr = ptr->left;
            while (ptr->right) {
              ptr = ptr->right;
            }
          } else { // the smallest in current branch
            bool valid = false;
            while (ptr->parent) {
              if (ptr == ptr->parent->left) {
                ptr = ptr->parent;
              } else {
                ptr = ptr->parent;
                valid = true;
                break;
              }
            }
            if (!valid) {
              throw invalid_iterator();
            }
          }
        }
        return tmp;
      }

      /**
       * TODO --iter
       */
      iterator &operator--() {
        if (ptr == mp->end_node) {
          if (mp->root == nullptr) {
            throw invalid_iterator();
          }
          ptr = mp->root;
          while (ptr->right) {
            ptr = ptr->right;
          }
        } else {
          if (ptr->left) {
            ptr = ptr->left;
            while (ptr->right) {
              ptr = ptr->right;
            }
          } else { // the smallest in current branch
            bool valid = false;
            while (ptr->parent) {
              if (ptr == ptr->parent->left) {
                ptr = ptr->parent;
              } else {
                ptr = ptr->parent;
                valid = true;
                break;
              }
            }
            if (!valid) {
              throw invalid_iterator();
            }
          }
        }
        return *this;
      }

      /**
       * an operator to check whether two iterators are same (pointing to the same memory).
       */
      value_type &operator*() const {
        return *ptr->vt;
      }

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

      /**
       * for the support of it->first.
       * See <http://kelvinh.github.io/blog/2013/11/20/overloading-of-member-access-operator-dash-greater-than-symbol-in-cpp/> for help.
       */
      value_type *operator->() const noexcept {
        return ptr->vt;
      }
    };

    class const_iterator {
      // it should have similar member method as iterator.
      //  and it should be able to construct from an iterator.
    private:
      // data members.
    public:
      const node *ptr = nullptr;
      const map *mp = nullptr;
      const_iterator(const node *ptr = nullptr, const map *mp = nullptr) : ptr(ptr), mp(mp) {
        // TODO
      }

      const_iterator(const const_iterator &other) {
        // TODO
        ptr = other.ptr;
        mp = other.mp;
      }

      const_iterator(const iterator &other) {
        // TODO
        ptr = other.ptr;
        mp = other.mp;
      }

      /**
       * TODO iter++
       */
      const_iterator operator++(int) {
        if (ptr == mp->end_node) {
          throw invalid_iterator();
        }
        const_iterator tmp = *this;
        if (ptr->right) {
          ptr = ptr->right;
          while (ptr->left) {
            ptr = ptr->left;
          }
        } else { // the largest in current branch
          bool valid = false;
          while (ptr->parent) {
            if (ptr == ptr->parent->right) {
              ptr = ptr->parent;
            } else {
              ptr = ptr->parent;
              valid = true;
              break;
            }
          }
          if (!valid) {
            ptr = mp->end_node;
          }
        }
        return tmp;
      }

      /**
       * TODO ++iter
       */
      const_iterator &operator++() {
        if (ptr == mp->end_node) {
          throw invalid_iterator();
        }
        if (ptr->right) {
          ptr = ptr->right;
          while (ptr->left) {
            ptr = ptr->left;
          }
        } else { // the largest in current branch
          bool valid = false;
          while (ptr->parent) {
            if (ptr == ptr->parent->right) {
              ptr = ptr->parent;
            } else {
              ptr = ptr->parent;
              valid = true;
              break;
            }
          }
          if (!valid) {
            ptr = mp->end_node;
          }
        }
        return *this;
      }

      /**
       * TODO iter--
       */
      const_iterator operator--(int) {
        const_iterator tmp = *this;
        if (ptr == mp->end_node) {
          if (mp->root == nullptr) {
            throw invalid_iterator();
          }
          ptr = mp->root;
          while (ptr->right) {
            ptr = ptr->right;
          }
        } else {
          if (ptr->left) {
            ptr = ptr->left;
            while (ptr->right) {
              ptr = ptr->right;
            }
          } else { // the smallest in current branch
            bool valid = false;
            while (ptr->parent) {
              if (ptr == ptr->parent->left) {
                ptr = ptr->parent;
              } else {
                ptr = ptr->parent;
                valid = true;
                break;
              }
            }
            if (!valid) {
              throw invalid_iterator();
            }
          }
        }
        return tmp;
      }

      /**
       * TODO --iter
       */
      const_iterator &operator--() {
        if (ptr == mp->end_node) {
          if (mp->root == nullptr) {
            throw invalid_iterator();
          }
          ptr = mp->root;
          while (ptr->right) {
            ptr = ptr->right;
          }
        } else {
          if (ptr->left) {
            ptr = ptr->left;
            while (ptr->right) {
              ptr = ptr->right;
            }
          } else { // the smallest in current branch
            bool valid = false;
            while (ptr->parent) {
              if (ptr == ptr->parent->left) {
                ptr = ptr->parent;
              } else {
                ptr = ptr->parent;
                valid = true;
                break;
              }
            }
            if (!valid) {
              throw invalid_iterator();
            }
          }
        }
        return *this;
      }

      /**
       * an operator to check whether two iterators are same (pointing to the same memory).
       */
      const value_type &operator*() const {
        return *ptr->vt;
      }

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

      /**
       * for the support of it->first.
       * See <http://kelvinh.github.io/blog/2013/11/20/overloading-of-member-access-operator-dash-greater-than-symbol-in-cpp/> for help.
       */
      const value_type *operator->() const noexcept {
        return ptr->vt;
      }
    };

    /**
     * TODO two constructors
     */
    map() {
      end_node = new node;
    }

    map(const map &other) {
      end_node = new node;
      num = other.num;
      Copy(root, other.root, nullptr);
    }

    /**
     * TODO assignment operator
     */
    map &operator=(const map &other) {
      if (this == &other) {
        return *this;
      }
      num = other.num;
      Clear(root);
      root = nullptr;
      Copy(root, other.root, nullptr);
      return *this;
    }

    /**
     * TODO Destructors
     */
    ~map() {
      Clear(root);
      delete end_node;
    }

    /**
     * TODO
     * access specified element with bounds checking
     * Returns a reference to the mapped value of the element with key equivalent to key.
     * If no such element exists, an exception of type `index_out_of_bound'
     */
    T &at(const Key &key) {
      node *ptr = root;
      while (ptr) {
        if (ls(key, ptr->vt->first)) {
          ptr = ptr->left;
        } else if (ls(ptr->vt->first, key)) {
          ptr = ptr->right;
        } else {
          return ptr->vt->second;
        }
      }
      throw index_out_of_bound();
    }

    const T &at(const Key &key) const {
      node *ptr = root;
      while (ptr) {
        if (ls(key, ptr->vt->first)) {
          ptr = ptr->left;
        } else if (ls(ptr->vt->first, key)) {
          ptr = ptr->right;
        } else {
          return ptr->vt->second;
        }
      }
      throw index_out_of_bound();
    }

    /**
     * TODO
     * access specified element
     * Returns a reference to the value that is mapped to a key equivalent to key,
     *   performing an insertion if such key does not already exist.
     */
    T &operator[](const Key &key) {
      try {
        node *ptr = root;
        while (ptr) {
          if (ls(key, ptr->vt->first)) {
            ptr = ptr->left;
          } else if (ls(ptr->vt->first, key)) {
            ptr = ptr->right;
          } else {
            return ptr->vt->second;
          }
        }
        throw index_out_of_bound();
      } catch (index_out_of_bound &) {
        value_type vt(key, T());
        pair<iterator, bool> inserted = insert(vt);
        return inserted.first.ptr->vt->second;
      }
    }

    /**
     * behave like at() throw index_out_of_bound if such key does not exist.
     */
    const T &operator[](const Key &key) const {
      node *ptr = root;
      while (ptr) {
        if (ls(key, ptr->vt->first)) {
          ptr = ptr->left;
        } else if (ls(ptr->vt->first, key)) {
          ptr = ptr->right;
        } else {
          return ptr->vt->second;
        }
      }
      throw index_out_of_bound();
    }

    /**
     * return an iterator to the beginning
     */
    iterator begin() {
      iterator it;
      it.mp = this;
      if (root == nullptr) {
        it.ptr = end_node;
        return it;
      }
      it.ptr = root;
      while (it.ptr->left) {
        it.ptr = it.ptr->left;
      }
      return it;
    }

    const_iterator cbegin() const {
      const_iterator it;
      it.mp = this;
      if (root == nullptr) {
        it.ptr = end_node;
        return it;
      }
      it.ptr = root;
      while (it.ptr->left) {
        it.ptr = it.ptr->left;
      }
      return it;
    }

    /**
     * return an iterator to the end
     * in fact, it returns past-the-end.
     */
    iterator end() {
      return iterator(end_node, this);
    }

    const_iterator cend() const {
      return const_iterator(end_node, this);
    }

    /**
     * checks whether the container is empty
     * return true if empty, otherwise false.
     */
    bool empty() const {
      return num == 0;
    }

    /**
     * returns the number of elements.
     */
    size_t size() const {
      return num;
    }

    /**
     * clears the contents
     */
    void clear() {
      Clear(root);
      root = nullptr;
      num = 0;
    }

    /**
     * insert an element.
     * return a pair, the first of the pair is
     *   the iterator to the new element (or the element that prevented the insertion),
     *   the second one is true if insert successfully, or false.
     */
    pair<iterator, bool> insert(const value_type &value) {
      node *ptr = nullptr;
      try {
        ptr = Insert(value.first, value.second);
      } catch (key_already_exist &) {
        iterator it = find(value.first);
        return {it, false};
      }
      node *keeper = ptr;
      while (ComputeBalance(keeper) >= -1 && ComputeBalance(keeper) <= 1) {
        UpdateHeight(keeper);
        if (keeper->parent == nullptr) {
          return {iterator(ptr, this), true};
        }
        keeper = keeper->parent;
      }
      UpdateHeight(keeper);
      if (ComputeBalance(keeper) > 1) { // LX
        if (ComputeBalance(keeper->left) < 0) { // LR
          LRotate(keeper->left);
          RRotate(keeper);
        } else { // LL
          RRotate(keeper);
        }
      } else { // RX
        if (ComputeBalance(keeper->right) > 0) { // RL
          RRotate(keeper->right);
          LRotate(keeper);
        } else { // RR
          LRotate(keeper);
        }
      }
      node *updater = keeper->parent;
      while (updater->parent) {
        updater = updater->parent;
        UpdateHeight(updater);
      }
      return {iterator(ptr, this), true};
    }

    /**
     * erase the element at pos.
     *
     * throw if pos pointed to a bad element (pos == this->end() || pos points an element out of this)
     */
    void erase(iterator pos) {
      if (pos.mp != this || pos.ptr == end_node || pos.ptr == nullptr || pos != find(pos.ptr->vt->first)) {
        throw invalid_iterator();
      }
      node *keep_begin = nullptr;
      if (pos.ptr->left == nullptr && pos.ptr->right == nullptr) {
        if (pos.ptr->parent == nullptr) {
          delete pos.ptr->vt;
          delete pos.ptr;
          --num;
          root = nullptr;
          return;
        }
        keep_begin = pos.ptr->parent;
        if (pos.ptr == keep_begin->left) {
          keep_begin->left = nullptr;
          delete pos.ptr->vt;
          delete pos.ptr;
          --num;
        } else {
          keep_begin->right = nullptr;
          delete pos.ptr->vt;
          delete pos.ptr;
          --num;
        }
      } else if (pos.ptr->left == nullptr) {
        if (pos.ptr->parent == nullptr) {
          root = pos.ptr->right;
          root->parent = nullptr;
          delete pos.ptr->vt;
          delete pos.ptr;
          --num;
          return;
        }
        keep_begin = pos.ptr->parent;
        if (pos.ptr == keep_begin->left) {
          keep_begin->left = pos.ptr->right;
          pos.ptr->right->parent = keep_begin;
          delete pos.ptr->vt;
          delete pos.ptr;
          --num;
        } else {
          keep_begin->right = pos.ptr->right;
          pos.ptr->right->parent = keep_begin;
          delete pos.ptr->vt;
          delete pos.ptr;
          --num;
        }
      } else if (pos.ptr->right == nullptr) {
        if (pos.ptr->parent == nullptr) {
          root = pos.ptr->left;
          root->parent = nullptr;
          delete pos.ptr->vt;
          delete pos.ptr;
          --num;
          return;
        }
        keep_begin = pos.ptr->parent;
        if (pos.ptr == keep_begin->left) {
          keep_begin->left = pos.ptr->left;
          pos.ptr->left->parent = keep_begin;
          delete pos.ptr->vt;
          delete pos.ptr;
          --num;
        } else {
          keep_begin->right = pos.ptr->left;
          pos.ptr->left->parent = keep_begin;
          delete pos.ptr->vt;
          delete pos.ptr;
          --num;
        }
      } else {
        node *replacer = pos.ptr->right;
        while (replacer->left) {
          replacer = replacer->left;
        }

        // exchange height
        int h = replacer->height;
        replacer->height = pos.ptr->height;
        pos.ptr->height = h;

        // exchange node
        if (replacer == pos.ptr->right) {
          node *ptr_parent = pos.ptr->parent, *ptr_left = pos.ptr->left, *replacer_right = replacer->right;
          pos.ptr->left = nullptr;
          pos.ptr->right = replacer_right;
          pos.ptr->parent = replacer;
          if (replacer_right) {
            replacer_right->parent = pos.ptr;
          }
          replacer->parent = ptr_parent;
          replacer->left = ptr_left;
          replacer->right = pos.ptr;
          if (ptr_left) {
            ptr_left->parent = replacer;
          }
          if (ptr_parent == nullptr) {
            root = replacer;
          } else if (ptr_parent->left == pos.ptr) {
            ptr_parent->left = replacer;
          } else {
            ptr_parent->right = replacer;
          }
        } else {
          node *ptr_parent = pos.ptr->parent, *ptr_left = pos.ptr->left, *ptr_right = pos.ptr->right;
          node *replacer_parent = replacer->parent, *replacer_left = replacer->left, *replacer_right = replacer->right;
          pos.ptr->left = replacer_left;
          pos.ptr->right = replacer_right;
          pos.ptr->parent = replacer_parent;
          if (replacer_right) {
            replacer_right->parent = pos.ptr;
          }
          if (replacer_left) {
            replacer_left->parent = pos.ptr;
          }
          if (replacer_parent->left == replacer) {
            replacer_parent->left = pos.ptr;
          } else {
            replacer_parent->right = pos.ptr;
          }
          replacer->parent = ptr_parent;
          replacer->left = ptr_left;
          replacer->right = ptr_right;
          if (ptr_left) {
            ptr_left->parent = replacer;
          }
          if (ptr_right) {
            ptr_right->parent = replacer;
          }
          if (ptr_parent == nullptr) {
            root = replacer;
          } else if (ptr_parent->left == pos.ptr) {
            ptr_parent->left = replacer;
          } else {
            ptr_parent->right = replacer;
          }
        }

        if (pos.ptr->left == nullptr && pos.ptr->right == nullptr) {
          if (pos.ptr->parent == nullptr) {
            delete pos.ptr->vt;
            delete pos.ptr;
            --num;
            root = nullptr;
            return;
          }
          keep_begin = pos.ptr->parent;
          if (pos.ptr == keep_begin->left) {
            keep_begin->left = nullptr;
            delete pos.ptr->vt;
            delete pos.ptr;
            --num;
          } else {
            keep_begin->right = nullptr;
            delete pos.ptr->vt;
            delete pos.ptr;
            --num;
          }
        } else if (pos.ptr->left == nullptr) {
          if (pos.ptr->parent == nullptr) {
            root = pos.ptr->right;
            root->parent = nullptr;
            delete pos.ptr->vt;
            delete pos.ptr;
            --num;
            return;
          }
          keep_begin = pos.ptr->parent;
          if (pos.ptr == keep_begin->left) {
            keep_begin->left = pos.ptr->right;
            pos.ptr->right->parent = keep_begin;
            delete pos.ptr->vt;
            delete pos.ptr;
            --num;
          } else {
            keep_begin->right = pos.ptr->right;
            pos.ptr->right->parent = keep_begin;
            delete pos.ptr->vt;
            delete pos.ptr;
            --num;
          }
        } else if (pos.ptr->right == nullptr) {
          if (pos.ptr->parent == nullptr) {
            root = pos.ptr->left;
            root->parent = nullptr;
            delete pos.ptr->vt;
            delete pos.ptr;
            --num;
            return;
          }
          keep_begin = pos.ptr->parent;
          if (pos.ptr == keep_begin->left) {
            keep_begin->left = pos.ptr->left;
            pos.ptr->left->parent = keep_begin;
            delete pos.ptr->vt;
            delete pos.ptr;
            --num;
          } else {
            keep_begin->right = pos.ptr->left;
            pos.ptr->left->parent = keep_begin;
            delete pos.ptr->vt;
            delete pos.ptr;
            --num;
          }
        }
      }
      KeepBalance(keep_begin);
    }

    /**
     * Returns the number of elements with key
     *   that compares equivalent to the specified argument,
     *   which is either 1 or 0
     *     since this container does not allow duplicates.
     * The default method of check the equivalence is !(a < b || b > a)
     */
    size_t count(const Key &key) const {
      node *ptr = root;
      while (ptr) {
        if (ls(key, ptr->vt->first)) {
          ptr = ptr->left;
        } else if (ls(ptr->vt->first, key)) {
          ptr = ptr->right;
        } else {
          return 1;
        }
      }
      return 0;
    }

    /**
     * Finds an element with key equivalent to key.
     * key value of the element to search for.
     * Iterator to an element with key equivalent to key.
     *   If no such element is found, past-the-end (see end()) iterator is returned.
     */
    iterator find(const Key &key) {
      node *ptr = root;
      while (ptr) {
        if (ls(key, ptr->vt->first)) {
          ptr = ptr->left;
        } else if (ls(ptr->vt->first, key)) {
          ptr = ptr->right;
        } else {
          return iterator(ptr, this);
        }
      }
      return iterator(end_node, this);
    }

    const_iterator find(const Key &key) const {
      const node *ptr = root;
      while (ptr) {
        if (ls(key, ptr->vt->first)) {
          ptr = ptr->left;
        } else if (ls(ptr->vt->first, key)) {
          ptr = ptr->right;
        } else {
          return const_iterator(ptr, this);
        }
      }
      return const_iterator(end_node, this);
    }
  };
}

#endif
