#include "scont.h"

SCont::SCont()
{
	size = 0;
	data = new Entry*[NUM_PLUGS];
	for(int i=0; i<NUM_PLUGS; i++)
		data[i] = new Entry();
}

int SCont::binarySearch(int first, int last, float val)
{
	int mid = (last + first)/2;
	if(mid == first)
		return first;
	else
	{
		if(val == data[mid]->val)
			return mid;
		else if(val > data[mid]->val)
			return binarySearch(mid, last, val);
		else
			return binarySearch(first, mid, val);
	}
}

void SCont::insert(unsigned house_id, unsigned hh_id, unsigned plug_id, float mvalue, float old_val)
{
	if(size == 0)
	{
		data[0]->house_id = house_id;
		data[0]->hh_id = hh_id;
		data[0]->plug_id = plug_id;
		data[0]->val = mvalue;
		size++;
	} else
	{
		if(old_val != -1)
		{
			int pos = binarySearch(0, size, old_val);
			int new_pos = binarySearch(0, size, mvalue);

			// safety check
			if(size >= NUM_PLUGS && ((data[pos]->plug_id != plug_id) || (data[pos]->house_id != house_id) || (data[pos]->hh_id != hh_id)))
			{
				cout<<"should not reach on line: "<<__LINE__<<" in file: "<<__FILE__<<endl;
				exit(-1);
			}

			if(pos == new_pos)
				data[pos]->val = mvalue;
			else if(new_pos > pos)
			{
				memmove(&(data[pos]), &(data[pos+1]), sizeof(Entry*)*(new_pos-pos));
				data[new_pos]->house_id = house_id;
				data[new_pos]->hh_id = hh_id;
				data[new_pos]->plug_id = plug_id;
				data[new_pos]->val = mvalue;
			} else
			{
				memmove(&(data[new_pos+1]), &(data[new_pos]), sizeof(Entry*)*(pos-new_pos));
				data[new_pos]->house_id = house_id;
				data[new_pos]->hh_id = hh_id;
				data[new_pos]->plug_id = plug_id;
				data[new_pos]->val = mvalue;
			}
		} else if(size < NUM_PLUGS)
		{
			int new_pos = binarySearch(0, size, mvalue);

			memmove(&(data[new_pos+1]), &(data[new_pos]), sizeof(Entry*)*(size-new_pos));
			data[new_pos]->house_id = house_id;
			data[new_pos]->hh_id = hh_id;
			data[new_pos]->plug_id = plug_id;
			data[new_pos]->val = mvalue;
			size++;
		} else
		{
			cout<<"should not reach on line: "<<__LINE__<<" in file: "<<__FILE__<<endl;
			exit(-1);
		}
	}
}

int SCont::getNumOfLargeNum(float threshold)
{
	if(size == 0)
		return 0;

	if(threshold < data[0]->val)
		return NUM_PLUGS;

	if(threshold >= data[NUM_PLUGS-1]->val)
		return 0;

	int pos = binarySearch(0, size, threshold);

	return (NUM_PLUGS - pos - 1);
}

SCont::~SCont()
{
	for(int i=0; i<NUM_PLUGS; i++)
		delete data[i];
	delete[] data;
}
