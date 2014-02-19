#ifndef SLIDINGMC_H
#define SLIDINGMC_H

#include <iostream>
#include <cstdlib>
#include "queue.h"
using namespace std;

#define WINDOW_SIZE 24*3600
#define NUM_BINS 100

class SlidingMc
{
	int size;
	int array[NUM_BINS];

  public:
	SlidingMc();
	~SlidingMc();
	float getMedian();
	void insert(float elem);
};
