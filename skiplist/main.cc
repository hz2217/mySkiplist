#include <iostream>
#include "skiplist.h"

using namespace std;

int main(int argc, char const *argv[])
{
	Skiplist skiplist;
	skiplist.Insert(3);	
	skiplist.Insert(6);	
	skiplist.Insert(1);	
	skiplist.Insert(5);	
	skiplist.Insert(9);	
	skiplist.Insert(8);	
	skiplist.Insert(22);	
	skiplist.Insert(7);	
	skiplist.Insert(7);
	skiplist.PrintSkiplist();
	skiplist.Delete(7);
	skiplist.PrintSkiplist();
	return 0;
}