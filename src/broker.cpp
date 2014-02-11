#include <iostream>
#include <fstream>
#include <cstdlib>

int main ()
{
  	//Reading config file


    //Read the data stream
    std::ifstream file("priv/temp.csv");
    std::string buffer;
    int house_id;

    /*unordered_map <int, pid_t> m;
    for(int i = 0; i < SIZE; ++i)
    {
        m[std::to_string(i)] = i;
    }
    for(int i = 0; i < (SIZE-1); ++i)
    {
        m[std::to_string(i)] = m[std::to_string(i+1)];
    }*/

    while (std::getline(file, buffer))
    {
    	std::string message;
    	unsigned pos_first = buffer.find_first_of(","); //need to remove sample id
    	unsigned pos_last = buffer.find_last_of(","); //need to get the starting point of the house id
    	message = buffer.substr(pos_first + 1, pos_last - pos_first - 1);
    	house_id = atof(buffer.substr(pos_last + 1).c_str());	//house id string to int
    	std::cout << "house_id = " << house_id << std::endl;
        std::cout << "message = " << message << std::endl;
    	//send message to the house with house id


    }
	return 0;
}