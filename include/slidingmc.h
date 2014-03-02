#ifndef SLIDINGMC_H
#define SLIDINGMC_H

#include <iostream>
#include <cstdlib>
#include <cstring>
using namespace std;

#define MAX_BINS 100

struct Bin
{
	float val;
	int freq;

	Bin(float v=-1, int f=0)
	: val(v), freq(f) {}
};

class SlidingMc
{
	int size;  		// number of elments in the median container for 1 hr & 24hr sliding window
	int num_bins;	// number of bins <= NUM_BINS for 1 hr & 24hr sliding window
	Bin *bins;		// array of bins for 1 hr & 24hr sliding window

	float findMedian(int mindex);
	int binarySearch(float val);
	int binarySearch(int first, int last, float val);
	void addNewBin(int pos, Bin b);
	void insToMc(float val);

  public:
	SlidingMc();
	~SlidingMc();
	float getMedian();
	void insert(float val);
	void del(float val);
};

#endif