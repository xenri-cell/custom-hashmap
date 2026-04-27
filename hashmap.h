#pragma once

#include <iostream>
#include <sstream>
#include <stdexcept>
#include <utility>

using namespace std;

template <typename KeyT, typename ValT>
class HashMap {
 private:
  struct ChainNode {
    const KeyT key;
    ValT value;
    ChainNode* next;

    ChainNode(KeyT key, ValT value) : key(key), value(value), next(nullptr) {
    }

    ChainNode(KeyT key, ValT value, ChainNode* next)
        : key(key), value(value), next(next) {
    }
  };

  ChainNode** data;
  size_t sz;
  size_t capacity;

  // Utility members for begin/next
  ChainNode* curr;
  size_t curr_idx;

  size_t hash_index(const KeyT& key) const{
    return std::hash<KeyT>{}(key) % capacity;
  }

  void resize_and_rehash()  {
    ChainNode** old_data = data;
    size_t old_capacity = capacity;

    capacity *= 2;
    data = new ChainNode*[capacity];
    for (size_t i = 0; i < capacity; ++i)  {
      data[i] = nullptr;
    }
    sz = 0;

    for (size_t i = 0; i < old_capacity; ++i)  {
      ChainNode* current = old_data[i];
      while (current != nullptr)  {
        ChainNode* next = current->next;

        size_t new_index = hash_index(current->key);

        current->next = data[new_index];
        data[new_index] = current;

        sz++;
        current = next;
      }
    }
    delete[] old_data;
  }

  void clear_nodes()  {
    for (size_t i = 0; i < capacity; ++i)  {
      ChainNode* current = data[i];
      while (current != nullptr)  {
        ChainNode* next = current->next;
        delete current;
        current = next;
      }
      data[i] = nullptr;
    }
    sz = 0;
  }

 public:
  /**
   * Creates an empty `HashMap` with 10 buckets.
   */

  HashMap() : HashMap(10){
    // TODO_STUDENT
  }

  /**
   * Creates an empty `HashMap` with `capacity` buckets.
   */
  HashMap(size_t capacity) {
    this->sz = 0;
    this->capacity = capacity > 0 ? capacity : 1;
    this->curr = nullptr;
    this->curr_idx = 0;
    this->data = new ChainNode*[this->capacity];

    for (size_t i = 0; i < this->capacity; ++i)  {
      this->data[i] = nullptr;
    }

    // TODO_STUDENT
  }

  /**
   * Checks if the `HashMap` is empty. Runs in O(1).
   */
  bool empty() const {
    // TODO_STUDENT
    return this->sz ==  0;
  }

  /**
   * Returns the number of mappings in the `HashMap`. Runs in O(1).
   */
  size_t size() const {
    // TODO_STUDENT
    return this->sz;
  }

  /**
   * Adds the mapping `{key -> value}` to the `HashMap`. If the key already
   * exists, does not update the mapping (like the C++ STL map).
   *
   * Uses the hash value of the key to determine the location in the
   * underlying hash table. Creates exactly one new node; resizes by doubling
   * when the load factor exceeds 1.5.
   *
   * On resize, doesn't create new nodes, but rearranges existing ones.
   *
   * Runs in O(L), where L is the length of the longest chain.
   */
  void insert(KeyT key, ValT value) {
    size_t index = this->hash_index(key);
    ChainNode* current = this->data[index];

    while (current != nullptr)  {
      if (current->key == key)  {
        return;
      }
      current = current->next;
    }
    ChainNode* new_node = new ChainNode(key, value, this->data[index]);
    this->data[index] = new_node;
    this->sz++;

    if ((double)this->sz / this->capacity > 1.5)  {
      this->resize_and_rehash();
    }
    // TODO_STUDENT
  }

  /**
   * Return a reference to the value stored for `key` in the map.
   *
   * If key is not present in the map, throw `out_of_range` exception.
   *
   * Runs in O(L), where L is the length of the longest chain.
   */
  ValT& at(const KeyT& key) const {
    size_t index = this->hash_index(key);

    ChainNode* current = this->data[index];
    while (current != nullptr)  {
      if (current->key == key)  {
        return current->value;
      }
      current = current->next;
    }
    // TODO_STUDENT
    throw out_of_range("Key ot found in HashMap::at");
  }

  /**
   * Returns `true` if the key is present in the map, and false otherwise.
   *
   * Runs in O(L), where L is the length of the longest chain.
   */
  bool contains(const KeyT& key) const {
    size_t index = this->hash_index(key);

    ChainNode* current = this->data[index];
    while (current != nullptr)  {
      if (current->key == key)  {
        return true;
      }
      current = current->next;
    }
    return false;
  }

  /**
   * Empties the `HashMap`, freeing all nodes. The bucket array may be left
   * alone.
   *
   * Runs in O(N+B), where N is the number of mappings and B is the number of
   * buckets.
   */
  void clear() {
    this->clear_nodes(); 
    // TODO_STUDENT
  }

  /**
   * Destructor, cleans up the `HashMap`.
   *
   * Runs in O(N+B), where N is the number of mappings and B is the number of
   * buckets.
   */
  ~HashMap() {
    this->clear_nodes();
    delete[] this->data;
    // TODO_STUDENT
  }

  /**
   * Removes the mapping for the given key from the `HashMap`, and returns the
   * value.
   *
   * Throws `out_of_range` if the key is not present in the map. Creates no new
   * nodes, and does not update the key or value of any existing nodes.
   *
   * Runs in O(L), where L is the length of the longest chain.
   */
  ValT erase(const KeyT& key) {
    size_t index = this->hash_index(key);
    ChainNode* current = this->data[index];
    ChainNode* prev = nullptr;

    while (current != nullptr)  {
      if (current->key == key)  {
        break;
      }
      prev = current;
      current = current->next;
    }
    if (current == nullptr)  {
      throw out_of_range("Key not found in HashMap::erase");
    }
    ValT erased_value = current->value;

    if (prev == nullptr)  {
      this->data[index] = current->next;
    }
    else  {
      prev->next = current->next;
    }
    delete current;
    this->sz--;

    return erased_value;
    // TODO_STUDENT
    //return this->data[0]->value;
  }

  /**
   * Copy constructor.
   *
   * Copies the mappings from the provided `HashMap`.
   *
   * Runs in O(N+B), where N is the number of mappings in `other`, and B is the
   * number of buckets.
   */
  HashMap(const HashMap& other) {
    this->sz = 0;
    this->capacity = other.capacity;
    this->curr = nullptr;
    this->curr_idx = 0;

    this->data = new ChainNode*[this->capacity];
    for (size_t i = 0; i < this->capacity; ++i)  {
      this->data[i] = nullptr;
    }

    for (size_t i = 0; i < other.capacity; ++i)  {
      ChainNode* other_current = other.data[i];
      ChainNode* last_new_node = nullptr;

      while (other_current != nullptr)  {
        ChainNode* new_node = new ChainNode(other_current->key, other_current->value);
        if (this->data[i] == nullptr)  {
          this->data[i] = new_node;
        }
        else  {
          last_new_node->next = new_node;
        }
        last_new_node = new_node;
        this->sz++;
        other_current = other_current->next;
      }
    }
        // TODO_STUDENT
  }

  /**
   * Assignment operator; `operator=`.
   *
   * Clears this table, and copies the mappings from the provided `HashMap`.
   *
   * Runs in O((N1+B1) + (N2+B2)), where N1 and B1 are the number of mappings
   * and buckets in `this`, and N2 and B2 are the number of mappings and buckets
   * in `other`.
   */
  HashMap& operator=(const HashMap& other) {
    if (this != &other)  {
      HashMap temp(other);
      std::swap(this->data, temp.data);
      std::swap(this->sz, temp.sz);
      std::swap(this->capacity, temp.capacity);
    }
    // TODO_STUDENT
    return *this;
  }

  // =====================

  /**
   *
   * Checks if the contents of `this` and `other` are equivalent.
   *
   * Two `HashMap` objects are equivalent if they contain the same
   * mappings from key to value. To be equivalent, the two `HashMap`
   * objects need not have the elements saved in the same order
   * inside of the buckets.
   *
   * Runs in worst-case O(B*L) time, where B is the maximum number
   * of buckets in either of the `HashMap` objects and L is the length
   * of the  largest chain on any of the buckets.
   *
   */
  bool operator==(const HashMap& other) const {
    
    // TODO_STUDENT
    return false;
  }

  /**
   * Resets internal state for an iterative traversal.
   *
   * See `next` for usage details. Modifies nothing except for `curr` and
   * `curr_idx`.
   *
   * Runs in worst-case O(B), where B is the number of buckets.
   */
  void begin() {
    // TODO_STUDENT
  }

  /**
   * Uses the internal state to return the "next" key and value
   * by reference, and advances the internal state. Returns `true` if the
   * reference parameters were set, and `false` otherwise.
   *
   * Example usage:
   *
   * ```c++
   * HashMap<string, int> hm;
   * hm.begin();
   * string key;
   * int value;
   * while (hm.next(key, val)) {
   *   cout << key << ": " << val << endl;
   * }
   * ```
   *
   * Does not visit the mappings in any defined order.
   *
   * Modifies nothing except for `curr` and `curr_idx`.
   *
   * Runs in worst-case O(B) where B is the number of buckets.
   */
  bool next(KeyT& key, ValT& value) {
    // TODO_STUDENT
    return false;
  }

  // ===============================================

  /**
   * Returns a pointer to the underlying memory managed by the `HashMap`.
   * For autograder testing purposes only.
   */
  void* get_data() const {
    return this->data;
  }

  /**
   * Returns the capacity of the underlying memory managed by the `HashMap`. For
   * autograder testing purposes only.
   */
  size_t get_capacity() {
    return this->capacity;
  }
};
