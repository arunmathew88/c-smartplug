#include <iostream>
#include "slidingmc.h"
using namespace std;

int binarySearchHelper(float *array, int first, int last, float val);
int binarySearch(float* array, int length, float val);

int binarySearch(float* array, int length, float val)
{
	if(length == 0)
		return -1;

	if(val < array[0])
		return -1;

	if(val > array[length-1])
		return length;

	return binarySearchHelper(array, 0, length, val);
}

int binarySearchHelper(float *array, int first, int last, float val)
{
	int mid = (last + first)/2;
	if(mid == first)
		return first;
	else
	{
		if(val == array[mid])
			return mid;
		else if(val > array[mid])
			return binarySearchHelper(array, mid, last, val);
		else
			return binarySearchHelper(array, first, mid, val);
	}
}

int main()
{
	float *array = new float[100];
	for (int count = 0; count < 100; ++count)
		array[count] = count*2;

	cout<<binarySearch(array, 100, 199)<<endl;
	cout<<binarySearch(array, 100, 198)<<endl;
	cout<<binarySearch(array, 100, 197)<<endl;
	cout<<binarySearch(array, 50, 33)<<endl;
	cout<<binarySearch(array, 30, 4)<<endl;
	cout<<binarySearch(array, 100, 40)<<endl;

	delete[] array;

    // SlidingMc *sm = new SlidingMc(1);
    // for(int i=1; i<4000; i++)
    //     sm->insert(i, i);
    // cout<<sm->getMedian(4000, WINDOW_1HR)<<endl;
    // delete sm;
}
