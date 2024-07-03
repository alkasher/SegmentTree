#include <vector>
#include <cstddef>

#include "SegmentTreeWithValues.hpp"


template <class T>
class Node {
private:
	size_t tl;
	size_t tr;
	T sum;
	Node<T>* left;
	Node<T>* right;

	void FillChildsRecursive(const Node<T>& node);
public:
	explicit Node<T>() : sum(0), left(nullptr), right(nullptr) {}

	explicit Node<T>(size_t tl, size_t tr, T sum)
		: tl(tl), tr(tr), sum(sum), left(nullptr), right(nullptr) {}

	explicit Node<T>(const Node<T>& node) : tl(node.tl), tr(node.tr),
		sum(node.sum), left(node.left), right(node.right) {}

	Node<T>& operator=(const Node<T>& node);

	void clear();
	~Node<T>() {}

	template <class T>
	friend class DynamicSegmentTree;

	template <class T>
	friend class PersistentSegmentTree;

	template <class T>
	friend void FillRecursively(std::vector<std::pair<size_t, T>>& vect, Node<T>* current);
};

template <class T>
Node<T>& Node<T>::operator=(const Node<T>& node) {
	if (*this == node)
		return *this;
	if (node == nullptr)
		throw 'e';
	this->clear();
	delete this;
	FillChildsRecursive(Node<T>&node);
	return *this;
}

template <class T>
void Node<T>::FillChildsRecursive(const Node<T>& node) {
	if (node->left)
	{
		this->left = new Node<T>(node->left->tl, node->left->tr, node->sum);
		node->left->FillChildsRecursive(this->left);
	}
	if (node->right) {
		this->right = new Node<T>(node->right->tl, node->right->tr, node->sum);
		node->right->FillChildsRecursive(this->right);
	}
}
template <class T>
void Node<T>::clear() {
	if (left) {
		left->clear();
		delete left;
	}
	if (right) {
		right->clear();
		delete right;
	}
}

/*
Смысл сжатого дерева отрезков в том, чтобы представить наше динамическое дерево в виде массива.
Такое возможно, только если заранее известно, что изменения новых точек не будет, лишь обновление
старых, таким образом, мы заносим старые точки в один массив, как это делали в Segment_Tree. Но
Мы хотим уметь поддерживать работу с прошлым деревом, а занесение точек в новый массив сбивает
индексы. Например, если раньше работали из 1000 точек только с восьмой, девятой, десятой, то
занеся их в новый массив они станут нулевой, второй, третьей. Чтобы поддерживать предыдущие
индексы сжатое дереов хранит не только значения в точках, но и их прошлые индексы: для этого
используется std::pair<size_t, T>
*/
template <class T>
class CompressedTree {
private:
	size_t size;
	size_t actual_size;
	std::vector<std::pair<size_t, T>> array;

	T GetSumNorm(size_t left, size_t right);
	T GetSum(size_t position, size_t tl, size_t tr, size_t l, size_t r);
public:

	explicit CompressedTree<T>(std::vector<std::pair<size_t, T>> vect);

	void UpdateElement(size_t position, T value);

	T GetSum(size_t left, size_t right);
};

/*
Динамическое дерево отрезков используется, когда общий отрезок огромен, но фактическая работа происходит
в небольшом числе точек. Например, если отрезок это [0, 10^9], а работа происходит только с одним миллионом из них.
В таком случае, использовать один массив на миллиард элементов, как это было раньше, не целесообразно. Тогда, дерево
можно реализовать именно так, как оно представляется визуально, используя узлы и ссылки в них на детей. Узел
создается тогда и только тогда, когда с соответствующей точкой происходит работа, создавая вместе с этим промежуточные
узлы от корня дерева вплоть до узла точки.

size - размер всего дерева, как и в случае SegmentTree для удобства работы я использую здесь увеличение числа настоящего
массива до 2^k, actual_size сохраняет же сколько точек было на самом деле. */
template <class T>
class DynamicSegmentTree {
private:
	size_t size;
	Node<T>* head;

	void UpdateElement(size_t position, T value, size_t tl, size_t tr, Node<T>* cur_pos);

	T GetSum(size_t left, size_t right, Node<T>* cur_pos);

public:

	explicit DynamicSegmentTree<T>(Node<T>* head, size_t size) : head(head), size(size) {}
	explicit DynamicSegmentTree<T>(size_t size) : head(nullptr), size(size) {}

	void UpdateElement(size_t position, T value);
	void SetElement(size_t position, T value);

	T GetSum(size_t left, size_t right);

	bool IsExist(size_t position);
	T GetValue(size_t position);

	CompressedTree<T> Compress();

	~DynamicSegmentTree<T>();
};




/*
update a[i] by value, which means a[i]+=value
*/
template <class T>
void DynamicSegmentTree<T>::UpdateElement(size_t position, T value) {
	if (!head)
		head = new Node<T>(0, size - 1, value);
	else
		head->sum += value;
	UpdateElement(position, value, 0, size - 1, head);
}

template <class T>
void DynamicSegmentTree<T>::SetElement(size_t position, T value) {
	if (IsExist(position)) {
		T new_value = value - GetValue(position);
		UpdateElement(position, new_value);
	}
	else {
		UpdateElement(position, value);
	}
	/*
	надо пройти вниз, плюcуя всем значени value, а затем пойти наверх, отнимая у всех то, что лежит в элементе
	занимает o(3logn): понять есть ли элемент, взять его значение, пройтись до него.
	можно сделать все за два прохода -> o(2*logn)
	*/
}

template <class T>
bool DynamicSegmentTree<T>::IsExist(size_t position) {
	if (!head) return false;
	Node<T>* current = head;
	size_t mid;
	while (current) {
		if (current->tl == current->tr)
			return true;
		mid = (current->tl + current->tr) / 2 + 1;
		if (position < mid) {
			current = current->left;
		}
		else {
			current = current->right;
		}
	}
	return false;
}

template <class T>
T DynamicSegmentTree<T>::GetValue(size_t position) {
	Node<T>* current = head;
	size_t mid;
	while (current) {
		if (current->tl == current->tr)
			return current->sum;
		mid = (current->tl + current->tr) / 2 + 1;
		if (position < mid) {
			current = current->left;
		}
		else {
			current = current->right;
		}
	}
}

template <class T>
T DynamicSegmentTree<T>::GetSum(size_t left, size_t right) {
	if (!head) return 0;
	return GetSum(left, right, head);
}

template <class T>
void DynamicSegmentTree<T>::UpdateElement(size_t position, T value, size_t tl, size_t tr, Node<T>* cur_pos) {
	if (tl == tr) {
		return;
	}
	size_t mid = (tl + tr) / 2 + 1;
	if (position < mid) {
		if (!cur_pos->left) {
			cur_pos->left = new Node<T>(tl, mid - 1, value);
		}
		else {
			cur_pos->left->sum += value;
		}
		UpdateElement(position, value, tl, mid - 1, cur_pos->left);
	}
	else {
		if (!cur_pos->right) {
			cur_pos->right = new Node<T>(mid, tr, value);
		}
		else {
			cur_pos->right->sum += value;
		}
		UpdateElement(position, value, mid, tr, cur_pos->right);
	}

}

template <class T>
T DynamicSegmentTree<T>::GetSum(size_t left, size_t right, Node<T>* cur_pos) {
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
void FillRecursively(std::vector<std::pair<size_t, T>>& vect, Node<T>* current) {
	if (current->tl == current->tr) {
		vect.push_back(std::pair<size_t, T>(current->tl, current->sum));
		return;
	}

	if (current->left)
	{
		FillRecursively(vect, current->left);
	}
	if (current->right) {
		FillRecursively(vect, current->right);
	}
}

template <class T>
CompressedTree<T> DynamicSegmentTree<T>::Compress() {
	std::vector<std::pair<size_t, T>> vect;
	FillRecursively(vect, head);
	return CompressedTree<T>(vect);
}

template <class T>
CompressedTree<T>::CompressedTree(std::vector<std::pair<size_t, T>> vect) {
	size_t size_ = vect.size();
	actual_size = size_;
	size_ = std::pow(2, findk(size_));
	vect.resize(size_);

	this->size = 2 * size_ - 1;
	this->array.resize(this->size, std::pair<size_t, T>(0, T(0)));

	for (int i = size_ - 1; i < this->size; ++i) {
		this->array[i].second = vect[i - size_ + 1].second;
		this->array[i].first = vect[i - size_ + 1].first;
	}//the last row of a tree = vect

	for (int i = size_ - 2; i >= 0; --i) {
		this->array[i].second = this->array[2 * i + 1].second + this->array[2 * i + 2].second;
	}
}

/*
Вот тут position это не position по отношению к сжатому дереву,
а позиция элемента в динамическом дереве, эти позиции как раз хранятся
в первом элементе пары.

Зато благодаря хранению элементов в виде массива, ясно, с какой позиции лежат
точки( с (size+1)/2 - 1 ) и сколько всего точек (actual_size)

Теперь остается у элементов с (size+1)/2 - 1 до actual_size
найти нужную position бинарным поиском(все позиции идут от меньшей к большей),
изменить элемент на данной позиции и пройтись вверх по дереву, изменяя все значения*/

template <class T>
void CompressedTree<T>::UpdateElement(size_t position, T value) {
	size_t start = 0;
	size_t tmp_size = (size + 1) / 2 - 1;
	size_t finish = actual_size;
	while (start <= finish) {
		size_t mid = (start + finish) / 2;
		if (array[tmp_size + mid].first == position) {
			array[tmp_size + mid].second += value;
			mid += tmp_size;
			while (true) {
				if (mid / 2 - !(mid % 2) >= 0) {
					mid = mid / 2 - !(mid % 2) * (mid / 2 != 0);
					array[mid].second += value;
					if (!mid) break;
				}

			}
			/*
			for (int i = tmp_size - 1; i >= 0; --i) {//тут я делал пересчет всего дерева, но это ведь глупо,
			я должен пересчитывать лишь родителей, так что идем по mid / 2 - !(mid % 2)
				this->array[i].second = this->array[2 * i + 1].second + this->array[2 * i + 2].second;
			}*/
			return;
		}
		if (array[tmp_size + mid].first > position) {
			finish = mid - 1;
			continue;
		}
		if (array[tmp_size + mid].first < position) {
			start = mid + 1;
			continue;
		}
	}
}

template <class T>
T CompressedTree<T>::GetSum(size_t left, size_t right) {
	size_t start = 0;
	size_t tmp_size = (size + 1) / 2 - 1;
	size_t finish = actual_size - 1;
	while (start <= finish) {
		size_t mid = (finish + start) / 2;
		if (array[tmp_size + mid].first >= left) {
			if (mid == 0) break;
			finish = mid - 1;
			continue;
		}
		if (array[tmp_size + mid].first < left) {
			start = mid + 1;
			continue;
		}
	}
	size_t tl = 0;
	tl = start;

	start = 0;
	finish = actual_size - 1;
	while (start <= finish) {
		size_t mid = (finish + start) / 2;
		if (array[tmp_size + mid].first > right) {
			finish = mid - 1;
			continue;
		}
		else {
			start = mid + 1;
			continue;
		}
	}
	size_t tr = finish;
	return this->GetSumNorm(tl, tr);
	//std::cout << array[tmp_size+tl].first << " " << array[tmp_size+tr].first;
	//return T(0);
}

template <class T>
T CompressedTree<T>::GetSumNorm(size_t left, size_t right) {
	if (left < 0 || right >= (size + 1) / 2 || right < left)
		throw 'e';
	return GetSum(0, 0, (size + 1) / 2 - 1, left, right);
}

template <class T>
T CompressedTree<T>::GetSum(size_t position, size_t tl, size_t tr, size_t l, size_t r) {
	if (tl >= l && tr <= r)
		return array[position].second;
	size_t mid = (tl + tr) / 2 + 1;
	T ans = T(0);
	if (l < mid) {
		ans += GetSum(position * 2 + 1, tl, mid - 1, l, std::min(r, mid - 1));

	}
	if (r >= mid) {
		ans += GetSum(position * 2 + 2, mid, tr, std::max(l, mid), r);
	}
	return ans;
}

template <class T>
DynamicSegmentTree<T>::~DynamicSegmentTree() {
	head->clear();
	if (head)
		delete head;
}