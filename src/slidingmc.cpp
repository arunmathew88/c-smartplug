#include "slidingmc.h"

SlidingMc::SlidingMc(unsigned ts)
{
	bins = new bin*[NUM_WINDOWS];
	last_ts = new unsigned[NUM_WINDOWS];
	for(int i=0; i<NUM_WINDOWS; i++)
	{
		size[i] = 0;
		num_bins[i] = 0;
		bins[i] = new bin[MAX_BINS];
		last_ts[i] = ts-1;
	}

	data = new float[MAX_WINDOW_SIZE];
	queue_index = 0;
	queue_ts = ts-1;
}

float SlidingMc::findMedian(Window ws, float mindex)
{
	int index = 0, cum = 0;

	while(cum < mindex)
	{
		index++;
		cum += bins[ws][index].freq;
	}

	if(index == 0)
		return bins[ws][0].val;
	else
		return (bins[ws][index-1].val + bins[ws][index].val)/2;
}

int SlidingMc::binarySearch(Window ws, int first, int last, float val)
{
	if(num_bins[ws] == 0)
		return 0;

	int mid = (last + first)/2;
	if(mid == first)
		if(val == bins[ws][mid].val)
			return mid;
		else if(val > bins[ws][mid].val)
			return last;
		else
			return first;
	else
		if(val == bins[ws][mid].val)
			return mid;
		else if(val > bins[ws][mid].val)
			return binarySearch(ws, mid, first, val);
		else
			return binarySearch(ws, first, mid, val);
}

void SlidingMc::insToMc(Window ws, float val)
{
	int pos = binarySearch(ws, 0, num_bins[ws], val);
	size[ws]++;

	// @todo merge bins here, if required
	if(pos >= num_bins[ws])
		bins[ws][pos] = bin(val, 1);
	else
	{
		if(bins[ws][pos].val == val || MAX_BINS <= num_bins[ws])
			bins[ws][pos].freq++;
		else
			addNewBin(ws, pos, bin(val, 1));
	}
}

float SlidingMc::getValueForTs(unsigned ts)
{
	if(ts > queue_ts)
	{
		cout<<"asking for a value before even inserting it!"<<endl;
		exit(-1);
	}

	return data[(queue_ts-ts+queue_index-1) % MAX_WINDOW_SIZE];
}

void SlidingMc::removeValueFromMc(Window ws, float val)
{
	int pos = binarySearch(ws, 0, num_bins[ws], val);
	bins[ws][pos].freq++;
}

void SlidingMc::alignWindow(unsigned ts)
{
	for(int i=0; i<NUM_WINDOWS; i++)
	{
		while(last_ts[i] <= (ts - WindowSize[i]))
		{
			float temp = getValueForTs(last_ts[i]);
			if(temp != -1)
				removeValueFromMc((Window)i, temp);

			last_ts[i]++;
		}
	}
}

void SlidingMc::addNewBin(Window ws, int pos, bin b)
{
	memcpy(&(bins[ws][pos+1]), &(bins[ws][pos]), sizeof(bin)*(num_bins[ws]-2-1));
	bins[ws][pos] = b;
	num_bins[ws]++;
}

float SlidingMc::getMedian(unsigned int ts, Window ws)
{
	alignWindow(ts);

	if(size[ws] == 0)
	{
		cout<<"Can't find median unless at least one value is inserted!"<<endl;
		exit(-1);
	} else
		return findMedian(ws, (float)(size[ws]/2));
}

void SlidingMc::insert(unsigned ts, float val)
{
	alignWindow(ts);

	data[queue_index] = val;
    queue_index++;
    if(queue_index == MAX_WINDOW_SIZE)
		queue_index = 0;
	queue_ts = ts;

	for(int i=0; i<NUM_WINDOWS; i++)
		insToMc((Window)i, val);
}

SlidingMc::~SlidingMc()
{
	for(int i=0; i<NUM_WINDOWS; i++)
		delete bins[i];

	delete bins;
	delete last_ts;
	delete data;
}
