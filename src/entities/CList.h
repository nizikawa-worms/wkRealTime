

#ifndef WKREALTIME_CLIST_H
#define WKREALTIME_CLIST_H

#include <iostream>
#include <vector>
#include <algorithm>
#include <iterator>
#include <cassert>



template <typename T>
class CList {
public:
	typedef int size_type;

	class iterator {
	public:
		typedef iterator self_type;
		typedef T value_type;
		typedef T& reference;
		typedef T* pointer;
		typedef std::forward_iterator_tag iterator_category;
		typedef int difference_type;
		iterator(pointer ptr) : ptr_(ptr) { }
		self_type operator++() { ptr_++; return *this; }
		self_type operator++(int junk) { self_type i = *this; ptr_++; return i; }
		value_type& operator*() { return *ptr_; }
		pointer operator->() { return ptr_; }
		bool operator==(const self_type& rhs) { return ptr_ == rhs.ptr_; }
		bool operator!=(const self_type& rhs) { return ptr_ != rhs.ptr_; }
	private:
		pointer ptr_;
	};

	class const_iterator {
	public:
		typedef const_iterator self_type;
		typedef T value_type;
		typedef T& reference;
		typedef T* pointer;
		typedef int difference_type;
		typedef std::forward_iterator_tag iterator_category;
		const_iterator(pointer ptr) : ptr_(ptr) { }
		self_type operator++() { ptr_++; return *this; }
		self_type operator++(int junk) { self_type i = *this; ptr_++; return i; }
		const value_type& operator*() { return *ptr_; }
		const pointer operator->() { return ptr_; }
		bool operator==(const self_type& rhs) { return ptr_ == rhs.ptr_; }
		bool operator!=(const self_type& rhs) { return ptr_ != rhs.ptr_; }
	private:
		pointer ptr_;
	};


	CList(size_type size) : size_(size) {
		data_ = new T[size_];
	}

	CList(const CList& source)  {
		//this isn't a copy, but it shouldn't matter
		data_ = source.data_;
		size_ = source.size_;
	}


	size_type size() const {
		return size_;
	}

	size_type max_size() {
		return max_size_;
	}

	T& operator[](size_type index) {
		assert(index < size_);
		return data_[index];
	}

	const T& operator[](size_type index) const {
		assert(index < size_);
		return data_[index];
	}

	iterator begin() {
		return iterator(data_);
	}

	iterator end() {
		return iterator(data_ + size_);
	}

	const_iterator begin() const {
		return const_iterator(data_);
	}

	const_iterator end() const {
		return const_iterator(data_ + size_);
	}

	const_iterator cbegin() const {
		return const_iterator(data_);
	}

	const_iterator cend() const {
		return const_iterator(data_ + size_);
	}

	bool empty() {
		return size_ == 0;
	}


private:
	size_type max_size_;
	size_type unk4_;
	size_type size_;
	T* data_;
	uintptr_t hash_list_;
};


#endif //WKREALTIME_CLIST_H
