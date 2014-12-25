#include<iostream>
#include<vector>
#include<queue>
#include<string>
#include<fstream>
#include<cmath>
#include "Decision_Tree.h"

using namespace std;
// 建树前的初始化
int isadded_attri[17];
int num_of_trainning_set;
double Pr_CA_De[17][4];
void ini(Node* &root, int num_of_ts) {
	for(int i =0 ; i<17; i++)
		isadded_attri[i] =0;
	num_of_trainning_set = num_of_ts;
	read_trainning_instance("dataset.txt", num_of_trainning_set, root);  
}
// 训练样本要采样多少条数据项（来源于dataset）,返回决策树的根节点
void read_trainning_instance(string name, int num_instance, Node* &root) {
	ifstream dataset;
	ifstream prca;
	int i, j, attri_index, count;
	Person p;
	string party_name= "";
	count = 0;
	dataset.open(name);
	char buffer[50];
	while (dataset.getline(buffer, 50) && count < num_instance) {
        for (i = 0; buffer[i] != ','; i++) 
			party_name += buffer[i];
		p.pre_attribute =( party_name == "republican")? 2:1; // 读取该人党派
	   // 读取该人各项属性值
		for(attri_index = 1; buffer[i] != '\0'; i++) {
			if(buffer[i] != ',') {
			   p.attributes[attri_index] = buffer[i];
			   attri_index++;
			}
		}
		root->Person_heap.push_back(p);		
		count++;
		party_name= "";
		
    }
	dataset.close();
	// 读取所需的先验概率值
	prca.open("PR_CA.txt");
		for(i = 1; i < 17; i++) 
			for(j = 1; j < 4; j++) 
				prca >> Pr_CA_De[i][j];	
	prca.close();
}


//计算一个节点的熵值
double cal_node_entropy(Node* &node) {
	double  sum,entropy;
	int num_re, num_de;
	vector<Person>::iterator it;
	for (num_re = 0, num_de = 0, it = node->Person_heap.begin(); it != node->Person_heap.end(); it++) {
		if(it->pre_attribute == 1)
			num_de++;
		else
			num_re++;
	}
	if (num_de == 0 || num_re == 0) // 该节点中只包含一类党派，“纯净的”
		return 0;
	sum = num_de + num_re;
	entropy = -num_de/sum * log(num_de/sum) / log(2.0)  -num_re/sum * log(num_re/sum) / log(2.0);
	return entropy;
}
/*
  计算根据某一属性分类后的所有堆的总熵值（加权），选取总熵值最小的属性成为最佳分割属性
*/
double cal_atrri_minH(Node* &father, int attri_num) {
	vector<Person>::iterator it;
	Node *first_temp, *second_temp, *third_temp;
	double H1, H2, H3, total, sum_num;
	int num1, num2, num3;
	first_temp = new Node;
	second_temp = new Node;
	third_temp = new Node;
	//遍历父节点中每个人的信息，根据所选的属性的对应值，将每个人对应加入三个堆里
	for (it = father->Person_heap.begin(); it != father->Person_heap.end(); it++) {	
		Person temp;
		temp.attributes[attri_num] = it->attributes[attri_num];
		temp.pre_attribute = it->pre_attribute;
		if(temp.attributes[attri_num] == 'y')
			first_temp->Person_heap.push_back(temp);
		else if (temp.attributes[attri_num] == 'n')
			second_temp->Person_heap.push_back(temp);
		else
			third_temp->Person_heap.push_back(temp);
	}
	//计算每一堆的人数
	num1 = first_temp->Person_heap.size();
	num2 = second_temp->Person_heap.size();
	num3 = third_temp->Person_heap.size();
	sum_num = num1 + num2 + num3;
	// 得到在选定属性下关于 'y' 'n' '?'划分的三堆的熵值
	H1 = cal_node_entropy(first_temp);
	H2 = cal_node_entropy(second_temp);
	H3 = cal_node_entropy(third_temp);

	// 加权计算总的熵值
	total = num1 / sum_num * H1 + num2 / sum_num * H2 + num3 /sum_num * H3;
	return total;
}
/*
int cal_uncertain_party(int attri_index){
}
*/
// 选择最佳分割属性，返回最佳属性的序号
int choose_best_attri(Node* &father) {
	int flag = 0;
	double minH = 1; //
	double tempH;
	for(int i = 1; i < 17; i++) {
		if (isadded_attri[i] == 0) {
			tempH = cal_atrri_minH(father, i);
			if (tempH <= minH) {
				minH = tempH;
				flag = i;
			}
		}
	}
	// 设置该属性已使用
	isadded_attri[flag] = 1;
	return flag;
}
//确定最佳属性后，将分堆加入叶子节点
void add_leaf_node(Node* &father, int attri_num) {
	vector<Person>::iterator it;
	father->first_y = new Node;
	father->second_n = new Node;
	father->third_miss = new Node;
	string temp = father->attributes_num;
	temp += char(attri_num + 48);
	father->first_y->attributes_num = father->second_n->attributes_num = father->third_miss->attributes_num = temp;
	for (it = father->Person_heap.begin(); it != father->Person_heap.end(); it++) {
		Person temp = *it;
		if(temp.attributes[attri_num] == 'y') 
			father->first_y->Person_heap.push_back(temp);

		else if (temp.attributes[attri_num] == 'n')
			father->second_n->Person_heap.push_back(temp);
		else
			father->third_miss->Person_heap.push_back(temp);
	}
    // 确保是满三叉树，即除了叶子节点外， 每个结点都有三个孩子,根据所用的区分属性的先验概率，随机选择一个党派。
	
	int i;
	double pd, pr ;
	if(father->first_y->Person_heap.empty()) {
		pd = pr = 0.0;
		Person virtual_p;
		for(i = 0; father->attributes_num[i] != '\0'; i++) {
			if( father->attributes_num[i] != '0') {
			    pd += (log(Pr_CA_De[father->attributes_num[i] - 48][1]) /log(2));
			    pr += (log(1-Pr_CA_De[father->attributes_num[i] - 48][1]) /log(2));
			}
		}
		    virtual_p.pre_attribute = pd > pr ? 1:2;
			father->first_y->Person_heap.push_back(virtual_p);
	}
	if(father->second_n->Person_heap.empty()) {
			pd = pr = 0.0;
			Person virtual_p;
		for(i = 0; father->attributes_num[i] != '\0'; i++) {
			if( father->attributes_num[i] != '0') {
			    pd += (log(Pr_CA_De[ father->attributes_num[i] - 48][2]) /log(2));
			    pr += (log(1-Pr_CA_De[father->attributes_num[i] - 48][2]) /log(2));
			}
		}
			virtual_p.pre_attribute = pd > pr ? 1:2;
			father->second_n->Person_heap.push_back(virtual_p);
	}
	if(father->third_miss->Person_heap.empty()) {
		Person virtual_p;
			pd = pr = 0.0;
		for(i = 0;  father->attributes_num[i] != '\0'; i++) {
			if( father->attributes_num[i] != '0') {
			    pd += (log(Pr_CA_De[ father->attributes_num[i] - 48][3]) /log(2));
			    pr += (log(1 - Pr_CA_De[father->attributes_num[i] - 48][3]) /log(2));
			}
		}
			virtual_p.pre_attribute = pd > pr ? 1:2;
			father->third_miss->Person_heap.push_back(virtual_p);
	}
	
}

// 建立决策树
void create_tree(Node* &root) {
	queue<Node*> q;
	Node *temp = new Node;
	int best_attri;
	q.push(root);
	while(!q.empty()) {
		temp = q.front();
		temp->H = cal_node_entropy(temp);
		// 如果该节点“不纯”,则选取一个未使用的最佳分割属性进行分割
		if (temp->H != 0) {
		    best_attri = choose_best_attri(temp);
			// 有可以分割的属性
			if (best_attri != 0) {
				temp->attributes_num += (char)(best_attri + 48); //记录到目前为止所用到的所有判断属性
				temp->node_attri = best_attri;// 记录该节点选用的用于分堆的属性
				add_leaf_node(temp,best_attri);
				q.push(temp->first_y);
				q.push(temp->second_n);
				q.push(temp->third_miss);
			}
			// 解决属性用完后仍然有杂堆
			else {
	            int num_re, num_de, i;
	            vector<Person>::iterator it;
	            for (num_re = 0, num_de = 0, it = temp->Person_heap.begin(); it != temp->Person_heap.end(); it++) {
		        if(it->pre_attribute == 1)
			        num_de++;
		        else
			      num_re++;
	            }			
	            temp->Person_heap.front().pre_attribute = (num_re > num_de)? 2:1;
			}
		}
		q.pop();
	}
}

// 一系列剪枝算法，对应输出不同的决策树到文件，方便比较不同决策树的准确率


void no_cut_thumb(Node* &root) {

	// 输出最原始的决策树，检查构建情况
	/*
	Node* temp = new Node;
	queue<Node*> q;
	vector<Person>::iterator it;
	q.push(root);
	while(!q.empty()) {
		temp = q.front();
		cout << "this heap is separate by arrtibute " << temp->attribute << " has number of persons " << temp->Person_heap.size() <<endl;
		if(temp->attribute!=0) {
		cout << "each person's party:" <<endl;	
		for(it = temp->Person_heap.begin(); it != temp->Person_heap.end(); it++)
			cout << it->pre_attribute << " ";
		cout << endl;
		}


		else {
			cout << temp->Person_heap.front().pre_attribute << " " << endl;
		}
		cout <<" the entropy of this heap is : " << temp->H <<endl;
		if(temp->first_y != NULL &&temp->first_y->Person_heap.size() != 0)
			q.push(temp->first_y);
		if(temp->second_n != NULL &&temp->second_n->Person_heap.size() != 0)
			q.push(temp->second_n);
		if(temp->third_miss != NULL && temp->third_miss->Person_heap.size() != 0 )
			q.push(temp->third_miss);
		q.pop();
	}
	*/
	
}
void cut_thumb_REP(Node* &root) {

}
void cut_thumb_PEP(Node* &root) {

}
void cut_thumb_cross(Node* &root) {

}
/*
  将建立并剪枝后的决策树写入文件，命名为剪枝算法名加用户自定义名
  写入格式： 层次遍历决策树，只记录决策树每个结点的属性值序号和对应子节点的属性值序号信息
  如 1：first_y = 3, second_n = 7, third_miss = 13;
*/

double test_accuracy (Node* &root) {
	ifstream dataset;
	dataset.open("dataset.txt");
	char buffer[50];
	int i, count, attri_index;
	double correct_num, error_num;
	string party_name="";
	count = 0;
	correct_num = error_num = 0.0;
	while (dataset.getline(buffer, 50)) {
	  if(count > num_of_trainning_set){ 
		Person p;
        for (i = 0; buffer[i] != ','; i++) 
			party_name += buffer[i];
		p.pre_attribute =( party_name == "republican")? 2:1; // 读取该人党派
		party_name ="";
	   // 读取该人各项属性值
		for(attri_index = 1; buffer[i] != '\0'; i++) {
			if(buffer[i] != ',') {
			   p.attributes[attri_index] = buffer[i];
			   attri_index++;
			}
		}
		//测试该人根据决策树是否满足实际值。满足则correct_num 加1， 否则 error_num 加1
		Node* temp = new Node;
		temp = root;
		while( temp->node_attri!= 0) {
			
			if(p.attributes[temp->node_attri] == 'y')
				temp = temp->first_y;
			else if (p.attributes[temp->node_attri] == 'n')
				temp =temp->second_n;
			else
				temp =temp->third_miss;
		}
		
		if(p.pre_attribute == temp->Person_heap.front().pre_attribute)
			correct_num += 1.0;
		else
			error_num += 1.0;
	}
	  count++;
  }
	return correct_num /(correct_num+error_num);
}

