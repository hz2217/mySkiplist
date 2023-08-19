#ifndef SKIPLIST_H
#define SKIPLIST_H

#include <iostream>
// #include <cmath>

#define MAX_LEVEL 32

typedef struct skiplistNode {
	skiplistNode *backward_;
	int v_;

	struct skiplistLevel {
		skiplistNode *forward_;
		unsigned long span_;
	} *level_;

	skiplistNode(int v = 0, int level = MAX_LEVEL) {
		v_ = v;
		backward_ = nullptr;
		level_ = new skiplistLevel[level];
		for (int i = 0; i < level; ++i) {
			level_[i].forward_ = nullptr;
			level_[i].span_ = 0;
		}
	}
	~skiplistNode() {
		delete [] level_;
	}
} skiplistNode;

class Skiplist {
private:
	skiplistNode *head_, *tail_;
	int level_;
	unsigned long length_;

public:
	Skiplist();
	~Skiplist();
	void Insert(int v);
	bool Delete(int v);
	bool Search(int v);
	void PrintSkiplist();
	int RandomLevel();
};

Skiplist::Skiplist() : level_(1), length_(0), tail_(nullptr),
					   head_(new skiplistNode()) {}

Skiplist::~Skiplist() {
	for (int i = 0; i < length_; ++i) {
		skiplistNode *tmp = head_;
		head_ = head_->level_[0].forward_;
		delete tmp;
	}
}

int Skiplist::RandomLevel() {
	int level = 1;
	while (rand()%2 && level < MAX_LEVEL) {
		level++;
	}
	return level;
}

void Skiplist::PrintSkiplist() {
	skiplistNode *curr = head_;
	for (int i = 0; i < length_; ++i) {
		curr = curr->level_[0].forward_;
		std::cout << curr->v_ << " ";
	}
	std::cout << std::endl;

	curr = tail_;
	while (curr != head_) {
		std::cout << curr->v_ << " ";
		curr = curr->backward_;
	}
	std::cout << std::endl;
}

void Skiplist::Insert(int v) {
	skiplistNode *update[MAX_LEVEL];
	unsigned long rank[MAX_LEVEL] = {0};
	skiplistNode *curr = head_;

	for (int i = level_-1; i >= 0; --i) {
		while (curr->level_[i].forward_ && curr->level_[i].forward_->v_ < v) {
			rank[i] += curr->level_[i].span_;
			curr = curr->level_[i].forward_;
		}
		if (i-1 >= 0) rank[i-1] = rank[i];
		update[i] = curr;
	}

	int level = RandomLevel();
	skiplistNode *node = new skiplistNode(v, level);

	for (int i = level-1; i >= 0; --i) {
		if (i > level_-1) {
			head_->level_[i].forward_ = node;
			head_->level_[i].span_ = rank[0]+1;
		} else {
			node->level_[i].forward_ = update[i]->level_[i].forward_;
			update[i]->level_[i].forward_ = node;

			node->level_[i].span_ = update[i]->level_[i].span_ - 
									(rank[0]-rank[i]+1);
			update[i]->level_[i].span_ -= node->level_[i].span_;
		}
	}

	node->backward_ = update[0];
	if (node->level_[0].forward_) {
		node->level_[0].forward_->backward_ = node;
	} else {
		tail_ = node;
	}

	length_++;
	level_ = std::max(level_, level);
}

bool Skiplist::Delete(int v) {
	if (!Search(v)) return false;

	skiplistNode *update[MAX_LEVEL];
	unsigned long rank[MAX_LEVEL] = {0};
	skiplistNode *curr = head_;

	for (int i = level_-1; i >= 0; --i) {
		while (curr->level_[i].forward_ && curr->level_[i].forward_->v_ < v) {
			rank[i] += curr->level_[i].span_;
			curr = curr->level_[i].forward_;
		}
		if (i-1 >= 0) rank[i-1] = rank[i];
		update[i] = curr; 
	}

	skiplistNode *delnode = update[0]->level_[0].forward_;

	for (int i = level_-1; i >= 0; --i) {
		if (update[i]->level_[i].forward_ == delnode) {
			update[i]->level_[i].forward_ = delnode->level_[i].forward_;
			update[i]->level_[i].span_ += delnode->level_[i].span_ - 1;
		} else {
			update[i]->level_[i].span_ -= 1;
		}
	}

	if (delnode->level_[0].forward_) {
		delnode->level_[0].forward_->backward_ = delnode->backward_;
	} else {
		tail_ = delnode->backward_;
	}

	while (level_ > 1 && head_->level_[level_-1].forward_ == nullptr) {
		level_--;
	} 
	length_--;
}

bool Skiplist::Search(int v) {
	skiplistNode *curr = head_;
	for (int i = level_-1; i >= 0; --i) {
		while (curr->level_[i].forward_ && curr->level_[i].forward_->v_ < v) {
			curr = curr->level_[i].forward_;
		}
	}
	if (curr->level_[0].forward_) {
		if (curr->level_[0].forward_->v_ == v) return true;
		return false;
	}
	return false;
}


#endif // SKIPLIST_H