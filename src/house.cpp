#include <iostream>
#include "mc.h"
using namespace std;

int main()
{
	Mc mc;
	for(int i=1000; i>0; i--)
	{
		mc.insert(i);
	}

	cout<<mc.getMedian()<<endl;
}
