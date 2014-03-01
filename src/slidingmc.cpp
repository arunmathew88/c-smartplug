#include "slidingmc.h"

SlidingMc::SlidingMc()
{
	size = 0;
	num_bins = 0;
	bins = new Bin[MAX_BINS+2];
	bins = &(bins[1]);
}

float SlidingMc::findMedian(int mindex)
{
	int index=-1, cum=0;

	while(cum < mindex)
	{
		index++;
		cum += bins[index].freq;
	}

	if(index == num_bins-1)
		return bins[index].val;
	else
		return (bins[index].val + bins[index+1].val)/2;
}

int SlidingMc::binarySearch(float val)
{
	if(num_bins == 0)
		return -1;

	if(val < bins[0].val)
		return -1;

	if(val > bins[num_bins-1].val)
		return num_bins;

	return binarySearch(0, num_bins, val);
}

int SlidingMc::binarySearch(int first, int last, float val)
{
	int mid = (last + first)/2;
	if(mid == first)
		return first;
	else
	{
		if(val == bins[mid].val)
			return mid;
		else if(val > bins[mid].val)
			return binarySearch(mid, last, val);
		else
			return binarySearch(first, mid, val);
	}
}

void SlidingMc::addNewBin(int pos, Bin b)
{
	if(pos+1 >= num_bins)
	{
		cout<<"should not reach at "<<__LINE__<<"in file: "<<__FILE__<<endl;
		exit(-1);
	}
	memmove(&(bins[pos+2]), &(bins[pos+1]), sizeof(Bin)*(num_bins-pos-1));
	bins[pos+1] = b;
	num_bins++;
}

void SlidingMc::insert(float val)
{
	int pos = binarySearch(val);
	size++;
	bool flag = false;

	if(pos == -1)
	{
		bins = bins - 1;
		bins[0] = Bin(val, 1);
		num_bins++;
		flag = true;
	} else if(pos > num_bins)
	{
		cout<<"error occured: binarySearch is wrong!"<<endl;
		exit(-1);
	} else if(pos == num_bins)
	{
		bins[pos] = Bin(val, 1);
		num_bins++;
	} else
	{
		if(bins[pos].val == val || MAX_BINS <= num_bins)
			bins[pos].freq++;
		else
			addNewBin(pos, Bin(val, 1));
	}

	// merge bins
	if(num_bins == MAX_BINS+1)
	{
		int min = bins[0].freq + bins[1].freq, sum, index=0;
		for(int i=1; i<MAX_BINS; i++)
		{
			sum = bins[i].freq + bins[i+1].freq;
			if(min > sum)
			{
				min = sum;
				index = i;
			}
		}

		if(flag)
		{
			memmove(&(bins[1]), &(bins[0]), sizeof(Bin)*(index+1));
			bins[index+1].freq = min;
			bins = bins + 1;
		} else
		{
			bins[index].freq = min;
			memmove(&(bins[index+1]), &(bins[index+2]), sizeof(Bin)*(num_bins-index-2));
		}
		num_bins--;
		flag = false;
	}

	if(flag)
	{
		memmove(&(bins[1]), &(bins[0]), sizeof(Bin)*(num_bins));
		bins = bins + 1;
	}
}

void SlidingMc::del(float val)
{
	int pos = binarySearch(val);
	if(pos < 0)
	{
		cout<<"should not reach here!"<<endl;
		exit(-1);
	}

	if(pos == num_bins)
		pos--;
	bins[pos].freq--;
	size--;
	if(bins[pos].freq == 0)
	{
		memmove(&(bins[pos]), &(bins[pos+1]), sizeof(Bin)*(num_bins-pos-1));
		num_bins--;
	}
}

float SlidingMc::getMedian()
{

	if(size == 0)
		return -1;
	else
		return findMedian((size+1)/2);
}

SlidingMc::~SlidingMc()
{
	bins =  bins - 1;
	delete[] bins;
}
