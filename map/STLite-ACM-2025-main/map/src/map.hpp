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
// #include <iostream> // todo: for debugging

namespace sjtu {
  class key_already_exist : public exception {
    /*__________________________*/
  };

  class key_not_exist : public exception {
    /*__________________________*/
  };

  template<class Key, class T, class Compare = std::less<Key> >
  class map {
  public:
    /**
     * the internal type of data.
     * it should have a default constructor, a copy constructor.
     * You can use sjtu::map as value_type by typedef.
     */
    typedef pair<const Key, T> value_type;

    struct node {
      bool red = true;
      node *parent = nullptr, *left = nullptr, *right = nullptr;
      value_type *vt = nullptr;

      node() = default;

      node(const bool is_red, const value_type &v, node *p = nullptr, node *l = nullptr, node *r = nullptr) {
        red = is_red;
        vt = new value_type(v);
        parent = p;
        left = l;
        right = r;
      }
    };

  private:
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

    void Copy(node *&target, node *source, node *parent) {
      if (source == nullptr) {
        return;
      }
      target = new node(source->red, *source->vt, parent);
      if (source->left) {
        Copy(target->left, source->left, target);
      }
      if (source->right) {
        Copy(target->right, source->right, target);
      }
    }

    /**
     * return nullptr if key is already existed
     */
    node *Insert(const value_type &v) {
      if (num == 0) {
        root = new node(false, v);
        ++num;
        return root;
      }
      node *p = root;
      while (true) {
        if (ls(v.first, p->vt->first)) {
          if (!p->left) {
            p->left = new node(true, v, p);
            ++num;
            return p->left;
          }
          p = p->left;
        } else if (ls(p->vt->first, v.first)) {
          if (!p->right) {
            p->right = new node(true, v, p);
            ++num;
            return p->right;
          }
          p = p->right;
        } else {
          return nullptr;
        }
      }
    }

    /**
     * throw key_not_exist if there is no target key
     */
    node *ReplaceAndLocate(const value_type &v) {
      node *n = root;
      while (n) {
        if (ls(v.first, n->vt->first)) {
          n = n->left;
        } else if (ls(n->vt->first, v.first)) {
          n = n->right;
        } else {
          if (n->left == nullptr || n->right == nullptr) {
            return n;
          }
          node *replacer = n->right;
          while (replacer->left) {
            replacer = replacer->left;
          }

          // exchange color
          bool c = replacer->red;
          replacer->red = n->red;
          n->red = c;

          // exchange node
          if (replacer == n->right) {
            node *ptr_parent = n->parent, *ptr_left = n->left, *replacer_right = replacer->right;
            n->left = nullptr;
            n->right = replacer_right;
            n->parent = replacer;
            if (replacer_right) {
              replacer_right->parent = n;
            }
            replacer->parent = ptr_parent;
            replacer->left = ptr_left;
            replacer->right = n;
            if (ptr_left) {
              ptr_left->parent = replacer;
            }
            if (ptr_parent == nullptr) {
              root = replacer;
            } else if (ptr_parent->left == n) {
              ptr_parent->left = replacer;
            } else {
              ptr_parent->right = replacer;
            }
          } else {
            node *ptr_parent = n->parent, *ptr_left = n->left, *ptr_right = n->right;
            node *replacer_parent = replacer->parent, *replacer_left = replacer->left, *replacer_right = replacer->
                right;
            n->left = replacer_left;
            n->right = replacer_right;
            n->parent = replacer_parent;
            if (replacer_right) {
              replacer_right->parent = n;
            }
            if (replacer_left) {
              replacer_left->parent = n;
            }
            if (replacer_parent->left == replacer) {
              replacer_parent->left = n;
            } else {
              replacer_parent->right = n;
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
            } else if (ptr_parent->left == n) {
              ptr_parent->left = replacer;
            } else {
              ptr_parent->right = replacer;
            }
          }
          return n;
        }
      }
      throw key_not_exist();
    }

    void AfterDeleteBlackNode(node *parent, bool is_left) {
      if (parent == nullptr) {
        return;
      }
      if (is_left) {
        node *brother = parent->right;
        if (brother && brother->red) {
          parent->red = true;
          brother->red = false;
          RR(parent);
        }
        brother = parent->right;
        const bool left_is_red = brother->left && brother->left->red,
            right_is_red = brother->right && brother->right->red;
        if (!left_is_red && !right_is_red) {
          if (parent->red) {
            parent->red = false;
            brother->red = true;
          } else {
            brother->red = true;
            if (parent->parent) {
              node *grandpa = parent->parent;
              if (parent == grandpa->left) {
                AfterDeleteBlackNode(grandpa, true);
              } else {
                AfterDeleteBlackNode(grandpa, false);
              }
            }
          }
        } else if (right_is_red) {
          brother->red = parent->red;
          parent->red = false;
          brother->right->red = false;
          RR(parent);
        } else {
          brother->left->red = parent->red;
          parent->red = false;
          RL(parent);
        }
      } else {
        node *brother = parent->left;
        if (brother->red) {
          parent->red = true;
          brother->red = false;
          LL(parent);
        }
        brother = parent->left;
        const bool left_is_red = brother->left && brother->left->red,
            right_is_red = brother->right && brother->right->red;
        if (!left_is_red && !right_is_red) {
          if (parent->red) {
            parent->red = false;
            brother->red = true;
          } else {
            brother->red = true;
            if (parent->parent) {
              node *grandpa = parent->parent;
              if (parent == grandpa->left) {
                AfterDeleteBlackNode(grandpa, true);
              } else {
                AfterDeleteBlackNode(grandpa, false);
              }
            }
          }
        } else if (left_is_red) {
          brother->red = parent->red;
          parent->red = false;
          brother->left->red = false;
          LL(parent);
        } else {
          brother->right->red = parent->red;
          parent->red = false;
          LR(parent);
        }
      }
    }

    inline void LL(node *p) {
      node *origin_parent = p->parent, *new_root = p->left, *lr = new_root->right;
      p->parent = new_root;
      p->left = lr;
      new_root->parent = origin_parent;
      new_root->right = p;
      if (lr) {
        lr->parent = p;
      }
      if (origin_parent == nullptr) {
        root = new_root;
      } else if (origin_parent->left == p) {
        origin_parent->left = new_root;
      } else {
        origin_parent->right = new_root;
      }
    }

    inline void RR(node *p) {
      node *origin_parent = p->parent, *new_root = p->right, *rl = new_root->left;
      p->parent = new_root;
      p->right = rl;
      new_root->parent = origin_parent;
      new_root->left = p;
      if (rl) {
        rl->parent = p;
      }
      if (origin_parent == nullptr) {
        root = new_root;
      } else if (origin_parent->left == p) {
        origin_parent->left = new_root;
      } else {
        origin_parent->right = new_root;
      }
    }

    inline void LR(node *p) {
      RR(p->left);
      LL(p);
    }

    inline void RL(node *p) {
      LL(p->right);
      RR(p);
    }

    void InsertBalance(node *p) {
      while (p->parent && p->parent->red) {
        // father is red
        node *father = p->parent, *grandpa = father->parent;
        if (grandpa->left == father) {
          // father is the left son
          if (grandpa->right && grandpa->right->red) {
            // uncle is red
            father->red = false;
            grandpa->right->red = false;
            grandpa->red = true;
            p = grandpa;
          } else {
            // uncle is black
            grandpa->red = true;
            if (p == father->left) {
              father->red = false;
              LL(grandpa);
            } else {
              p->red = false;
              LR(grandpa);
            }
            return;
          }
        } else {
          // father is the right son
          if (grandpa->left && grandpa->left->red) {
            // uncle is red
            father->red = false;
            grandpa->left->red = false;
            grandpa->red = true;
            p = grandpa;
          } else {
            // uncle is black
            grandpa->red = true;
            if (p == father->right) {
              father->red = false;
              RR(grandpa);
            } else {
              p->red = false;
              RL(grandpa);
            }
            return;
          }
        }
      }
      if (p->parent == nullptr) {
        p->red = false;
      }
    }

    void RemoveAndBalance(node *p) {
      if (p->left == nullptr && p->right == nullptr) {
        if (p->parent == nullptr) {
          delete p->vt;
          delete p;
          root = nullptr;
          num = 0;
        } else {
          const bool is_red = p->red;
          bool is_left;
          node *parent = p->parent;
          if (parent->left == p) {
            parent->left = nullptr;
            is_left = true;
          } else {
            parent->right = nullptr;
            is_left = false;
          }
          delete p->vt;
          delete p;
          --num;
          if (!is_red) {
            AfterDeleteBlackNode(parent, is_left);
          }
        }
      } else if (p->left == nullptr) {
        node *origin_parent = p->parent, *r = p->right;
        r->red = false;
        r->parent = origin_parent;
        if (origin_parent == nullptr) {
          root = r;
        } else if (origin_parent->left == p) {
          origin_parent->left = r;
        } else {
          origin_parent->right = r;
        }
        --num;
        delete p->vt;
        delete p;
      } else {
        // p->right == nullptr
        node *origin_parent = p->parent, *l = p->left;
        l->red = false;
        l->parent = origin_parent;
        if (origin_parent == nullptr) {
          root = l;
        } else if (origin_parent->left == p) {
          origin_parent->left = l;
        } else {
          origin_parent->right = l;
        }
        --num;
        delete p->vt;
        delete p;
      }
    }

  public:
    node *root = nullptr;
    node *end_node = nullptr;
    node *smallest = nullptr;
    node *largest = nullptr;
    int num = 0;
    Compare ls;

    /**
     * see BidirectionalIterator at CppReference for help.
     *
     * if there is anything wrong throw invalid_iterator.
     *     like it = map.begin(); --it;
     *       or it = map.end(); ++end();
     */
    class const_iterator;

    class iterator {
      /**
       * TODO add data members
       *   just add whatever you want.
       */

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
        } else {
          // the largest in current branch
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
        } else {
          // the largest in current branch
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
          ptr = mp->largest;
        } else {
          if (ptr->left) {
            ptr = ptr->left;
            while (ptr->right) {
              ptr = ptr->right;
            }
          } else {
            // the smallest in current branch
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
          ptr = mp->largest;
        } else {
          if (ptr->left) {
            ptr = ptr->left;
            while (ptr->right) {
              ptr = ptr->right;
            }
          } else {
            // the smallest in current branch
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
          ptr = mp->largest;
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
          ptr = mp->largest;
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
      if (root) {
        smallest = root;
        while (smallest->left) {
          smallest = smallest->left;
        }
        largest = root;
        while (largest->right) {
          largest = largest->right;
        }
      }
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
      smallest = nullptr;
      largest = nullptr;
      Copy(root, other.root, nullptr);
      if (root) {
        smallest = root;
        while (smallest->left) {
          smallest = smallest->left;
        }
        largest = root;
        while (largest->right) {
          largest = largest->right;
        }
      }
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
      value_type vt(key, T());
      pair<iterator, bool> inserted = insert(vt);
      return inserted.first.ptr->vt->second;
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
      } else {
        it.ptr = smallest;
      }
      return it;
    }

    const_iterator cbegin() const {
      const_iterator it;
      it.mp = this;
      if (root == nullptr) {
        it.ptr = end_node;
      } else {
        it.ptr = smallest;
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
      node *ptr = Insert(value);
      if (ptr == nullptr) {
        iterator it = find(value.first);
        return {it, false};
      }
      InsertBalance(ptr);
      if (num == 1) {
        smallest = ptr;
        largest = ptr;
      } else if (ls(value.first, smallest->vt->first)) {
        smallest = ptr;
      } else if (ls(largest->vt->first, value.first)) {
        largest = ptr;
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
      node *target = ReplaceAndLocate(*pos.ptr->vt);
      if (num == 1) {
        smallest = nullptr;
        largest = nullptr;
      } else if (target == smallest) {
        smallest = (++begin()).ptr;
      } else if (target == largest) {
        largest = (--(--end())).ptr;
      }
      RemoveAndBalance(target);
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

    // todo: for debugging
    /*void PrintEdge(node *p) {
      if (p == nullptr) {
        return;
      }

      if (p->left) {
        if (p->red) {
          std::cout << "\033[31m" << p->vt->first << " ";
        } else {
          std::cout << "\033[0m" << p->vt->first << " ";
        }
        if (p->left->red) {
          std::cout << "\033[31m" << p->left->vt->first << "\n";
        } else {
          std::cout << "\033[0m" << p->left->vt->first << "\n";
        }
      }
      if (p->right) {
        if (p->red) {
          std::cout << "\033[31m" << p->vt->first << " ";
        } else {
          std::cout << "\033[0m" << p->vt->first << " ";
        }
        if (p->right->red) {
          std::cout << "\033[31m" << p->right->vt->first << "\n";
        } else {
          std::cout << "\033[0m" << p->right->vt->first << "\n";
        }
      }
      PrintEdge(p->left);
      PrintEdge(p->right);
    }

    void PrintKey(node *p) {
      if (p == nullptr) {
        return;
      }

      if (p->red) {
        std::cout << "\033[31m" << p->vt->first << "\n";
      } else {
        std::cout << "\033[0m" << p->vt->first << "\n";
      }

      PrintKey(p->left);
      PrintKey(p->right);
    }*/
  };
}

#endif
