#pragma once

namespace expos {

	template<typename T>
	struct PoolElement {
		T element;
		PoolElement *next;
	};

	template<typename T>
	class PoolLinkedList {
	private:
		PoolElement<T> *pool;
		PoolElement<T> *first, *open;
		size_t size_;
	public:
		PoolLinkedList(size_t size);
		~PoolLinkedList();

		T *getOpen();
		void add(T*);

		T* get();
		void erase(T*);

		size_t size() {
			return size_;
		}
	};

}