#include <iostream>
#include<vector>
#include<string>
#include "Decision_Tree.h"
using namespace std;
int main() {
	int i;
	double sum, ave, t ;
	sum = ave = 0.0;
	ofstream accurcise;
	accurcise.open("test.txt");
	for(i = 0; i < 5; i++) {
	Node* Decsion_tree = new Node;
    ini(Decsion_tree, i);
	create_tree(Decsion_tree);
	no_cut_thumb(Decsion_tree);
	t = test_accuracy(Decsion_tree, i);
	accurcise << i << " " << t << endl;
	sum += t;
	}
		accurcise << "average accuracy：  "<< " " << sum / 5.0000 << endl;
}
