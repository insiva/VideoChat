/*
 * XPoll.h
 *
 *  Created on: 2016年7月5日
 *      Author: zzh
 */

#ifndef INCLUDE_XLIB_XPOOL_H_
#define INCLUDE_XLIB_XPOOL_H_

#include<xlib/Config.h>
#include<pthread.h>
#include<string.h>

using namespace std;

#define STACK_CAPACITY 64

template<typename C> class PoolStack {
public:
	PoolStack() :
			iSize(0), iCapacity(STACK_CAPACITY) {
		this->pStack = new ptr[this->iCapacity];
		memset(this->pStack, 0, this->iCapacity * sizeof(ptr));
	}

	~PoolStack() {
		for (int i = 0; i < this->iSize; i++) {
			C *c = (C *) this->pStack[i];
			delete c;
		}
		delete[] this->pStack;
	}

	C *pop() {
		C *c = XNULL;
		if (this->iSize > 0) {
			c = (C *) this->pStack[--iSize];
			this->pStack[iSize] = XNULL;
		}
		return c;
	}

	void push(C *c) {
		while (this->iSize >= this->iCapacity) {
			this->expandCapacity();
		}
		this->pStack[iSize++] = (ptr) c;
	}

	uint size() const {
		return this->iSize;
	}
private:
	ptr *pStack;
	int iSize, iCapacity;
	void expandCapacity() {
		int capacity = this->iCapacity * 2;
		ptr *newStack = new ptr[capacity];
		memset(newStack, 0, capacity * sizeof(ptr));
		memcpy(newStack, this->pStack, this->iCapacity * sizeof(ptr));
		delete[] this->pStack;
		this->pStack = newStack;
	}
};

template<class C> class XPool {
public:
	XPool() {
		this->pPool = new PoolStack<C>();
		pthread_mutex_init(&mLock, XNULL);
	}
	~XPool() {
		//DLOG("Pool Delete\n");
		delete this->pPool;
		pthread_mutex_destroy(&mLock);
	}
	C *obtain() {
		C *c = XNULL;
		pthread_mutex_lock(&mLock);
		if (this->pPool->size() > 0) {
			c = this->pPool->pop();
		}
		pthread_mutex_unlock(&mLock);
		if (c == XNULL) {
			c = new C();
		}
		return c;
	}
	void recycle(C * c) {
		pthread_mutex_lock(&mLock);
		this->pPool->push(c);
		pthread_mutex_unlock(&mLock);
	}
private:
	PoolStack<C> *pPool;
	pthread_mutex_t mLock;
};

#endif /* INCLUDE_XLIB_XPOOL_H_ */
