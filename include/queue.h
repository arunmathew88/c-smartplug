#ifndef QUEUE_H
#define QUEUE_H

#include <iostream>
using namespace std;

template <class T>
class Queue
{
	T *data;
	int index;
	int max_size;

  public:
	Queue(int size);
	~Queue();
	void enque(T elem);
};

#endif
