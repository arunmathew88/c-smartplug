#ifndef COMMON_H
#define COMMON_H

// input struct
struct measurement
{
	float value;
	unsigned char property;
	unsigned timestamp, plug_id, household_id, house_id;

	measurement(unsigned t=-1, float v=-1, unsigned char p=0, unsigned i=0, unsigned hh=0, unsigned h=0) :
		value(v), property(p), timestamp(t), plug_id(i), household_id(h), house_id(h) {}
};
typedef struct measurement measurement;

#endif
