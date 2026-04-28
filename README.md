# custom-hashmap
A custom templated HashMap implementation in C++ using separate chaining, dynamic resizing, and manual memory management.

🚀 Features
Templated design — supports any key/value types

Separate chaining — collision handling via linked lists

Dynamic resizing — doubles capacity when load factor > 1.5

O(1) average‑case insert, lookup, and erase

🧠 How It Works
Hashing
Keys are hashed using std::hash<KeyT> and mapped into buckets

Collision Handling
Each bucket stores a linked list of ChainNode objects.
New nodes are inserted at the head for O(1) insertion.

Resizing & Rehashing
When the load factor exceeds 1.5, the table doubles in size and all nodes are rehashed into new buckets, without allocating new nodes.

Lookup
at(key) walks the chain and returns a reference to the stored value.
Throws out_of_range if the key is missing.

Deletion
erase(key) removes a node from the chain and returns its value.

🎯 Learning Goals
This project demonstrates:
Understanding of hash tables and collision resolution
Implementing dynamic arrays and linked lists
Managing memory manually (new/delete)
Practicing Big‑O reasoning and performance‑aware design
