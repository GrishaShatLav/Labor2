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

	template<bool Reverse>
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
			*ptr = value;
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

	template<bool Reverse>
	class ConstIterator {
	private:
		Array& array;
		T* ptr;

	public:
		ConstIterator(Array& array) : array(array) {
			if (Reverse)
				ptr = array.items + array.size - 1;
			else
				ptr = array.items;
		}

		const T& get() const {
			return *ptr;
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
	Array(const Array& other); // Copy
	Array(Array&& other); // Move
	~Array();

	int insert(const T& value);
	int insert(int index, const T& value);
	void remove(int index);
	int getSize() const;
	int getCapacity() const;

	Iterator<false> iterator();
	ConstIterator<false> constIterator() const;
	Iterator<true> reverseIterator();
	ConstIterator<true> constReverseIterator() const;

	Array& operator=(const Array& other); // Copy
	Array& operator=(Array&& other); // Move
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
inline Array<T>::Array() {
	this->items = nullptr;
	this->size = 0;
	this->capacity = defaultCapacity;
}

template<typename T>
inline Array<T>::Array(int capacity) {
	this->size = 0;

	if (capacity == 0)
		this->capacity = defaultCapacity;
	else
		this->capacity = capacity;

	items = allocate(capacity);
}

// Copy
template<typename T>
inline Array<T>::Array(const Array& other) {
	deepCopy(const_cast<Array&>(other));
}

// Move
template<typename T>
inline Array<T>::Array(Array&& other) {
	size = other.size;
	capacity = other.capacity;
	items = other.items;

	other.size = 0;
	other.capacity = 0;
	other.items = nullptr;
}

template<typename T>
inline Array<T>::~Array() {
	destruct();
}


template<typename T>
inline int Array<T>::insert(const T& value) {
	if (size == capacity)
		resize();
	else if (!items)
		items = allocate(capacity);

	new(items + size) T(value);
	size++;

	return size - 1;
}

template<typename T>
inline int Array<T>::insert(int index, const T& value) {
	if (size == capacity)
		resize();
	if (!items)
		items = allocate(capacity);
	if (index < 0 || index >= size)
		return -1;

	new(items + size) T(std::move(items[size - 1]));

	for (int i = size - 1; i > index; i--)
		items[i] = T(std::move(items[i - 1]));

	items[index].~T();
	new(items + index) T(value);
	size++;

	return index;
}

template<typename T>
inline void Array<T>::remove(int index) {
	if (size == 0)
		throw "Array is empty";

	for (int i = index; i < size - 1; i++)
		items[i] = std::move(items[i + 1]);

	items[size - 1].~T();
	size--;
}

template<typename T>
inline int Array<T>::getSize() const {
	return size;
}

template<typename T>
inline int Array<T>::getCapacity() const {
	return capacity;
}


template<typename T>
inline Array<T>::Iterator<false> Array<T>::iterator() {
	return Iterator<false>(*this);
}

template<typename T>
inline Array<T>::ConstIterator<false> Array<T>::constIterator() const {
	return ConstIterator<false>(*this);
}

template<typename T>
inline Array<T>::Iterator<true> Array<T>::reverseIterator() {
	return Iterator<true>(*this);
}

template<typename T>
inline Array<T>::ConstIterator<true> Array<T>::constReverseIterator() const {
	return ConstIterator<true>(*this);
}

// Copy
template<typename T>
inline Array<T>& Array<T>::operator=(const Array& other) {
	if (this != &other) {
		destruct();
		deepCopy(other);
	}
	return *this;
}

// Move
template<typename T>
inline Array<T>& Array<T>::operator=(Array&& other) {
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
inline const T& Array<T>::operator[](int index) const {
	return items[index];
}

template<typename T>
inline T& Array<T>::operator[](int index) {
	return items[index];
}


template<typename T>
inline void Array<T>::resize() {
	capacity *= kResizeCoef;
	T* temp = allocate(capacity);

	for (int i = 0; i < size; i++)
		new(temp + i) T(std::move(items[i]));

	std::free(items);
	items = temp;
}

template<typename T>
inline void Array<T>::deepCopy(const Array& other) {
	size = other.size;
	capacity = other.capacity;
	items = allocate(capacity);

	for (int i = 0; i < other.getSize(); i++)
		new(items + i) T(other[i]);
}

template<typename T>
inline T* Array<T>::allocate(size_t capacity) {
	return static_cast<T*>(std::malloc(capacity * sizeof(T)));
}

template<typename T>
inline void Array<T>::destruct() {
	for (int i = 0; i < size; i++)
		items[i].~T();

	std::free(items);
}