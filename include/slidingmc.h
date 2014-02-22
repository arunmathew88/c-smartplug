#ifndef SLIDINGMC_H
#define SLIDINGMC_H

#include <iostream>
#include <cstdlib>
#include "queue.h"
using namespace std;

#define WINDOW_SIZE 24*3600
#define NUM_BINS 100

struct bin
{
	float val;
	int freq;

	bin(float v=-1, int f=0)
	: val(v), freq(f) {}
};

class SlidingMc
{
	int size;
	int num_bins;
	bin *bins;
	Queue<float> *queue;
	unsigned base_ts;

	float findMedian(int index, int cum, int mindex);
	int binarySearch(int first, int last, float val);
	void addNewBin(int pos, bin b);

  public:
	SlidingMc(unsigned int ts);
	~SlidingMc();
	void init(unsigned int ts, int size);
	float getMedian(unsigned int ts);
	void insert(unsigned int ts, float val);
};

#endif
