#pragma once

#include <iostream>
#include <algorithm>

template<typename T>
class Array final {
private:
	T* items;
	int size;
	int capacity;
	static const int defaultCapacity = 8;
	static const int kResizeCoef = 2;

	template<bool Const, bool Reverse>
	class Iterator {
	private:
		Array& array;
		T* ptr;

	public:
		Iterator(Array& array) : array(array) {
			if (Reverse)
				ptr = array.items + array.size - 1;
			else
				ptr = array.items;
		}

		const T& get() const {
			return *ptr;
		}

		void set(const T& value) {
			if (!Const) {
				*ptr = value;
			}
		}

		void next() {
			if (!hasNext())
				throw "Iterator out of range";

			if (Reverse)
				ptr--;
			else
				ptr++;
		}

		bool hasNext() const {
			if (Reverse)
				return ptr >= array.items;
			else
				return ptr < array.items + array.size;
		}
	};

public:
	Array();
	Array(int capacity);
	Array(const Array& other); 
	Array(Array&& other); 
	~Array();

	int insert(const T& value);
	int insert(int index, const T& value);
	void remove(int index);
	int getSize() const;
	int getCapacity() const;

	Iterator<false, false> iterator();
	Iterator<true, false> iterator() const;
	Iterator<false, true> reverseIterator();
	Iterator<true, true> reverseIterator() const;

	Array& operator=(const Array& other); 
	Array& operator=(Array&& other); 
	const T& operator[](int index) const;
	T& operator[](int index);

private:
	void resize();
	void deepCopy(const Array& other);
	T* allocate(size_t capacity);
	T* allocateWithInit(int size, size_t capacity);
	void destruct();
};

template<typename T>
 Array<T>::Array() {
	this->items = nullptr;
	this->size = 0;
	this->capacity = defaultCapacity;
}

template<typename T>
 Array<T>::Array(int capacity) {
	this->size = 0;

	if (capacity == 0)
		this->capacity = defaultCapacity;
	else
		this->capacity = capacity;

	items = allocate(capacity);
}

// Constcopy
template<typename T>
Array<T>::Array(const Array& other) {
	deepCopy(const_cast<Array&>(other));
}

// Constmove
template<typename T>
 Array<T>::Array(Array&& other) {
	size = other.size;
	capacity = other.capacity;
	items = other.items;

	other.size = 0;
	other.capacity = 0;
	other.items = nullptr;
}

template<typename T>
 Array<T>::~Array() {
	destruct();
}


template<typename T>
 int Array<T>::insert(const T& value) {
	if (size == capacity)
		resize();
	else if (!items)
		items = allocate(capacity);

	new(items + size) T(value);
	size++;

	return size - 1;
}

template<typename T>
 int Array<T>::insert(int index, const T& value) {
	if (size == capacity)
		resize();
	if (!items)
		items = allocate(capacity);

	for (int i = size; i > index; i--)
		new(items + i) T(std::move(items[i - 1]));

	items[index].~T();
	new(items + index) T(value);
	size++;

	return index;
}

template<typename T>
 void Array<T>::remove(int index) {
	if (size == 0)
		throw "Array is empty";

	for (int i = index; i < size - 1; i++)
		items[i] = std::move(items[i + 1]);

	items[size - 1].~T();
	size--;
}

template<typename T>
 int Array<T>::getSize() const {
	return size;
}

template<typename T>
 int Array<T>::getCapacity() const {
	return capacity;
}


template<typename T>
 Array<T>::Iterator<false, false> Array<T>::iterator() {
	return Iterator<false, false>(*this);
}

template<typename T>
Array<T>::Iterator<true, false> Array<T>::iterator() const {
	return Iterator<true, false>(*this);
}

template<typename T>
 Array<T>::Iterator<false, true> Array<T>::reverseIterator() {
	return Iterator<false, true>(*this);
}

template<typename T>
 Array<T>::Iterator<true, true> Array<T>::reverseIterator() const {
	return Iterator<true, true>(*this);
}


 
template<typename T>
 Array<T>& Array<T>::operator=(const Array& other) {
	if (this != &other) {
		destruct();
		deepCopy(other);
	}
	return *this;
}


template<typename T>
 Array<T>& Array<T>::operator=(Array&& other) {
	if (this != &other) {
		for (int i = 0; i < size; i++)
			items[i].~T();

		size = other.size;
		capacity = other.capacity;
		items = other.items;

		other.size = 0;
		other.capacity = 0;
		other.items = nullptr;
	}

	return *this;
}

template<typename T>
 const T& Array<T>::operator[](int index) const {
	return items[index];
}

template<typename T>
 T& Array<T>::operator[](int index) {
	return items[index];
}


template<typename T>
void Array<T>::resize() {
	capacity *= kResizeCoef;
	T* temp = allocate(capacity);

	for (int i = 0; i < size; i++)
		new(temp + i) T(std::move(items[i]));

	std::free(items);
	items = temp;
}

template<typename T>
void Array<T>::deepCopy(const Array& other) {
	size = other.size;
	capacity = other.capacity;
	items = allocate(capacity);

	for (int i = 0; i < other.getSize(); i++)
		new(items + i) T(other[i]);
}

template<typename T>
 T* Array<T>::allocate(size_t capacity) {
	return static_cast<T*>(std::malloc(capacity * sizeof(T)));
}

template<typename T>
 void Array<T>::destruct() {
	for (int i = 0; i < size; i++)
		items[i].~T();

	std::free(items);
}