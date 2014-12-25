#include<iostream>
#include<string>
#include<queue>
#include<vector>
#include <algorithm>
#include<time.h>
#include<fstream>
using namespace std;
// information of each person
struct Person {
    	/*
		  16 attribute order( 1 ~ 16): 		 
		   handicapped_infants, water_cost, adoption, physician,
		  el_salvador, religious,  anti_satellite, aid_to_nicaraguan, mx,
		  immigration, synfuels, education, superfund, crime, duty, export_admin
		  each has 3 possible value :'y', 'n', '?'.
		*/
	char attributes[17]; 
    // party atrribute: 1 for  democrats ,2 for republican
	int pre_attribute; 
};
// node of decision tree, each node has a vector of Person , its entropy and its children pointers
struct Node {
	vector<Person> Person_heap;
    // the entropy of this node
	double H ; 
	// the sequence number of attribute
	//int attribute; 
	int node_attri;
	string attributes_num;
	// leaf node: each node can be split into 3 heaps, "y" heap, "n" heap, "?" heap.
	Node *first_y, *second_n, *third_miss;
	Node() {
		first_y = second_n = third_miss = NULL;
		attributes_num = "0";
		node_attri = 0;
	}
};

// 建树前的初始化
void ini(Node* &root,int num_of_t_set);
// 训练样本要采样多少条数据项（来源于dataset）,返回决策树的根节点
void read_trainning_instance(string filename, int num_instance, Node* &root); 


//计算一个节点的熵值
double cal_node_entropy(Node* &node);
/*
  计算根据某一属性分类后的所有堆的总熵值（加权），选取总熵值最小的属性成为最佳分割属性
*/
double cal_atrri_minH(Node* &father, int attri_num);
//int cal_uncertain_party(int attri_index);
// 选择最佳分割属性，返回最佳属性的序号
int choose_best_attri(Node* &father);
//确定最佳属性后，将分堆加入叶子节点
void add_leaf_node(Node* &father, int attri_num);
// 建立决策树
void create_tree(Node* &root);
// **暂未实现**   一系列剪枝算法，对应输出不同的决策树到文件，方便比较不同决策树的准确率
void no_cut_thumb(Node* &root);
void cut_thumb_REP(Node* &root);
void cut_thumb_PEP(Node* &root);
/*
  **暂未实现**
  将建立并剪枝后的决策树写入文件，命名为剪枝算法名加用户自定义名
  写入格式： 层次遍历决策树，只记录决策树每个结点的属性值序号和对应子节点的属性值序号信息
  如 1：first_y = 3, second_n = 7, third_miss = 13;
*/
void write_tree_tofile(Node* &root, string name); 
//  **暂未实现** 从文件中读取决策树，重建决策树
void read_tree_fromfile(Node* root, string name);
// 检查决策树预测的准确性
double test_accuracy(Node* &root);
