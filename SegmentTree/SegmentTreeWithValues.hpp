#include <vector>
#include <cstddef>
#include "SegmentTree.hpp"

template <class T>
class Node_ {
private:
	std::vector<T> vect;
	T value;
public:
	explicit Node_<T>(std::vector<T> vect, T value) : value(value), vect(vect) {}
	explicit Node_<T>(T value, std::vector<T> vect) : Node_<T>(vect, value) {}
	explicit Node_<T>(T value) : value(value), vect(1, value) {}

	~Node_<T>() {}

	template <class T>
	friend class SegmentTreeWithValues;

};

template <class T>
class SegmentTreeWithValues {
private:
	std::vector<Node_<T>*> nodes;
	size_t size;

	size_t CountLessThan(int left, int right, T value, int position, int tl, int tr);

public:
	explicit SegmentTreeWithValues<T>(std::vector<T> vect);

	size_t CountLessThan(int left, int right, T value);

	~SegmentTreeWithValues<T>();
};



template <class T>
SegmentTreeWithValues<T>::SegmentTreeWithValues<T>(std::vector<T> vect) {
	size_t size_ = vect.size();
	size_ = std::pow(2, findk(size_));
	vect.resize(size_);

	this->size = 2 * size_ - 1;
	this->nodes.resize(this->size, 0);

	Node_<T>* current;

	for (int i = size_ - 1; i < this->size; ++i) {
		current = new Node_<T>(vect[i - size_ + 1]);
		this->nodes[i] = current;
	}//the last row of a tree = vect

	for (int i = size_ - 2; i >= 0; --i) {
		std::vector<T> tmp_vect;
		//tmp_vect.insert(tmp_vect.end(), this->nodes[2 * i + 1]->vect.begin(), this->nodes[2 * i + 1]->vect.end());
		//tmp_vect.insert(tmp_vect.end(), this->nodes[2 * i + 2]->vect.begin(), this->nodes[2 * i + 2]->vect.end());
		int k = 0;
		int j = 0;
		while (k < this->nodes[2 * i + 1]->vect.size() && j < this->nodes[2 * i + 2]->vect.size()) {
			if (this->nodes[2 * i + 1]->vect[k] <= this->nodes[2 * i + 2]->vect[j])
			{
				tmp_vect.push_back(this->nodes[2 * i + 1]->vect[k++]);
			}
			else
			{
				tmp_vect.push_back(this->nodes[2 * i + 2]->vect[j++]);
			}
		}
		while (k < this->nodes[2 * i + 1]->vect.size()) {
			tmp_vect.push_back(this->nodes[2 * i + 1]->vect[k++]);
		}
		while (j < this->nodes[2 * i + 2]->vect.size()) {
			tmp_vect.push_back(this->nodes[2 * i + 2]->vect[j++]);
		}
		current = new Node_<T>(this->nodes[2 * i + 1]->value + this->nodes[2 * i + 2]->value,
			tmp_vect);
		this->nodes[i] = current;
	}
}

template <class T>
SegmentTreeWithValues<T>::~SegmentTreeWithValues() {
	size_t size_ = (size + 1) / 2;
	for (int i = size_ - 2; i >= 0; --i) {
		delete this->nodes[2 * i + 2];
		delete this->nodes[2 * i + 1];
	}
	delete this->nodes[0];
}

/*
Посчитать число элементов на отрезке [left, right] таких, что они меньше value*/
template <class T>
size_t SegmentTreeWithValues<T>::CountLessThan(int left, int right, T value) {
	return CountLessThan(left, right, value, 0, 0, (size + 1) / 2 - 1);
}



template <class T>
size_t SegmentTreeWithValues<T>::CountLessThan(int left, int right, T value, int position, int tl, int tr) {
	if (tl >= left && tr <= right) {
		//size_t count = 0;
		int start = 0;
		int finish = this->nodes[position]->vect.size() - 1;
		int mid = 0;// = (start + finish) / 2;
		while (start <= finish) {
			mid = (start + finish) / 2;
			if (this->nodes[position]->vect[mid] < value) {
				start = mid + 1;
				//count += mid;
			}
			else {
				finish = mid - 1;
			}
		}
		return std::max(start, mid);
	}
	//return array[position];
	size_t count = 0;
	int mid = (tl + tr) / 2 + 1;

	if (left < mid) {
		count += CountLessThan(left, right, value, 2 * position + 1, tl, mid - 1);
	}
	if (right >= mid) {
		count += CountLessThan(left, right, value, position * 2 + 2, mid, tr);
	}
	return count;
}
