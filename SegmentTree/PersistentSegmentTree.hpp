#include <vector>
#include <cstddef>

#include "DynamicSegmentTree.hpp"


/*
������������� ������ �������� - �����, ��� � ����� ������ �������, �� �����
������, ����� ����� ���� �� ������� � ����� �� ������� ������.
����� ������ - ���������� ������ ��������.

����� ��� �����������, ������ ����� ������������ ��� ������� ���������, � ��
��� ������(*). � ��� �� ����� �������� ������ ���������� �� ����. ���� ������
����� ������� ������ �������� ���� ������ ����� ������. ���� �������:
��� ���������� ��������, ����������� ���� ���� �� ����� �� ������� ��������,
����� �� �������� ����� ������, ��� �������� ��� ��� ����������(�� ������������
�������� ��� �� ����� ���������, ��� ���� ������ �� ���������� ���������),
� ����� ���� ������ ������� � ������.


(*)������,��������� ������������ ������������ ������ ��������, ��
� ���� ��� ������ - �� ����� �������, � ���������� �����(��� �������� ����������
���� ��������� ����� �� ������� ������, ��� ����� ����, ������� ���� �������
���� ����� ������ ����� �� ������� ������, ���������� ��������).

*/
template <class T>
class PersistentSegmentTree {
private:
	size_t size;
	Node<T>* head;
	std::vector<Node<T>*> versions;

	void UpdateElement(size_t position, T value, size_t tl, size_t tr, Node<T>* cur_pos, Node<T>* next_version_pos);

	void MakeNodes(Node<T>* cur_pos, std::vector<T>& vect);

	T GetSum(size_t left, size_t right, Node<T>* cur_pos);


public:
	explicit PersistentSegmentTree<T>(std::vector<T> vect);

	void UpdateElement(size_t position, T value);

	T GetSum(size_t left, size_t right, size_t version);


	~PersistentSegmentTree<T>();
};


/*
������� ��� ������ �� ����� �� �������, � ������ ����������� ���������� �������.
*/
template <class T>
void PersistentSegmentTree<T>::MakeNodes(Node<T>* cur_pos, std::vector<T>& vect) {
	if (cur_pos->tl == cur_pos->tr) {
		cur_pos->sum += vect[cur_pos->tl];
		return;
	}
	size_t mid = (cur_pos->tl + cur_pos->tr) / 2 + 1;

	cur_pos->left = new Node<T>(cur_pos->tl, mid - 1, 0);
	MakeNodes(cur_pos->left, vect);
	cur_pos->right = new Node<T>(mid, cur_pos->tr, 0);
	MakeNodes(cur_pos->right, vect);
	cur_pos->sum += cur_pos->left->sum + cur_pos->right->sum;
}

template <class T>
PersistentSegmentTree<T>::PersistentSegmentTree<T>(std::vector<T> vect) {
	size_t size_ = vect.size();
	static size_t count = 0;

	size_ = std::pow(2, findk(size_));
	vect.resize(size_);

	this->size = 2 * size_ - 1;

	head = new Node<T>(0, (size + 1) / 2 - 1, 0);
	MakeNodes(head, vect);
	versions.push_back(head);

}


template <class T>
void PersistentSegmentTree<T>::UpdateElement(size_t position, T value) {
	Node<T>* next_version_head = new Node<T>(head->tl, head->tr, head->sum);
	next_version_head->sum += value;
	UpdateElement(position, value, 0, (size + 1) / 2 - 1, head, next_version_head);
	head = next_version_head;
	versions.push_back(next_version_head);
}


/*
�� ������ ���� ��������� ���������, � ���������� ���� ���������
�� ���� ���� ��������� = new Node<T>, � ������� ���� ������������*/
template <class T>
void PersistentSegmentTree<T>::UpdateElement(size_t position, T value, size_t tl, size_t tr, Node<T>* cur_pos, Node<T>* next_version_pos) {
	if (tl == tr) {
		return;
	}
	size_t mid = (tl + tr) / 2 + 1;
	if (position < mid) {
		next_version_pos->left = new Node<T>(tl, mid - 1, cur_pos->left->sum + value);
		next_version_pos->right = cur_pos->right;
		UpdateElement(position, value, tl, mid - 1, cur_pos->left, next_version_pos->left);
	}
	else {
		next_version_pos->right = new Node<T>(mid, tr, cur_pos->right->sum + value);
		//next_version_pos->right->sum += value;
		next_version_pos->left = cur_pos->left;
		UpdateElement(position, value, mid, tr, cur_pos->right, next_version_pos->right);
	}
}


/*
������, ���� � ��� ������� ����� ������������ ������, �� ���� ������
��������� ����� �� ���� �����, ������� ������������ ������ ������.
*/
template <class T>
T PersistentSegmentTree<T>::GetSum(size_t left, size_t right, size_t version) {
	if (!head) return 0;
	return GetSum(left, right, versions[version]);
}


template <class T>
T PersistentSegmentTree<T>::GetSum(size_t left, size_t right, Node<T>* cur_pos) {
	if (cur_pos->tl == cur_pos->tr)
		return cur_pos->sum;

	size_t mid = (cur_pos->tl + cur_pos->tr) / 2 + 1;
	T sum = T(0);

	if (left < mid && cur_pos->left) {
		sum += GetSum(left, std::min(right, mid - 1), cur_pos->left);
	}
	if (right >= mid && cur_pos->right) {
		sum += GetSum(std::max(mid, left), right, cur_pos->right);
	}
	return sum;
}

template <class T>
PersistentSegmentTree<T>::~PersistentSegmentTree() {
	head->clear();
	if (head)
		delete head;
}