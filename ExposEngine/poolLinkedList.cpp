#include "stdafx.h"
#include "poolLinkedList.h"

namespace expos {
	template<typename T>
	PoolLinkedList<T>::PoolLinkedList(size_t size) : pool(new PoolElement<T>[size]) {

	}

	template<typename T>
	PoolLinkedList<T>::~PoolLinkedList() {
		delete[] pool;
	}

	template<typename T>
	T *PoolLinkedList<T>::getOpen() {
		T *openEl = this->open->element;
		this->open = this->open->next;
		return openEl;
	}

	template<typename T>
	void PoolLinkedList<T>::add(T *element) {
		T *oldFirst = this->first->element;
		this->first
		element->
	}

	T* PoolLinkedList::get();
	void PoolLinkedList::erase(T*);
}