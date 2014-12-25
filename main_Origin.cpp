#include <iostream>
#include<vector>
#include<string>
#include "Decision_Tree.h"
using namespace std;
int main() {
	int i;
	ofstream accurcise;
	accurcise.open("test.txt");
	for(i= 20; i<=400; i=i+5) {
	Node* Decsion_tree = new Node;
    ini(Decsion_tree, i);
	create_tree(Decsion_tree);
	accurcise << i << " " << test_accuracy(Decsion_tree) << endl;
	}
}
