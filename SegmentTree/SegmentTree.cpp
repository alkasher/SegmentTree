

#include <iostream>

//#include "SegmentTree.hpp"
#include "PersistentSegmentTree.hpp"


void test1() {
	std::vector<int> vect;
	for (int i = 0; i < 6; ++i) {
		vect.push_back(i);
	}
	vect[2] = 1;
	vect[4] = 3;
	vect[5] = 0;
	vect[0] = 5;
	SegmentTreeWithValues<int>* tree = new SegmentTreeWithValues<int>(vect);
	std::cout << tree->CountLessThan(4, 4, 6);
	delete tree;
}

void test2() {
	DynamicSegmentTree<int>* tree = new DynamicSegmentTree<int>(9);
	tree->UpdateElement(1, 1);
	tree->UpdateElement(0, 0);
	tree->UpdateElement(2, 2);
	tree->UpdateElement(3, 3);
	tree->UpdateElement(6, 6);

	CompressedTree<int> comp = tree->Compress();

	std::cout << comp.GetSum(0, 6);



	delete tree;

}

void test3() {
	std::vector<int> vect;
	for (int i = 0; i < 7; ++i)
		vect.push_back(i);
	PersistentSegmentTree<int>* tree = new PersistentSegmentTree<int>(vect);

	tree->UpdateElement(0, 1);
	tree->UpdateElement(1, 1);

	delete tree;
}

int main()
{
	test1();
}
