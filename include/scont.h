#ifndef SCONT_H
#define SCONT_H

#include <iostream>
#include <cstring>
#include "common.h"
using namespace std;

struct Entry
{
	unsigned house_id, hh_id, plug_id;
	float val;

	Entry(unsigned hid=0, unsigned hhid=0, unsigned pid=0, float v=-1)
	: house_id(hid), hh_id(hhid), plug_id(pid), val(v) {}
};
typedef struct Entry Entry;

class SCont
{
	unsigned size;
	Entry **data;
	int binarySearch(int first, int last, float val);

  public:
	SCont();
	~SCont();
	void insert(unsigned house_id, unsigned hh_id, unsigned plug_id, float mvalue, float old_val);
	int getNumOfLargeNum(float threshold);
};

#endif
