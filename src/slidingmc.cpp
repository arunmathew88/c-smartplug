#include "slidingmc.h"

SlidingMc::SlidingMc(unsigned ts)
{
	bins = new Bin*[NUM_WINDOWS];
	last_ts = new unsigned[NUM_WINDOWS];
	for(int i=0; i<NUM_WINDOWS; i++)
	{
		size[i] = 0;
		num_bins[i] = 0;
		bins[i] = new Bin[MAX_BINS+1];
		bins[i] = &(bins[i][1]);
		last_ts[i] = ts-1;
	}

	data = new float[MAX_WINDOW_SIZE];
	queue_index = 0;
	queue_ts = ts-1;
}

float SlidingMc::findMedian(Window ws, int mindex)
{
	int index=-1, cum=0;

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

int SlidingMc::binarySearch(Window ws, float val)
{
	if(num_bins[ws] == 0)
		return 0;

	if(val < bins[ws][0].val)
		return -1;

	return binarySearch(ws, 0, num_bins[ws], val);
}

int SlidingMc::binarySearch(Window ws, int first, int last, float val)
{
	int mid = (last + first)/2;
	if(mid == first)
		return first;
	else
	{
		if(val == bins[ws][mid].val)
			return mid;
		else if(val > bins[ws][mid].val)
			return binarySearch(ws, mid+1, last, val);
		else
			return binarySearch(ws, first, mid, val);
	}
}

void SlidingMc::insToMc(Window ws, float val)
{
	int pos = binarySearch(ws, val);
	size[ws]++;
	bool flag = false;

	if(pos == -1)
	{
		bins[ws] = bins[ws] - 1;
		bins[ws][0] = Bin(val, 1);
		flag = true;
	} else if(pos > num_bins[ws])
	{
		cout<<"error occured: binarySearch is wrong!"<<endl;
		exit(-1);
	} else if(pos == num_bins[ws])
	{
		bins[ws][pos] = Bin(val, 1);
		num_bins[ws]++;
	} else
	{
		if(bins[ws][pos].val == val || MAX_BINS < num_bins[ws])
			bins[ws][pos].freq++;
		else
			addNewBin(ws, pos, Bin(val, 1));
	}

	// merge bins
	if(num_bins[ws] == MAX_BINS)
	{
		int min = bins[ws][0].freq + bins[ws][1].freq, sum, index=0;
		for(int i=1; i<MAX_BINS-1; i++)
		{
			sum = bins[ws][i].freq + bins[ws][i+1].freq;
			if(min > sum)
			{
				min = sum;
				index = i;
			}
		}

		if(flag)
		{
			memmove(&(bins[ws][1]), &(bins[ws][0]), sizeof(Bin)*(index+1));
			bins[ws][index+1].freq = min;
			bins[ws] = bins[ws] + 1;
		} else
		{
			bins[ws][index].freq = min;
			memmove(&(bins[ws][index+1]), &(bins[ws][index+2]), sizeof(Bin)*(num_bins[ws]-index-2));
		}
		num_bins[ws]--;
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

void SlidingMc::setValueForTs(unsigned ts, float val)
{
	if(ts < queue_ts)
	{
		cout<<"already inserted into the qeue!"<<endl;
		exit(-1);
	}

	queue_ts++;
	while(ts > queue_ts)
	{
		data[queue_index] = -1;
    	queue_index++;
    	if(queue_index == MAX_WINDOW_SIZE)
			queue_index = 0;
		queue_ts++;
	}

	data[queue_index] = val;
    queue_index++;
    if(queue_index == MAX_WINDOW_SIZE)
		queue_index = 0;
}

void SlidingMc::removeValueFromMc(Window ws, float val)
{
	int pos = binarySearch(ws, val);
	bins[ws][pos].freq--;
	size[ws]--;
	if(bins[ws][pos].freq == 0)
	{
		memmove(&(bins[ws][pos]), &(bins[ws][pos+1]), sizeof(Bin)*(num_bins[ws]-pos));
		num_bins[ws]--;
	}
}

void SlidingMc::alignWindow(unsigned ts)
{
	for(int i=0; i<NUM_WINDOWS; i++)
	{
		while(last_ts[i] + WindowSize[i] <= ts)
		{
			float temp = getValueForTs(last_ts[i]);
			if(temp != -1)
				removeValueFromMc((Window)i, temp);

			last_ts[i]++;
		}
	}
}

void SlidingMc::addNewBin(Window ws, int pos, Bin b)
{
	memmove(&(bins[ws][pos+1]), &(bins[ws][pos]), sizeof(Bin)*(num_bins[ws]-pos+1));
	bins[ws][pos] = b;
	num_bins[ws]++;
}

float SlidingMc::getMedian(unsigned int ts, Window ws)
{
	alignWindow(ts);

	if(size[ws] == 0)
		return -1;
	else
		return findMedian(ws, (size[ws]+1)/2);
}

void SlidingMc::insert(unsigned ts, float val)
{
	alignWindow(ts);
	setValueForTs(ts, val);

	for(int i=0; i<NUM_WINDOWS; i++)
		insToMc((Window)i, val);
}

SlidingMc::~SlidingMc()
{
	for(int i=0; i<NUM_WINDOWS; i++)
		delete (bins[i]-1);

	delete bins;
	delete last_ts;
	delete data;
}
