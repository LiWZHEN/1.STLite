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
      // node *map_root = nullptr;
      map *mp = nullptr;
      bool at_end = false;

      iterator() {
        // TODO
        ptr = nullptr;
        // map_root = nullptr;
        mp = nullptr;
        // at_begin = false;
        at_end = false;
      }

      iterator(const iterator &other) {
        // TODO
        ptr = other.ptr;
        // map_root = other.map_root;
        mp = other.mp;
        // at_begin = other.at_begin;
        at_end = other.at_end;
      }

      iterator(node *n, map *m, bool e) {
        ptr = n;
        // map_root = r;
        mp = m;
        // at_begin = b;
        at_end = e;
      }

      /**
       * TODO iter++
       */
      iterator operator++(int) {
        iterator it(*this);
        if (at_end) {
          throw invalid_iterator();
        }
        /*if (at_begin && ptr == mp->the_end) {
          if (mp->root == nullptr) {
            throw invalid_iterator();
          }
          ptr = mp->root;
          while (ptr->left) {
            ptr = ptr->left;
          }
          at_begin = false;
          if (ptr->right) {
            ptr = ptr->right;
            while (ptr->left) {
              ptr = ptr->left;
            }
            return it;
          }
          while (ptr->parent && ptr == ptr->parent->right) {
            ptr = ptr->parent;
          }
          if (ptr->parent == nullptr) {
            at_end = true;
            ptr = mp->the_end;
            return it;
          }
          ptr = ptr->parent;
          return it;
        }*/
        if (ptr == nullptr) {
          throw invalid_iterator();
        }
        if (ptr->right) {
          ptr = ptr->right;
          while(ptr->left) {
            ptr = ptr->left;
          }
          return it;
        }
        // the largest in this branch
        while (ptr->parent && ptr == ptr->parent->right) {
          ptr = ptr->parent;
        }
        if (ptr->parent == nullptr) {
          ptr = mp->the_end;
          at_end = true;
          return it;
        }
        ptr = ptr->parent;
        return it;
      }

      /**
       * TODO ++iter
       */
      iterator &operator++() {
        if (at_end) {
          throw invalid_iterator();
        }
        /*if (at_begin && ptr == mp->the_end) {
          if (mp->root == nullptr) {
            throw invalid_iterator();
          }
          ptr = mp->root;
          while (ptr->left) {
            ptr = ptr->left;
          }
          at_begin = false;
          if (ptr->right) {
            ptr = ptr->right;
            while (ptr->left) {
              ptr = ptr->left;
            }
            return *this;
          }
          while (ptr->parent && ptr == ptr->parent->right) {
            ptr = ptr->parent;
          }
          if (ptr->parent == nullptr) {
            at_end = true;
            ptr = mp->the_end;
            return *this;
          }
          ptr = ptr->parent;
          return *this;
        }*/
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
          ptr = mp->the_end;
          at_end = true;
          return *this;
        }
        ptr = ptr->parent;
        return *this;
      }

      /**
       * TODO iter--
       */
      iterator operator--(int) {
        iterator it(*this);
        if (at_end) {
          if (mp->root == nullptr) {
            throw invalid_iterator();
          }
          ptr = mp->root;
          while (ptr->right) {
            ptr = ptr->right;
          }
          at_end = false;
          return it;
        }
        if (ptr == nullptr) {
          throw invalid_iterator();
        }
        if (ptr->left) {
          ptr = ptr->left;
          while(ptr->right) {
            ptr = ptr->right;
          }
          return it;
        }
        // the smallest in this branch
        while (ptr->parent && ptr == ptr->parent->left) {
          ptr = ptr->parent;
        }
        if (ptr->parent == nullptr) {
          throw invalid_iterator();
        }
        ptr = ptr->parent;
        return it;
      }

      /**
       * TODO --iter
       */
      iterator &operator--() {
        if (at_end) {
          if (mp->root == nullptr) {
            throw invalid_iterator();
          }
          ptr = mp->root;
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
        if (ptr == nullptr || mp->root == nullptr) {
          throw invalid_iterator();
        }
        Key k = ptr->vt->first;
        node *p = mp->root;
        while (true) {
          if (mp->ls(p->vt->first, k)) { // current key is smaller, search in the right branch
            p = p->right;
            if (p == nullptr) {
              throw invalid_iterator();
            }
          } else if (mp->ls(k, p->vt->first)) { // current key is larger, search in the left branch
            p = p->left;
            if (p == nullptr) {
              throw invalid_iterator();
            }
          } else { // target founded
            return *(p->vt);
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
        /*node *p = ptr;
        if (at_begin && ptr == mp->the_end) {
          if (mp->root) {
            p = mp->root;
            while (p->left) {
              p = p->left;
            }
          }
        }*/
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
      // const node *map_root = nullptr;
      const map *mp;
      // bool at_begin = false;
      bool at_end = false;

      const_iterator() {
        // TODO
        ptr = nullptr;
        // map_root = nullptr;
        mp = nullptr;
        // at_begin = false;
        at_end = false;
      }

      const_iterator(const const_iterator &other) {
        // TODO
        ptr = other.ptr;
        // map_root = other.map_root;
        mp = other.mp;
        // at_begin = other.at_begin;
        at_end = other.at_end;
      }

      const_iterator(const iterator &other) {
        // TODO
        ptr = other.ptr;
        // map_root = other.map_root;
        mp = other.mp;
        // at_begin = other.at_begin;
        at_end = other.at_end;
      }

      const_iterator(const node *n, const map *m, bool e) {
        ptr = n;
        // map_root = r;
        mp = m;
        // at_begin = b;
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
        /*if (at_begin && ptr == the_end) {
          if (mp->root == nullptr) {
            throw invalid_iterator();
          }
          ptr = mp->root;
          while (ptr->left) {
            ptr = ptr->left;
          }
          at_begin = false;
          if (ptr->right) {
            ptr = ptr->right;
            while (ptr->left) {
              ptr = ptr->left;
            }
            return it;
          }
          while (ptr->parent && ptr == ptr->parent->right) {
            ptr = ptr->parent;
          }
          if (ptr->parent == nullptr) {
            at_end = true;
            ptr = mp->the_end;
            return it;
          }
          ptr = ptr->parent;
          return it;
        }*/
        if (ptr == nullptr) {
          throw invalid_iterator();
        }
        if (ptr->right) {
          ptr = ptr->right;
          while(ptr->left) {
            ptr = ptr->left;
          }
          return it;
        }
        // the largest in this branch
        while (ptr->parent && ptr == ptr->parent->right) {
          ptr = ptr->parent;
        }
        if (ptr->parent == nullptr) {
          ptr = mp->the_end;
          at_end = true;
        } else {
          ptr = ptr->parent;
        }
        return it;
      }

      /**
       * TODO ++iter
       */
      const_iterator &operator++() {
        if (at_end) {
          throw invalid_iterator();
        }
        /*if (at_begin && ptr == mp->the_end) {
          if (mp->root == nullptr) {
            throw invalid_iterator();
          }
          ptr = mp->root;
          while (ptr->left) {
            ptr = ptr->left;
          }
          at_begin = false;
          if (ptr->right) {
            ptr = ptr->right;
            while (ptr->left) {
              ptr = ptr->left;
            }
            return *this;
          }
          while (ptr->parent && ptr == ptr->parent->right) {
            ptr = ptr->parent;
          }
          if (ptr->parent == nullptr) {
            at_end = true;
            ptr = mp->the_end;
            return *this;
          }
          ptr = ptr->parent;
          return *this;
        }*/
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
          ptr = mp->the_end;
          at_end = true;
        } else {
          ptr = ptr->parent;
        }
        return *this;
      }

      /**
       * TODO iter--
       */
      const_iterator operator--(int) {
        const_iterator it(*this);
        if (at_end) {
          if (mp->root == nullptr) {
            throw invalid_iterator();
          }
          while (ptr->right) {
            ptr = ptr->right;
          }
          return it;
        }
        if (ptr == nullptr) {
          throw invalid_iterator();
        }
        if (ptr->left) {
          ptr = ptr->left;
          while(ptr->right) {
            ptr = ptr->right;
          }
          return it;
        }
        // the smallest in this branch
        while (ptr->parent && ptr == ptr->parent->left) {
          ptr = ptr->parent;
        }
        if (ptr->parent == nullptr) {
          throw invalid_iterator();
        }
        ptr = ptr->parent;
        return it;
      }

      /**
       * TODO --iter
       */
      const_iterator &operator--() {
        if (at_end) {
          if (mp->root == nullptr) {
            throw invalid_iterator();
          }
          ptr = mp->root;
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
        if (ptr == nullptr || mp->root == nullptr) {
          throw invalid_iterator();
        }
        const Key k = ptr->vt->first;
        const node *p = mp->root;
        while (true) {
          if (mp->ls(p->vt->first, k)) { // current key is smaller, search in the right branch
            p = p->right;
            if (p == nullptr) {
              throw invalid_iterator();
            }
          } else if (mp->ls(k, p->vt->first)) { // current key is larger, search in the left branch
            p = p->left;
            if (p == nullptr) {
              throw invalid_iterator();
            }
          } else { // target founded
            return *(p->vt);
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
        const node *p = ptr;
        /*if (at_begin && ptr == mp->the_end) {
          p = mp->root;
          if (p == nullptr) {
            return nullptr;
          }
          while (p->left) {
            p = p->left;
          }
        }*/
        if (p == nullptr) {
          return nullptr;
        }
        return p->vt;
      }
    };

    void copy(node *&target, const node *source, node *parent) {
      if (source == nullptr) {
        if (target) {
          if (parent) {
            if (parent->left == target) {
              parent->left = nullptr;
            } else {
              parent->right = nullptr;
            }
          } else {
            root = nullptr;
          }
          delete target->vt;
          target->vt = nullptr;
          delete target;
          target = nullptr;
        }
        return;
      }
      if (target == nullptr) {
        target = new node;
      }
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
      if (target == nullptr) {
        return;
      }
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

  private:
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
        while (true) {
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
            return;
          }
          if (ptr->right == nullptr) {
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
            return;
          }
          // left branch and right branch all exist
          node *new_root, *origin_parent = ptr->parent;
          if (ptr->left->height - ptr->right->height > 1) { // LX
            if (ptr->left->left == nullptr || (ptr->left->right && ptr->left->left->height < ptr->left->right->height)) { // LR
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
                ptr->height = std::max(origin_lrr->height, ptr->right->height) + 1;
              } else {
                ptr->height = ptr->right->height + 1;
              }
              new_root->height = std::max(origin_l->height, ptr->height) + 1;
            } else { // LL
              new_root = ptr->left;
              node *origin_lr = ptr->left->right;
              ptr->parent = new_root;
              ptr->left = origin_lr;
              new_root->parent = origin_parent;
              new_root->right = ptr;
              if (origin_lr) {
                origin_lr->parent = ptr;
                ptr->height = std::max(origin_lr->height, ptr->right->height) + 1;
              } else {
                ptr->height = ptr->right->height + 1;
              }
              new_root->height = std::max(new_root->left->height, ptr->height) + 1;
            }
          } else if (ptr->left->height - ptr->right->height < -1) { // RX
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
                ptr->height = std::max(ptr->left->height, origin_rll->height) + 1;
              } else {
                ptr->height = ptr->left->height + 1;
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
                ptr->height = std::max(ptr->left->height, origin_rl->height) + 1;
              } else {
                ptr->height = ptr->left->height + 1;
              }
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
          if (ptr->parent == nullptr) {
            break;
          }
          return;
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
              if (ptr->left->left == nullptr || (ptr->left->right && ptr->left->left->height < ptr->left->right->height)) { // LR
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
                  ptr->height = std::max(origin_lrr->height, ptr->right->height) + 1;
                } else {
                  ptr->height = ptr->right->height + 1;
                }
                new_root->height = std::max(origin_l->height, ptr->height) + 1;
              } else { // LL
                new_root = ptr->left;
                node *origin_lr = ptr->left->right;
                ptr->parent = new_root;
                ptr->left = origin_lr;
                new_root->parent = origin_parent;
                new_root->right = ptr;
                if (origin_lr) {
                  origin_lr->parent = ptr;
                  ptr->height = std::max(origin_lr->height, ptr->right->height) + 1;
                } else {
                  ptr->height = ptr->right->height + 1;
                }
                new_root->height = std::max(new_root->left->height, ptr->height) + 1;
              }
            } else if (ptr->left->height - ptr->right->height < -1) { // RX
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
                  ptr->height = std::max(ptr->left->height, origin_rll->height) + 1;
                } else {
                  ptr->height = ptr->left->height + 1;
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
                  ptr->height = std::max(ptr->left->height, origin_rl->height) + 1;
                } else {
                  ptr->height = ptr->left->height + 1;
                }
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

  public:
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
        ++num;
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
        return iterator(the_end, this, true);
      }
      node *ptr = root;
      while (ptr->left) {
        ptr = ptr->left;
      }
      return iterator(ptr, this, false);
    }

    const_iterator cbegin() const {
      if (root == nullptr) {
        const node *p = the_end;
        return const_iterator(p, this, true);
      }
      node *ptr = root;
      while (ptr->left) {
        ptr = ptr->left;
      }
      const node *p = ptr;
      return const_iterator(p, this, false);
    }

    /**
     * return an iterator to the end
     * in fact, it returns past-the-end.
     */
    iterator end() {
      return iterator(the_end, this, true);
    }

    const_iterator cend() const {
      return const_iterator(the_end, this, true);
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
          return {iterator(p, this, false), success};
        }
      }
    }

    /**
     * erase the element at pos.
     *
     * throw if pos pointed to a bad element (pos == this->end() || pos points an element out of this)
     */
    void erase(iterator pos, bool changed = false) {
      if (pos.mp != this) {
        throw invalid_iterator();
      }
      if (pos.at_end || pos.mp->root == nullptr || pos.ptr == nullptr) {
        throw invalid_iterator();
      }
      /*if (pos.at_begin) {
        pos.at_begin = false;
        pos.ptr = pos.mp->root;
        while (pos.ptr->left) {
          pos.ptr = pos.ptr->left;
        }
      }*/
      if (!changed) {
        try {
          node *p = pos.mp->root;
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
        iterator changed_pos(pos.ptr, this, false);
        erase(changed_pos, true);
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
            return iterator(ptr, this, false);
          }
        }
      } catch (index_out_of_bound) {
        return iterator(the_end, this, true);
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
            return const_iterator(ptr, this, false);
          }
        }
      } catch (index_out_of_bound) {
        return const_iterator(the_end, this, true);
      }
    }

  };
}

#endif
