#include <iostream>
#include <string>
#include <tuple>
#include <unordered_map>
using namespace std;

#define SIZE 2000000

int main()
{
    unordered_map <string, int> m;
    for(int i = 0; i < SIZE; ++i)
    {
    	m[std::to_string(i)] = i;
    }
    for(int i = 0; i < (SIZE-1); ++i)
    {
    	m[std::to_string(i)] = m[std::to_string(i+1)];
    }
}
