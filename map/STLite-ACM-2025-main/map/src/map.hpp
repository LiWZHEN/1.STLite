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
      // Key *key = nullptr;
      // T *value = nullptr;
      value_type *vt = nullptr;

      node *left = nullptr;
      node *right = nullptr;
      node *parent = nullptr;
      int height = 0;
    };

    Compare ls;
    node *root = nullptr;
    node *the_end = nullptr;
    size_t num = 0;

    class key_already_exist final : public std::exception {
      /*--------------------------*/
    };

    /**
     * see BidirectionalIterator at CppReference for help.
     *
     * if there is anything wrong throw invalid_iterator.
     *     like it = map.begin(); --it;
     *       or it = map.end(); ++end();
     */
    class const_iterator;

    class iterator {
    private:
      /**
       * TODO add data members
       *   just add whatever you want.
       */

    public:
      node *ptr = nullptr;
      node *map_root = nullptr;
      map *mp = nullptr;
      bool at_end = false;
      iterator() {
        // TODO
        ptr = nullptr;
        map_root = nullptr;
        mp = nullptr;
        at_end = false;
      }

      iterator(const iterator &other) {
        // TODO
        ptr = other.ptr;
        map_root = other.map_root;
        mp = other.mp;
        at_end = other.at_end;
      }

      iterator(node *n, node *r, map *m, bool e) {
        ptr = n;
        map_root = r;
        mp = m;
        at_end = e;
      }

      /**
       * TODO iter++
       */
      iterator operator++(int) {
        if (at_end) {
          throw invalid_iterator();
        }
        iterator it(*this);
        if (it.ptr == nullptr) {
          throw invalid_iterator();
        }
        if (it.ptr->right) {
          it.ptr = it.ptr->right;
          while(it.ptr->left) {
            it.ptr = it.ptr->left;
          }
          return it;
        }
        // the largest in this branch
        while (it.ptr->parent && it.ptr == it.ptr->parent->right) {
          it.ptr = it.ptr->parent;
        }
        if (it.ptr->parent == nullptr) {
          throw invalid_iterator();
        }
        it.ptr = it.ptr->parent;
        return it;
      }

      /**
       * TODO ++iter
       */
      iterator &operator++() {
        if (at_end) {
          throw invalid_iterator();
        }
        if (ptr == nullptr) {
          throw invalid_iterator();
        }
        if (ptr->right) {
          ptr = ptr->right;
          while(ptr->left) {
            ptr = ptr->left;
          }
          return *this;
        }
        // the largest in this branch
        while (ptr->parent && ptr == ptr->parent->right) {
          ptr = ptr->parent;
        }
        if (ptr->parent == nullptr) {
          throw invalid_iterator();
        }
        ptr = ptr->parent;
        return *this;
      }

      /**
       * TODO iter--
       */
      iterator operator--(int) {
        if (at_end) {
          if (map_root == nullptr) {
            throw invalid_iterator();
          }
          node *new_ptr = map_root;
          while (new_ptr->right) {
            new_ptr = new_ptr->right;
          }
          return iterator(new_ptr, map_root, mp, false);
        }
        iterator it(*this);
        if (it.ptr == nullptr) {
          throw invalid_iterator();
        }
        if (it.ptr->left) {
          it.ptr = it.ptr->left;
          while(it.ptr->right) {
            it.ptr = it.ptr->right;
          }
          return it;
        }
        // the smallest in this branch
        while (it.ptr->parent && it.ptr == it.ptr->parent->left) {
          it.ptr = it.ptr->parent;
        }
        if (it.ptr->parent == nullptr) {
          throw invalid_iterator();
        }
        it.ptr = it.ptr->parent;
        return it;
      }

      /**
       * TODO --iter
       */
      iterator &operator--() {
        if (at_end) {
          if (map_root == nullptr) {
            throw invalid_iterator();
          }
          ptr = map_root;
          while (ptr->right) {
            ptr = ptr->right;
          }
          at_end = false;
          return *this;
        }
        if (ptr == nullptr) {
          throw invalid_iterator();
        }
        if (ptr->left) {
          ptr = ptr->left;
          while(ptr->right) {
            ptr = ptr->right;
          }
          return *this;
        }
        // the smallest in this branch
        while (ptr->parent && ptr == ptr->parent->left) {
          ptr = ptr->parent;
        }
        if (ptr->parent == nullptr) {
          throw invalid_iterator();
        }
        ptr = ptr->parent;
        return *this;
      }

      value_type &operator*() const {
        if (ptr == nullptr || map_root == nullptr) {
          throw invalid_iterator();
        }
        const Key k = ptr->vt->first;
        node *p = map_root;
        while (true) {
          if (ls(p->vt->first, k)) { // current key is smaller, search in the right branch
            p = p->right;
            if (p == nullptr) {
              throw invalid_iterator();
            }
          } else if (ls(k, p->vt->first)) { // current key is larger, search in the left branch
            p = p->left;
            if (p == nullptr) {
              throw invalid_iterator();
            }
          } else { // target founded
            return p->vt->second;
          }
        }
      }

      /**
       * an operator to check whether two iterators are same (pointing to the same memory).
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

      /**
       * for the support of it->first.
       * See <http://kelvinh.github.io/blog/2013/11/20/overloading-of-member-access-operator-dash-greater-than-symbol-in-cpp/> for help.
       */
      value_type *operator->() const noexcept {
        if (ptr == nullptr) {
          return nullptr;
        }
        return ptr->vt;
      }
    };

    class const_iterator {
      // it should have similar member method as iterator.
      //  and it should be able to construct from an iterator.
    private:
      // data members.
      const node *ptr = nullptr;
      const node *map_root = nullptr;
      const map *mp;
      bool at_end = false;
    public:
      const_iterator() {
        // TODO
        ptr = nullptr;
        map_root = nullptr;
        mp = nullptr;
        at_end = false;
      }

      const_iterator(const const_iterator &other) {
        // TODO
        ptr = other.ptr;
        map_root = other.map_root;
        mp = other.mp;
        at_end = other.at_end;
      }

      const_iterator(const iterator &other) {
        // TODO
        ptr = other.ptr;
        map_root = other.map_root;
        mp = other.mp;
        at_end = other.at_end;
      }

      const_iterator(const node *n, const node *r, const map *m, bool e) {
        ptr = n;
        map_root = r;
        mp = m;
        at_end = e;
      }

      /**
       * TODO iter++
       */
      const_iterator operator++(int) {
        if (at_end) {
          throw invalid_iterator();
        }
        const_iterator it(*this);
        if (it.ptr == nullptr) {
          throw invalid_iterator();
        }
        if (it.ptr->right) {
          it.ptr = it.ptr->right;
          while(it.ptr->left) {
            it.ptr = it.ptr->left;
          }
          return it;
        }
        // the largest in this branch
        while (it.ptr->parent && it.ptr == it.ptr->parent->right) {
          it.ptr = it.ptr->parent;
        }
        if (it.ptr->parent == nullptr) {
          throw invalid_iterator();
        }
        it.ptr = it.ptr->parent;
        return it;
      }

      /**
       * TODO ++iter
       */
      const_iterator &operator++() {
        if (at_end) {
          throw invalid_iterator();
        }
        if (ptr == nullptr) {
          throw invalid_iterator();
        }
        if (ptr->right) {
          ptr = ptr->right;
          while(ptr->left) {
            ptr = ptr->left;
          }
          return *this;
        }
        // the largest in this branch
        while (ptr->parent && ptr == ptr->parent->right) {
          ptr = ptr->parent;
        }
        if (ptr->parent == nullptr) {
          throw invalid_iterator();
        }
        ptr = ptr->parent;
        return *this;
      }

      /**
       * TODO iter--
       */
      const_iterator operator--(int) {
        if (at_end) {
          if (map_root == nullptr) {
            throw invalid_iterator();
          }
          node *new_ptr = map_root;
          while (new_ptr->right) {
            new_ptr = new_ptr->right;
          }
          return const_iterator(new_ptr, map_root, mp, false);
        }
        const_iterator it(*this);
        if (it.ptr == nullptr) {
          throw invalid_iterator();
        }
        if (it.ptr->left) {
          it.ptr = it.ptr->left;
          while(it.ptr->right) {
            it.ptr = it.ptr->right;
          }
          return it;
        }
        // the smallest in this branch
        while (it.ptr->parent && it.ptr == it.ptr->parent->left) {
          it.ptr = it.ptr->parent;
        }
        if (it.ptr->parent == nullptr) {
          throw invalid_iterator();
        }
        it.ptr = it.ptr->parent;
        return it;
      }

      /**
       * TODO --iter
       */
      const_iterator &operator--() {
        if (at_end) {
          if (map_root == nullptr) {
            throw invalid_iterator();
          }
          ptr = map_root;
          while (ptr->right) {
            ptr = ptr->right;
          }
          at_end = false;
          return *this;
        }
        if (ptr == nullptr) {
          throw invalid_iterator();
        }
        if (ptr->left) {
          ptr = ptr->left;
          while(ptr->right) {
            ptr = ptr->right;
          }
          return *this;
        }
        // the smallest in this branch
        while (ptr->parent && ptr == ptr->parent->left) {
          ptr = ptr->parent;
        }
        if (ptr->parent == nullptr) {
          throw invalid_iterator();
        }
        ptr = ptr->parent;
        return *this;
      }

      const value_type &operator*() const {
        if (ptr == nullptr || map_root == nullptr) {
          throw invalid_iterator();
        }
        const Key k = ptr->vt->first;
        node *p = map_root;
        while (true) {
          if (ls(p->vt->first, k)) { // current key is smaller, search in the right branch
            p = p->right;
            if (p == nullptr) {
              throw invalid_iterator();
            }
          } else if (ls(k, p->vt->first)) { // current key is larger, search in the left branch
            p = p->left;
            if (p == nullptr) {
              throw invalid_iterator();
            }
          } else { // target founded
            return p->vt->second;
          }
        }
      }

      /**
       * an operator to check whether two iterators are same (pointing to the same memory).
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

      /**
       * for the support of it->first.
       * See <http://kelvinh.github.io/blog/2013/11/20/overloading-of-member-access-operator-dash-greater-than-symbol-in-cpp/> for help.
       */
      const value_type *operator->() const noexcept {
        if (ptr == nullptr) {
          return nullptr;
        }
        return ptr->vt;
      }
    };

    void copy(node *&target, const node *source, node *parent) {
      target->parent = parent;
      target->vt = new value_type({source->vt->first, source->vt->second});
      target->height = source->height;
      if (source->left) {
        target->left = new node;
        copy(target->left, source->left, target);
      }
      if (source->right) {
        target->right = new node;
        copy(target->right, source->right, target);
      }
    }

    void clear_branch(node *&target) {
      if (target->left) {
        clear_branch(target->left);
      }
      if (target->right) {
        clear_branch(target->right);
      }
      delete target->vt;
      target->vt = nullptr;

      if (target->parent) { // update the height of parent node
        if (target->parent->left == target) { // the left child of parent node
          if (target->parent->right == nullptr) {
            target->parent->height = 1;
          } else {
            target->parent->height = target->parent->right->height + 1;
          }
        } else { // the right child of parent node
          if (target->parent->left == nullptr) {
            target->parent->height = 1;
          } else {
            target->parent->height = target->parent->left->height + 1;
          }
        }
      }
      delete target;
      target = nullptr;
    }


    /**
     * TODO two constructors
     */
    map() {
      the_end = new node;
    }
    map(const map &other) {
      if (other.root == nullptr) {
        return;
      }
      root = new node;
      copy(root, other.root, nullptr);
      num = other.num;
      the_end = new node;
    }

    /**
     * TODO assignment operator
     */
    map &operator=(const map &other) {
      if (this == &other) {
        return *this;
      }
      clear_branch(root);
      copy(root, other.root, nullptr);
      num = other.num;
      return *this;
    }

    /**
     * TODO Destructors
     */
    ~map() {
      clear_branch(root);
      delete the_end;
      num = 0;
    }

    /**
     * TODO
     * access specified element with bounds checking
     * Returns a reference to the mapped value of the element with key equivalent to key.
     * If no such element exists, an exception of type `index_out_of_bound'
     */
    T &at(const Key &key) {
      if (root == nullptr) {
        throw index_out_of_bound();
      }
      node *ptr = root;
      while (true) {
        if (ls(ptr->vt->first, key)) { // current key is smaller, search in the right branch
          ptr = ptr->right;
          if (ptr == nullptr) {
            throw index_out_of_bound();
          }
        } else if (ls(key, ptr->vt->first)) { // current key is larger, search in the left branch
          ptr = ptr->left;
          if (ptr == nullptr) {
            throw index_out_of_bound();
          }
        } else { // target founded
          return ptr->vt->second;
        }
      }
    }

    const T &at(const Key &key) const {
      if (root == nullptr) {
        throw index_out_of_bound();
      }
      node *ptr = root;
      while (true) {
        if (ls(ptr->vt->first, key)) { // current key is smaller, search in the right branch
          ptr = ptr->right;
          if (ptr == nullptr) {
            throw index_out_of_bound();
          }
        } else if (ls(key, ptr->vt->first)) { // current key is larger, search in the left branch
          ptr = ptr->left;
          if (ptr == nullptr) {
            throw index_out_of_bound();
          }
        } else { // target founded
          return ptr->vt->second;
        }
      }
    }

    void add_height(node *ptr) {
      while (ptr->parent) {
        if (ptr == ptr->parent->left) { // the left child of its parent
          if (ptr->parent->right == nullptr || ptr->height > ptr->parent->right->height) { // height need update
            ptr->parent->height = ptr->height + 1;
            ptr = ptr->parent;
          } else { // insert do not affect height
            return;
          }
        } else { // the right child of its parent
          if (ptr->parent->left == nullptr || ptr->height > ptr->parent->left->height) { // height need update
            ptr->parent->height = ptr->height + 1;
            ptr = ptr->parent;
          } else { // insert do not affect height
            return;
          }
        }
      }
    }

    void cut_height(node *ptr) {
      while (ptr->parent) {
        if (ptr == ptr->parent->left) { // the left child of its parent
          if (ptr->parent->right == nullptr || ptr->height >= ptr->parent->right->height) { // height need update
            ptr->parent->height = ptr->height + 1;
            ptr = ptr->parent;
          } else { // insert do not affect height
            return;
          }
        } else { // the right child of its parent
          if (ptr->parent->left == nullptr || ptr->height >= ptr->parent->left->height) { // height need update
            ptr->parent->height = ptr->height + 1;
            ptr = ptr->parent;
          } else { // insert do not affect height
            return;
          }
        }
      }
    }

    node *only_insert(const Key &key, const T &value) { // return a pointer if successfully inserted, else throw
      if (root == nullptr) {
        root = new node;
        root->height = 1;
        root->vt = new value_type({key, value});
        // root->key = new Key(key);
        // root->value = new T(value);
        return root;
      }
      node *ptr = root;
      while (true) {
        if (ls(ptr->vt->first, key)) { // current key is smaller, search in the right branch
          if (ptr->right == nullptr) {
            ptr->right = new node;
            ptr->right->parent = ptr;
            ptr = ptr->right;
            ptr->height = 1;
            ptr->vt = new value_type({key, value});
            // ptr->key = new Key(key);
            // ptr->value = new T(value);
            add_height(ptr);
            return ptr;
          }
          ptr = ptr->right;
        } else if (ls(key, ptr->vt->first)) { // current key is larger, search in the left branch
          if (ptr->left == nullptr) {
            ptr->left = new node;
            ptr->left->parent = ptr;
            ptr = ptr->left;
            ptr->height = 1;
            ptr->vt = new value_type({key, value});
            // ptr->key = new Key(key);
            // ptr->value = new T(value);
            add_height(ptr);
            return ptr;
          }
          ptr = ptr->left;
        } else { // target founded
          throw key_already_exist();
        }
      }
    }

    void maintain_balance(node *ptr, bool after_insert = true) {
      if (after_insert) {
        while (ptr->parent) {
          ptr = ptr->parent;

          if (ptr->left == nullptr) {
            if (ptr->right == nullptr || ptr->right->height < 2) {
              continue;
            }

            // RX
            node *new_root, *origin_parent = ptr->parent;

            if (ptr->right->left == nullptr) { // RR
              new_root = ptr->right;
              new_root->parent = origin_parent;
              new_root->left = ptr;
              ptr->parent = ptr->right;
              ptr->right = nullptr;
              ptr->height = 1;
            } else { // RL (ptr->right->right == nullptr)
              new_root = ptr->right->left;
              new_root->right = ptr->right;
              new_root->left = ptr;
              new_root->parent = origin_parent;
              ptr->parent = new_root;
              ptr->right = nullptr;
              new_root->right->parent = new_root;
              new_root->right->left = nullptr;
              new_root->height = 2;
              ptr->height = 1;
              new_root->right->height = 1;
            }

            if (origin_parent == nullptr) { // change root pointer
              root = new_root;
            } else if (ptr == origin_parent->left) { // change origin_parent->left
              origin_parent->left = new_root;
              cut_height(new_root);
            } else { // change origin_parent->right
              origin_parent->right = new_root;
              cut_height(new_root);
            }
            return;
          } else if (ptr->right == nullptr) {
            if (ptr->left->height < 2) {
              continue;
            }

            // LX
            node *new_root, *origin_parent = ptr->parent;

            if (ptr->left->right == nullptr) { // LL
              new_root = ptr->left;
              new_root->parent = origin_parent;
              new_root->right = ptr;
              ptr->left = nullptr;
              ptr->parent = new_root;
              ptr->height = 1;
            } else { // LR (ptr->left->left == nullptr)
              new_root = ptr->left->right;
              new_root->left = new_root->parent;
              new_root->right = ptr;
              new_root->parent = origin_parent;
              new_root->left->left = nullptr;
              new_root->left->parent = new_root;
              ptr->left = nullptr;
              ptr->parent = new_root;
              new_root->height = 2;
              new_root->left->height = 1;
              ptr->height = 1;
            }

            if (origin_parent == nullptr) { // change root pointer
              root = new_root;
            } else if (ptr == origin_parent->left) { // change origin_parent->left
              origin_parent->left = new_root;
              cut_height(new_root);
            } else { // change origin_parent->right
              origin_parent->right = new_root;
              cut_height(new_root);
            }
            return;
          } else { // left branch and right branch all exist
            node *new_root, *origin_parent = ptr->parent;
            if (ptr->left->height - ptr->right->height > 1) { // LX
              if (ptr->left->left->height > ptr->left->right->height) { // LL
                new_root = ptr->left;
                node *origin_r_child = ptr->left->right;
                new_root->parent = origin_parent;
                new_root->right = ptr;
                origin_r_child->parent = ptr;
                ptr->left = origin_r_child;
                ptr->parent = new_root;
              } else { // LR
                new_root = ptr->left->right;
                if (new_root->left) {
                  node *origin_l = ptr->left, *origin_lrl = new_root->left;
                  ptr->left = nullptr;
                  ptr->parent = new_root;
                  origin_l->parent = new_root;
                  origin_l->right = origin_lrl;
                  new_root->parent = origin_parent;
                  new_root->left = origin_l;
                  new_root->right = ptr;
                  origin_lrl->parent = origin_l;
                  ptr->height = ptr->right->height + 1;
                  origin_l->height = std::max(origin_l->left->height, origin_lrl->height) + 1;
                  new_root->height = std::max(ptr->height, origin_l->height) + 1;
                } else {
                  node *origin_l = ptr->left, *origin_lrr = new_root->right;
                  ptr->parent = new_root;
                  ptr->left = origin_lrr;
                  origin_l->parent = new_root;
                  origin_l->right = nullptr;
                  new_root->parent = origin_parent;
                  new_root->left = origin_l;
                  new_root->right = ptr;
                  origin_lrr->parent = ptr;
                  origin_l->height = origin_l->left->height + 1;
                  ptr->height = std::max(origin_lrr->height, ptr->right->height) + 1;
                  new_root->height = std::max(origin_l->height, ptr->height) + 1;
                }
              }
            } else if (ptr->left->height - ptr->right->height < -1) { // RX
              if (ptr->right->left->height < ptr->right->right->height) { // RR
                new_root = ptr->right;
                node *origin_rl = new_root->left;
                ptr->parent = new_root;
                ptr->right = origin_rl;
                new_root->parent = origin_parent;
                new_root->left = ptr;
                origin_rl->parent = ptr;
                ptr->height = std::max(ptr->left->height, ptr->right->height) + 1;
                new_root->right->height = new_root->right->right->height + 1;
                new_root->height = std::max(ptr->height, new_root->right->height) + 1;
              } else { // RL
                new_root = ptr->right->left;
                if (new_root->left) {
                  node *origin_r = ptr->right, *origin_rll = new_root->left;
                  ptr->parent = new_root;
                  ptr->right = origin_rll;
                  origin_r->parent = new_root;
                  origin_r->left = nullptr;
                  new_root->parent = origin_parent;
                  new_root->left = ptr;
                  new_root->right = origin_r;
                  origin_rll->parent = ptr;
                  ptr->height = std::max(ptr->left->height, origin_rll->height) + 1;
                  origin_r->height = origin_r->right->height + 1;
                  new_root->height = std::max(ptr->height, origin_r->height) + 1;
                } else {
                  node *origin_r = ptr->right, *origin_rlr = new_root->right;
                  ptr->parent = new_root;
                  ptr->right = nullptr;
                  origin_r->parent = new_root;
                  origin_r->left = origin_rlr;
                  new_root->parent = origin_parent;
                  new_root->left = ptr;
                  new_root->right = origin_r;
                  ptr->height = ptr->left->height + 1;
                  origin_r->height = std::max(origin_rlr->height, origin_r->right->height) + 1;
                  new_root->height = std::max(ptr->height, origin_r->height) + 1;
                }
              }
            } else {
              continue;
            }
            if (origin_parent == nullptr) { // change root pointer
              root = new_root;
            } else if (ptr == origin_parent->left) { // change origin_parent->left
              origin_parent->left = new_root;
              cut_height(new_root);
            } else { // change origin_parent->right
              origin_parent->right = new_root;
              cut_height(new_root);
            }
            return;
          }
        }
      } else { // maintain balance after delete
        while (true) {
          // ptr = ptr->parent;
          if (ptr->left == nullptr) {
            if (ptr->right == nullptr || ptr->right->height < 2) {
              if (ptr->parent == nullptr) {
                break;
              }
              ptr = ptr->parent;
              continue;
            }
            // RX
            node *new_root, *origin_parent = ptr->parent;
            if (ptr->right->right == nullptr || (ptr->right->left && ptr->right->left->height > ptr->right->right->height)) { // RL
              new_root = ptr->right->left;
              node *origin_r = ptr->right, *origin_rll = new_root->left, *origin_rlr = new_root->right;
              ptr->parent = new_root;
              ptr->right = origin_rll;
              origin_r->parent = new_root;
              origin_r->left = origin_rlr;
              new_root->parent = origin_parent;
              new_root->left = ptr;
              new_root->right = origin_r;
              if (origin_rll) {
                origin_rll->parent = ptr;
                ptr->height = origin_rll->height + 1;
              } else {
                ptr->height = 1;
              }
              if (origin_rlr) {
                origin_rlr->parent = origin_r;
                if (origin_r->right) {
                  origin_r->height = std::max(origin_r->right->height, origin_rlr->height) + 1;
                } else {
                  origin_r->height = origin_rlr->height + 1;
                }
              } else {
                if (origin_r->right) {
                  origin_r->height = origin_r->right->height + 1;
                } else {
                  origin_r->height = 1;
                }
              }
              new_root->height = std::max(ptr->height, origin_r->height) + 1;
            } else { // RR
              new_root = ptr->right;
              node *origin_rl = new_root->left;
              ptr->parent = new_root;
              ptr->right = origin_rl;
              new_root->parent = origin_parent;
              new_root->left = ptr;
              if (origin_rl) {
                origin_rl->parent = ptr;
                ptr->height = origin_rl->height + 1;
              } else {
                ptr->height = 1;
              }
              new_root->height = std::max(ptr->height, new_root->right->height) + 1;
            }
            if (origin_parent == nullptr) { // change root pointer
              root = new_root;
            } else if (ptr == origin_parent->left) { // change origin_parent->left
              origin_parent->left = new_root;
              cut_height(new_root);
            } else { // change origin_parent->right
              origin_parent->right = new_root;
              cut_height(new_root);
            }
          } else if (ptr->right == nullptr) {
            if (ptr->left->height < 2) {
              if (ptr->parent == nullptr) {
                break;
              }
              ptr = ptr->parent;
              continue;
            }
            // LX
            node *new_root, *origin_parent = ptr->parent;
            if (ptr->left->left == nullptr || (ptr->left->right && ptr->left->left->height < ptr->left->right->height)  ) { // LR
              new_root = ptr->left->right;
              node *origin_l = ptr->left, *origin_lrl = new_root->left, *origin_lrr = new_root->right;
              ptr->parent = new_root;
              ptr->left = origin_lrr;
              origin_l->parent = new_root;
              origin_l->right = origin_lrl;
              new_root->parent = origin_parent;
              new_root->left = origin_l;
              new_root->right = ptr;
              if (origin_lrl) {
                origin_lrl->parent = origin_l;
                if (origin_l->left) {
                  origin_l->height = std::max(origin_l->left->height, origin_lrl->height) + 1;
                } else {
                  origin_l->height = origin_lrl->height + 1;
                }
              } else {
                if (origin_l->left) {
                  origin_l->height = origin_l->left->height + 1;
                } else {
                  origin_l->height = 1;
                }
              }
              if (origin_lrr) {
                origin_lrr->parent = ptr;
                ptr->height = origin_lrr->height + 1;
              } else {
                ptr->height = 1;
              }
              new_root->height = std::max(origin_l->height, ptr->height) + 1;
            } else { // LL
              new_root = ptr->left;
              node *origin_lr = new_root->right;
              ptr->parent = new_root;
              ptr->left = origin_lr;
              new_root->parent = origin_parent;
              new_root->right = ptr;
              if (origin_lr) {
                origin_lr->parent = ptr;
                ptr->height = origin_lr->height + 1;
              } else {
                ptr->height = 1;
              }
              new_root->height = std::max(ptr->height, new_root->left->height) + 1;
            }
            if (origin_parent == nullptr) { // change root pointer
              root = new_root;
            } else if (ptr == origin_parent->left) { // change origin_parent->left
              origin_parent->left = new_root;
              cut_height(new_root);
            } else { // change origin_parent->right
              origin_parent->right = new_root;
              cut_height(new_root);
            }
          } else { // left branch and right branch all exist
            node *new_root, *origin_parent = ptr->parent;
            if (ptr->left->height - ptr->right->height > 1) { // LX
              if (ptr->left->left->height < ptr->left->right->height) { // LR
                new_root = ptr->left->right;
                if (new_root->left) {
                  node *origin_l = ptr->left, *origin_lrl = new_root->left;
                  ptr->left = nullptr;
                  ptr->parent = new_root;
                  origin_l->parent = new_root;
                  origin_l->right = origin_lrl;
                  new_root->parent = origin_parent;
                  new_root->left = origin_l;
                  new_root->right = ptr;
                  origin_lrl->parent = origin_l;
                  ptr->height = ptr->right->height + 1;
                  origin_l->height = std::max(origin_l->left->height, origin_lrl->height) + 1;
                  new_root->height = std::max(ptr->height, origin_l->height) + 1;
                } else {
                  node *origin_l = ptr->left, *origin_lrr = new_root->right;
                  ptr->parent = new_root;
                  ptr->left = origin_lrr;
                  origin_l->parent = new_root;
                  origin_l->right = nullptr;
                  new_root->parent = origin_parent;
                  new_root->left = origin_l;
                  new_root->right = ptr;
                  origin_lrr->parent = ptr;
                  origin_l->height = origin_l->left->height + 1;
                  ptr->height = std::max(origin_lrr->height, ptr->right->height) + 1;
                  new_root->height = std::max(origin_l->height, ptr->height) + 1;
                }
              } else { // LL
                new_root = ptr->left;
                node *origin_r_child = ptr->left->right;
                new_root->parent = origin_parent;
                new_root->right = ptr;
                origin_r_child->parent = ptr;
                ptr->left = origin_r_child;
                ptr->parent = new_root;
              }
            } else if (ptr->left->height - ptr->right->height < -1) { // RX
              if (ptr->right->left->height > ptr->right->right->height) { // RL
                new_root = ptr->right->left;
                if (new_root->left) {
                  node *origin_r = ptr->right, *origin_rll = new_root->left;
                  ptr->parent = new_root;
                  ptr->right = origin_rll;
                  origin_r->parent = new_root;
                  origin_r->left = nullptr;
                  new_root->parent = origin_parent;
                  new_root->left = ptr;
                  new_root->right = origin_r;
                  origin_rll->parent = ptr;
                  ptr->height = std::max(ptr->left->height, origin_rll->height) + 1;
                  origin_r->height = origin_r->right->height + 1;
                  new_root->height = std::max(ptr->height, origin_r->height) + 1;
                } else {
                  node *origin_r = ptr->right, *origin_rlr = new_root->right;
                  ptr->parent = new_root;
                  ptr->right = nullptr;
                  origin_r->parent = new_root;
                  origin_r->left = origin_rlr;
                  new_root->parent = origin_parent;
                  new_root->left = ptr;
                  new_root->right = origin_r;
                  ptr->height = ptr->left->height + 1;
                  origin_r->height = std::max(origin_rlr->height, origin_r->right->height) + 1;
                  new_root->height = std::max(ptr->height, origin_r->height) + 1;
                }
              } else { // RR
                new_root = ptr->right;
                node *origin_rl = new_root->left;
                ptr->parent = new_root;
                ptr->right = origin_rl;
                new_root->parent = origin_parent;
                new_root->left = ptr;
                origin_rl->parent = ptr;
                ptr->height = std::max(ptr->left->height, ptr->right->height) + 1;
                new_root->right->height = new_root->right->right->height + 1;
                new_root->height = std::max(ptr->height, new_root->right->height) + 1;
              }
            } else {
              if (ptr->parent == nullptr) {
                break;
              }
              ptr = ptr->parent;
              continue;
            }
            if (origin_parent == nullptr) { // change root pointer
              root = new_root;
            } else if (ptr == origin_parent->left) { // change origin_parent->left
              origin_parent->left = new_root;
              cut_height(new_root);
            } else { // change origin_parent->right
              origin_parent->right = new_root;
              cut_height(new_root);
            }
          }
          if (ptr->parent == nullptr) {
            break;
          }
          ptr = ptr->parent;
        }
      }
    }

    /**
     * TODO
     * access specified element
     * Returns a reference to the value that is mapped to a key equivalent to key,
     *   performing an insertion if such key does not already exist.
     */
    T &operator[](const Key &key) {

      node *ptr = root;

      try {
        if (root == nullptr) {
          throw index_out_of_bound();
        }
        while (true) {
          if (ls(ptr->vt->first, key)) { // current key is smaller, search in the right branch
            ptr = ptr->right;
            if (ptr == nullptr) {
              throw index_out_of_bound();
            }
          } else if (ls(key, ptr->vt->first)) { // current key is larger, search in the left branch
            ptr = ptr->left;
            if (ptr == nullptr) {
              throw index_out_of_bound();
            }
          } else { // target founded
            return ptr->vt->second;
          }
        }
      } catch (index_out_of_bound) { // not exist yet, need to insert a node
        T default_value;
        node *inserted = only_insert(key, default_value);
        return inserted->vt->second;
      }
    }

    /**
     * behave like at() throw index_out_of_bound if such key does not exist.
     */
    const T &operator[](const Key &key) const {
      if (root == nullptr) {
        throw index_out_of_bound();
      }
      node *ptr = root;
      while (true) {
        if (ls(ptr->vt->first, key)) { // current key is smaller, search in the right branch
          ptr = ptr->right;
          if (ptr == nullptr) {
            throw index_out_of_bound();
          }
        } else if (ls(key, ptr->vt->first)) { // current key is larger, search in the left branch
          ptr = ptr->left;
          if (ptr == nullptr) {
            throw index_out_of_bound();
          }
        } else { // target founded
          return ptr->vt->second;
        }
      }
    }

    /**
     * return an iterator to the beginning
     */
    iterator begin() {
      if (root == nullptr) {
        return iterator(the_end, nullptr, this, true);
      }
      node *ptr = root;
      while (ptr->left) {
        ptr = ptr->left;
      }
      return iterator(ptr, root, this, false);
    }

    const_iterator cbegin() const {
      if (root == nullptr) {
        const node *p = the_end;
        return const_iterator(p, nullptr, this, true);
      }
      node *ptr = root;
      while (ptr->left) {
        ptr = ptr->left;
      }
      const node *r = root, *p = ptr;
      return const_iterator(p, r, this, false);
    }

    /**
     * return an iterator to the end
     * in fact, it returns past-the-end.
     */
    iterator end() {
      return iterator(the_end, root, this, true);
    }

    const_iterator cend() const {
      return const_iterator(the_end, root, this, true);
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
      clear_branch(root);
      num = 0;
    }

    /**
     * insert an element.
     * return a pair, the first of the pair is
     *   the iterator to the new element (or the element that prevented the insertion),
     *   the second one is true if insert successfully, or false.
     */
    pair<iterator, bool> insert(const value_type &value) {
      bool success = true;
      try {
        node *pos = only_insert(value.first, value.second);
        maintain_balance(pos, true);
        ++num;
      } catch (key_already_exist &) {
        success = false;
      }
      node *p = root;
      while (true) {
        if (ls(p->vt->first, value.first)) { // current key is smaller, search in the right branch
          p = p->right;
        } else if (ls(value.first, p->vt->first)) { // current key is larger, search in the left branch
          p = p->left;
        } else { // target founded
          return {iterator(p, root, this, false), success};
        }
      }
    }

    /**
     * erase the element at pos.
     *
     * throw if pos pointed to a bad element (pos == this->end() || pos points an element out of this)
     */
    void erase(iterator pos) {
      if (pos.at_end || pos.map_root == nullptr || pos.ptr == nullptr) {
        throw invalid_iterator();
      }
      try {
        node *p = pos.map_root;
        Key target = pos.ptr->vt->first;
        while (true) {
          if (ls(p->vt->first, target)) {
            p = p->right;
            if (p == nullptr) {
              throw index_out_of_bound();
            }
          } else if (ls(target, p->vt->first)) {
            p = p->left;
            if (p == nullptr) {
              throw index_out_of_bound();
            }
          } else {
            if (p != pos.ptr) {
              throw invalid_iterator();
            }
            break;
          }
        }
      } catch (...) {
        throw invalid_iterator();
      }

      // can delete
      if (pos.ptr->left == nullptr && pos.ptr->right == nullptr) { // the leaf node
        if (pos.ptr->parent == nullptr) { // the root
          root = nullptr;
          delete pos.ptr->vt;
          pos.ptr->vt = nullptr;
          delete pos.ptr;
          pos.ptr = nullptr;
        } else if (pos.ptr == pos.ptr->parent->left) { // parent's left child
          node *parent = pos.ptr->parent;
          delete pos.ptr->vt;
          pos.ptr->vt = nullptr;
          delete pos.ptr;
          pos.ptr = nullptr;
          parent->left = nullptr;
          if (parent->right == nullptr) {
            parent->height = 1;
            cut_height(parent);
            maintain_balance(parent, false);
          } else {
            parent->height = parent->right->height + 1;
            cut_height(parent);
            maintain_balance(parent, false);
          }
        } else { // parent's right child
          node *parent = pos.ptr->parent;
          delete pos.ptr->vt;
          pos.ptr->vt = nullptr;
          delete pos.ptr;
          pos.ptr = nullptr;
          parent->right = nullptr;
          if (parent->left == nullptr) {
            parent->height = 1;
            cut_height(parent);
            maintain_balance(parent, false);
          } else {
            parent->height = parent->left->height + 1;
            cut_height(parent);
            maintain_balance(parent, false);
          }
        }
        --num;
      } else if (pos.ptr->left == nullptr) { // only have right child
        node *parent = pos.ptr->parent, *r_child = pos.ptr->right;
        if (parent == nullptr) { // change root
          root = r_child;
          r_child->parent = nullptr;
          delete pos.ptr->vt;
          pos.ptr->vt = nullptr;
          delete pos.ptr;
          pos.ptr = nullptr;
        } else if (pos.ptr == parent->left) { // parent's left child
          parent->left = r_child;
          r_child->parent = parent;
          cut_height(r_child);
          delete pos.ptr->vt;
          pos.ptr->vt = nullptr;
          delete pos.ptr;
          pos.ptr = nullptr;
          maintain_balance(r_child, false);
        } else { // parent's right child
          parent->right = r_child;
          r_child->parent = parent;
          cut_height(r_child);
          delete pos.ptr->vt;
          pos.ptr->vt = nullptr;
          delete pos.ptr;
          pos.ptr = nullptr;
          maintain_balance(r_child, false);
        }
        --num;
      } else if (pos.ptr->right == nullptr) { // only have left child
        node *parent = pos.ptr->parent, *l_child = pos.ptr->left;
        if (parent == nullptr) { // change root
          root = l_child;
          l_child->parent = nullptr;
          delete pos.ptr->vt;
          pos.ptr->vt = nullptr;
          delete pos.ptr;
          pos.ptr = nullptr;
        } else if (pos.ptr == parent->left) { // parent's left child
          parent->left = l_child;
          l_child->parent = parent;
          cut_height(l_child);
          delete pos.ptr->vt;
          pos.ptr->vt = nullptr;
          delete pos.ptr;
          pos.ptr = nullptr;
          maintain_balance(l_child);
        } else { // parent's right child
          parent->right = l_child;
          l_child->parent = parent;
          cut_height(l_child);
          delete pos.ptr->vt;
          pos.ptr->vt = nullptr;
          delete pos.ptr;
          pos.ptr = nullptr;
          maintain_balance(l_child);
        }
        --num;
      } else { // have both right and left children
        node *target = pos.ptr->right;
        while (target->left) {
          target = target->left;
        }
        // change pos.ptr and target
        if (target == pos.ptr->right) { // directly connected
          bool up_l;
          node *up_parent = pos.ptr->parent, *up_left = pos.ptr->left;
          int up_h = pos.ptr->height;
          if (up_parent && pos.ptr == up_parent->left) {
            up_l = true;
          } else {
            up_l = false;
          }
          node *down_right = target->right;
          int down_h = target->height;

          pos.ptr->parent = target;
          target->right = pos.ptr;

          pos.ptr->left = nullptr;
          pos.ptr->right = down_right;
          pos.ptr->height = down_h;
          target->parent = up_parent;
          target->left = up_left;
          target->height = up_h;
          up_left->parent = target;
          if (up_parent) {
            if (up_l) {
              up_parent->left = target;
            } else {
              up_parent->right = target;
            }
          } else {
            root = target;
          }
          if (down_right) {
            down_right->parent = pos.ptr;
          }
        } else { // not directly connected
          bool up_l;
          // bool down_l;
          node *up_parent = pos.ptr->parent, *up_left = pos.ptr->left, *up_right = pos.ptr->right;
          int up_h = pos.ptr->height;
          if (up_parent && pos.ptr == up_parent->left) {
            up_l = true;
          } else {
            up_l = false;
          }
          node *down_parent = target->parent, *down_right = target->right;
          int down_h = target->height;
          /*if (down_parent && target == down_parent->left) {
            down_l = true;
          } else {
            down_l = false;
          }*/
          pos.ptr->parent = down_parent;
          pos.ptr->left = nullptr;
          pos.ptr->right = down_right;
          pos.ptr->height = down_h;
          target->parent = up_parent;
          target->left = up_left;
          target->right = up_right;
          target->height = up_h;
          if (up_parent) {
            if (up_l) {
              up_parent->left = target;
            } else {
              up_parent->right = target;
            }
          } else {
            root = target;
          }
          up_left->parent = target;
          up_right->parent = target;
          down_parent->left = pos.ptr;
          if (down_right) {
            down_right->parent = pos.ptr;
          }
        }

        // delete pos.ptr after changing
        iterator changed_pos(pos.ptr, root, this, false);
        erase(changed_pos);
      }
    }

    /**
     * Returns the number of elements with key
     *   that compares equivalent to the specified argument,
     *   which is either 1 or 0
     *     since this container does not allow duplicates.
     * The default method of check the equivalence is !(a < b || b > a)
     */
    size_t count(const Key &key) const {
      try {
        if (root == nullptr) {
          throw index_out_of_bound();
        }
        node *ptr = root;
        while (true) {
          if (ls(ptr->vt->first, key)) { // current key is smaller, search in the right branch
            ptr = ptr->right;
            if (ptr == nullptr) {
              throw index_out_of_bound();
            }
          } else if (ls(key, ptr->vt->first)) { // current key is larger, search in the left branch
            ptr = ptr->left;
            if (ptr == nullptr) {
              throw index_out_of_bound();
            }
          } else { // target founded
            return 1;
          }
        }
      } catch (index_out_of_bound) {
        return 0;
      }
    }

    /**
     * Finds an element with key equivalent to key.
     * key value of the element to search for.
     * Iterator to an element with key equivalent to key.
     *   If no such element is found, past-the-end (see end()) iterator is returned.
     */
    iterator find(const Key &key) {
      try {
        if (root == nullptr) {
          throw index_out_of_bound();
        }
        node *ptr = root;
        while (true) {
          if (ls(ptr->vt->first, key)) { // current key is smaller, search in the right branch
            ptr = ptr->right;
            if (ptr == nullptr) {
              throw index_out_of_bound();
            }
          } else if (ls(key, ptr->vt->first)) { // current key is larger, search in the left branch
            ptr = ptr->left;
            if (ptr == nullptr) {
              throw index_out_of_bound();
            }
          } else { // target founded
            return iterator(ptr, root, this, false);
          }
        }
      } catch (index_out_of_bound) {
        return iterator(the_end, root, this, true);
      }
    }

    const_iterator find(const Key &key) const {
      try {
        if (root == nullptr) {
          throw index_out_of_bound();
        }
        node *ptr = root;
        while (true) {
          if (ls(ptr->vt->first, key)) { // current key is smaller, search in the right branch
            ptr = ptr->right;
            if (ptr == nullptr) {
              throw index_out_of_bound();
            }
          } else if (ls(key, ptr->vt->first)) { // current key is larger, search in the left branch
            ptr = ptr->left;
            if (ptr == nullptr) {
              throw index_out_of_bound();
            }
          } else { // target founded
            return const_iterator(ptr, root, this, false);
          }
        }
      } catch (index_out_of_bound) {
        return const_iterator(the_end, root, this, true);
      }
    }

  };
}

#endif
