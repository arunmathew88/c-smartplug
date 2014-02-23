#ifndef SLIDINGMC_H
#define SLIDINGMC_H

#include <iostream>
#include <cstdlib>
#include <cstring>
using namespace std;

#define MAX_WINDOW_SIZE 24*4000
#define MAX_BINS 10000

enum Window
{
 	WINDOW_1HR = 0,
	WINDOW_24HR,
	NUM_WINDOWS
};

struct Bin
{
	float val;
	int freq;

	Bin(float v=-1, int f=0)
	: val(v), freq(f) {}
};

class SlidingMc
{
	int size[NUM_WINDOWS];  		// number of elments in the median container for 1 hr & 24hr sliding window
	int num_bins[NUM_WINDOWS];		// number of bins <= NUM_BINS for 1 hr & 24hr sliding window
	Bin **bins;						// array of bins for 1 hr & 24hr sliding window
	unsigned *last_ts;				// last timestamp removed from the container

	// queue structure
	float *data;
	int queue_index;
	unsigned queue_ts;

	float findMedian(Window ws, int mindex);
	int binarySearch(Window ws, float val);
	int binarySearch(Window ws, int first, int last, float val);
	void addNewBin(Window ws, int pos, Bin b);
	void insToMc(Window ws, float val);
	float getValueForTs(unsigned ts);
	void setValueForTs(unsigned ts, float val);
	void removeValueFromMc(Window ws, float val);
	void alignWindow(unsigned ts);

  public:
	SlidingMc(unsigned ts);
	~SlidingMc();
	float getMedian(unsigned ts, Window ws);
	void insert(unsigned ts, float val);
};

#endif
