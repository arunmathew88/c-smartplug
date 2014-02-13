#ifndef COMMON_H
#define COMMON_H

struct DataPoint
{
	int ts, plug_id, hh_id;
    float val;
    char prop;

    DataPoint(int t, int p, int h, float v, char pr)
    : ts(t), plug_id(p), hh_id(h), val(v), prop(pr) {}

    DataPoint()
    : ts(0), plug_id(0), hh_id(0), val(0.0), prop(0) {}
};
typedef struct DataPoint DP;

#endif
