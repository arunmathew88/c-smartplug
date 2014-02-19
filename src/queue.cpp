#include "queue.h"

template <class T>
Queue<T>::Queue(int size)
{
	max_size = size;
	data = new T[size];
	index = 0;
}

template <class T>
void Queue<T>::enque(T elem)
{
	data[index] = elem;
	index++;
	if(index == max_size)
		index = 0;
}

template <class T>
Queue<T>::~Queue()
{
	delete data;
}

template class Queue<float>;
