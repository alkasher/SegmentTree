#include <vector>
#include <cstddef>

template <class T>
class SegmentTree {
private:
	size_t size;
	std::vector<T> array;

	T GetSum(int position, int tl, int tr, int l, int r);
	T GetMin(int left, T sum, int position, int tl, int tr, T cur_sum);
public:
	explicit SegmentTree<T>(std::vector<T> vect);

	void SetElement(size_t position, T value);

	T GetSum(int left, int right);
	T GetMin(int left, T sum);


	~SegmentTree() {}
};

int findk(int len) {//this function takes number and finds int k, that satisfies 2^(k-1)< len <= 2^k
	int k = 0;
	while (len)
	{
		len >>= 1;
		++k;
	}
	return std::pow(2, k - 1) == len ? k - 1 : k;
	//if len == 2^n it does one additional division

}
size_t findk(size_t len) {//this function takes number and finds int k, that satisfies 2^(k-1)< len <= 2^k
	size_t k = 0;
	size_t tmp_len = len;
	while (len)
	{
		len >>= 1;
		++k;
	}
	return std::pow(2, k - 1) == tmp_len ? k - 1 : k;
	//if len == 2^n it does one additional division

}

template <class T>
SegmentTree<T>::SegmentTree<T>(std::vector<T> vect) {
	size_t size_ = vect.size();
	size_ = std::pow(2, findk(size_));
	vect.resize(size_);

	this->size = 2 * size_ - 1;
	this->array.resize(this->size, 0);

	for (int i = size_ - 1; i < this->size; ++i) {
		this->array[i] = vect[i - size_ + 1];
	}//the last row of a tree = vect

	for (int i = size_ - 2; i >= 0; --i) {
		this->array[i] = this->array[2 * i + 1] + this->array[2 * i + 2];
	}
}

template <class T>
void SegmentTree<T>::SetElement(size_t position, T value) {
	this->array[(size + 1) / 2 - 1 + position] = value;
	for (int i = (size + 1) / 2 - 2; i >= 0; --i) {
		this->array[i] = this->array[2 * i + 1] + this->array[2 * i + 2];
	}
}

template <class T>
T SegmentTree<T>::GetSum(int left, int right) {
	if (left < 0 || right >= (size + 1) / 2 || right < left)
		throw 'e';
	return GetSum(0, 0, (size + 1) / 2 - 1, left, right);
}

template <class T>
T SegmentTree<T>::GetSum(int position, int tl, int tr, int l, int r) {
	if (tl >= l && tr <= r)
		return array[position];
	int mid = (tl + tr) / 2 + 1;
	int ans = 0;
	if (l < mid) {
		ans += GetSum(position * 2 + 1, tl, mid - 1, l, std::min(r, mid - 1));

	}
	if (r >= mid) {
		ans += GetSum(position * 2 + 2, mid, tr, std::max(l, mid), r);
	}
	return ans;
}

/*
returns minimum index k such that a[left] + a[left+1] + ... + a[k] >= sum
*/
template <class T>
T SegmentTree<T>::GetMin(int left, T sum) {
	return GetMin(left, sum, 0, 0, (size + 1) / 2 - 1, 0);
}

template <class T>
T SegmentTree<T>::GetMin(int left, T sum, int position, int tl, int tr, T cur_sum) {
	int mid = (tl + tr) / 2 + 1;
	if (2 * position + 1 >= size) {
		return position - ((size + 1) / 2 - 1);
	}
	T LeftSum = GetSum(2 * position + 1, tl, mid - 1, left, mid - 1);
	if (LeftSum + cur_sum >= sum) {
		return GetMin(left, sum, 2 * position + 1, tl, mid - 1, cur_sum);
	}
	else {
		return GetMin(left, sum, 2 * position + 2, mid, tr, LeftSum + cur_sum);
	}
}