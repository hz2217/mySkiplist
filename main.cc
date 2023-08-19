#include <iostream>
#include "./skiplist.h"
using namespace std;

bool cmp(int a, int b) { return a < b; }

int main() {
	Skiplist<int, string> skiplist(32, cmp);
	skiplist.insert(1, "One");
	skiplist.insert(2, "Two");
	skiplist.insert(3, "Three");
	skiplist.insert(4, "Four");
	skiplist.insert(5, "Five");
	skiplist.insert(6, "Six");
	skiplist.insert(7, "Seven");
	skiplist.insert(8, "Eight");
	skiplist.insert(9, "Nine");
	skiplist.insert(10, "Ten");
	skiplist.insert(11, "Eleven");

	skiplist.display();

	skiplist.dump_file();

	skiplist.search(1);
	skiplist.search(3);

	skiplist.remove(1);
	skiplist.remove(2);
	skiplist.remove(3);
	skiplist.remove(4);
	skiplist.remove(5);
	skiplist.remove(6);
	skiplist.remove(7);
	skiplist.remove(8);
	skiplist.remove(9);
	skiplist.remove(10);
	skiplist.remove(11);

	skiplist.display();
	return 0;
}