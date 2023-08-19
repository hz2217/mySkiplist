#ifndef SKIPLIST_H
#define SKIPLIST_H

#include <iostream> 
#include <cstdlib>
#include <cmath>
#include <cstring>
#include <mutex>
#include <fstream>
#include <functional>
using namespace std;

#define STORE_FILE "./store/dumpFile"

mutex mtx;

template<typename K, typename V>
class Node {
private:
	K key_;
	V val_;

public:
	int level_;
	Node<K, V> **forward;

	Node() {};
	Node(const K k, const V v, int level);
	~Node() { delete [] forward; }

	K key() const { return key_; }
	V value() const { return val_; }
	void set_value(V v) { val_ = v; }
};

template<typename K, typename V>
Node<K, V>::Node(const K k, const V v, int level) 
				 : key_(k), val_(v), level_(level) {
	forward = new Node<K, V>*[level];
	memset(forward, 0, sizeof(Node<K, V>*)*level);
}

template<typename K, typename V>
class Skiplist {
private:
	Node<K, V> *head_, *tail_;
	int level_;
	int len_;
	int max_level_;
	function<bool(K&&, K&&)> cmp_;
	
	ofstream file_writer_;
	ifstream file_reader_;

	void get_key_value_from_string(const string &str,
								   string *key, string *value);
	bool is_valid_string(const string &str);
public:
	Skiplist(int max_level, function<bool(K&&, K&&)>);
	~Skiplist();

	bool search(K);
	int insert(K, V);
	void remove(K);

	void display();

	int size();
	int random_level();

	void dump_file();
	void load_file();
};

template<typename K, typename V>
int Skiplist<K, V>::random_level() {
	int level = 1;
	while (rand()%2 && level < max_level_) {
		level++;
	}
	return level;
}

template<typename K, typename V> 
Skiplist<K, V>::Skiplist(int max_level, function<bool(K&&, K&&)> cmp) : 
						 max_level_(max_level), cmp_(cmp), level_(1), len_(0) {
    
    K k;
    V v;
    head_ = new Node<K, V>(k, v, max_level);
}

template<typename K, typename V> 
Skiplist<K, V>::~Skiplist() {
    if (file_writer_.is_open()) {
        file_writer_.close();
    }
    if (file_reader_.is_open()) {
        file_reader_.close();
    }
    delete head_;
}

template<typename K, typename V>
int Skiplist<K, V>::size() { return len_; }

template<typename K, typename V>
bool Skiplist<K, V>::is_valid_string(const string& str) {
    if (str.empty()) {
        return false;
    }
    if (str.find(":") == string::npos) {
        return false;
    }
    return true;
}

template<typename K, typename V>
void Skiplist<K, V>::get_key_value_from_string(const string& str, 
											   string* key, 
											   string* value) {

    if(!is_valid_string(str)) {
        return;
    }
    *key = str.substr(0, str.find(":"));
    *value = str.substr(str.find(":")+1, str.length());
}

template<typename K, typename V>
int Skiplist<K, V>::insert(K k, V v) {
	mtx.lock();

	Node<K, V> *curr = head_;
	Node<K, V> *update[max_level_];
	memset(update, 0, sizeof(Node<K, V>*)*max_level_);

	for (int i = level_-1; i >= 0; --i) {

		// while (curr->forward[i] && cmp_(curr->forward[i]->key(), move(k))) {
		while (curr->forward[i] && 
			   cmp_(curr->forward[i]->key(), static_cast<K &&>(k))) {
			curr = curr->forward[i];
		}
		update[i] = curr;
	}

	curr = update[0]->forward[0];
    if (curr && curr->key() == k) {
	    // cout << "\n-----------Insert------------"; 
        // cout << "\nExists (" << k << " : " << curr->value() << ")\n";
        mtx.unlock();
        return 1;
    }

	int randomlevel = random_level();
	Node<K, V> *node = new Node<K, V>(k, v, randomlevel); 

	for (int i = randomlevel-1; i >= 0; --i) {
		if (i > level_-1) {
			head_->forward[i] = node;
			node->forward[i] = nullptr;
		} else {
			node->forward[i] = update[i]->forward[i];
			update[i]->forward[i] = node;
		}
	}
    // cout << "\n-----------Insert------------"; 
    // cout << "\nInsert  (" << k << " : " << v << ")" << endl;
    level_ = level_ > randomlevel ? level_ : randomlevel;
	len_++;
	mtx.unlock();
	return 0;
}

template<typename K, typename V>
void Skiplist<K, V>::display() {
    cout << "\n----------Skip List----------\n"; 
    for (int i = level_-1; i >= 0; --i) {
        Node<K, V> *node = head_->forward[i]; 
        cout << "Level " << i << ": ";
        while (node != NULL) {
            cout << "(" << node->key() << ":" << node->value() << ") ";
            node = node->forward[i];
        }
        cout << endl;
    }	
	cout << "skiplist size: " << len_ << endl;
}

template<typename K, typename V>
void Skiplist<K, V>::remove(K key) {

	mtx.lock();
	Node<K, V> *curr = head_;
	Node<K, V> *update[max_level_];
    memset(update, 0, sizeof(Node<K, V>*)*(max_level_));

    for (int i = level_-1; i >= 0; --i) {
    	while (curr->forward[i] && cmp_(curr->forward[i]->key(), move(key))) {
    		curr = curr->forward[i];
    	}
    	update[i] = curr;
    }

    curr = update[0]->forward[0];
    V value = curr->value();

    if (!curr || curr->key() != key) return;

    for (int i = level_-1; i >= 0; --i) {
    	if (update[i]->forward[i] != curr) continue;
    	update[i]->forward[i] = curr->forward[i];	
    }

    while ((level_ >= 1) && head_->forward[level_]) {
    	level_--;
    }
    len_--;
    // cout << "\n-----------Remove------------"; 
    // cout << "\nRemove  (" << key << " : " << value << ")" << endl;
    mtx.unlock();
    return;
}

template<typename K, typename V>
bool Skiplist<K, V>::search(K key) {

	// cout << "\n-----------Search------------"; 
    Node<K, V> *curr = head_;

    for (int i = level_-1; i >= 0; --i) {
        while (curr->forward[i] && cmp_(curr->forward[i]->key(), move(key))) {
            curr = curr->forward[i];
        }
    }

    curr = curr->forward[0];

    if (curr && curr->key() == key) {
    	// cout << "\nFound  (" << key << " : " << curr->value() << ")" << endl;
        return true;
    }
    // cout << "\nNot Found  (" << key << " : " << " " << ")" << endl;
    return false;
}

// dump data in memory to file
template<typename K, typename V> 
void Skiplist<K, V>::dump_file() {

	cout << "\n----------Dump_file----------\n"; 
    file_writer_.open(STORE_FILE);
    Node<K, V>* node = head_->forward[0];

    while (node) {
    	file_writer_ << node->key() << ":" << node->value() << "\n";
    	cout << node->key() << ":" << node->value() << "\n";
    	node = node->forward[0];
    }

    file_writer_.flush();
    file_writer_.close();
    return;
}

template<typename K, typename V>
void Skiplist<K, V>::load_file() {

	file_reader_.open(STORE_FILE);
	cout << "\n----------Load_file----------\n"; 
    string line;
    string* key = new string();
    string* value = new string();	

    while (getline(file_reader_, line)) {
    	get_key_value_from_string(line, key, value);
    	if (key->empty() || value->empty()) continue;
    	insert(*key, *value);
        cout << "key:" << *key << "value:" << *value << endl;
    }
    delete key;
    delete value;
    file_reader_.close();
}

#endif // SKIPLIST_H