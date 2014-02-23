#include <iostream>
#include <cstring>
#include <cstdio>
#include <map>
#include <vector>
#include <tgmath.h>
#include "zhelpers.hpp"
#include "common.h"
#include "mc.h"
using namespace std;

#define NUM_THREADS 10


//global
house_state state;
std::unordered_map<unsigned int, plug_state> household_aggregate;
plug_state house_aggregate;

std::unordered_map<unsigned int,
	std::unordered_map<unsigned int,
		std::unordered_map<unsigned int,
			std::unordered_map<unsigned int, Mc>
		>
	>
> median_container;

std::unordered_map<unsigned int,
	std::unordered_map<unsigned int, Mc>
>house_median_container;


unsigned int house_id;


void forcastPlugLoad(unsigned int ts, unsigned int hh_id, unsigned int plug_id, float average_load, unsigned int slice) {
	unsigned int forcast_ts = ts - ts % timeslice_lengths.at(slice) +
			((ts % timeslice_lengths.at(slice))?2:1) * timeslice_lengths.at(slice);
	float median = median_container[hh_id][plug_id][slice][forcast_ts % 86400].getMedian(), forcast;
	if (median < 0)
		forcast = average_load;
	else
		forcast = (average_load + median)/2;
	printf("PLUG_FORECAST_%u_S %u %u,%u,%u,%u,%f\n", timeslice_lengths.at(slice), ts, forcast_ts, house_id, hh_id, plug_id, forcast);
}

void forcastHouseLoad(unsigned int ts, float average_load, unsigned int slice) {
	unsigned int forcast_ts = ts - ts % timeslice_lengths.at(slice) +
			((ts % timeslice_lengths.at(slice))?2:1) * timeslice_lengths.at(slice);
	float median = house_median_container[slice][forcast_ts % 86400].getMedian(), forcast;
	if (median < 0)
		forcast = average_load;
	else
		forcast = (average_load + median)/2;
	printf("HOUSE_FORECAST_%u_S %u %u,%u,%f\n", timeslice_lengths.at(slice), ts, forcast_ts, house_id, forcast);
}

void processHouse(unsigned int boundary_ts, unsigned int x, float load, bool flush = false) {

	static unsigned int last_timestamp = boundary_ts;
	if (flush) {
		for (auto& slice:timeslice_lengths) {
			if (slice.first == TIMESLICE_30S) continue;
			forcastHouseLoad(last_timestamp, house_aggregate[slice.first].accumulated_load, slice.first);
			if (last_timestamp % slice.second == 0)
				house_median_container[slice.first][(last_timestamp-slice.second)%86400].insert(house_aggregate[slice.first].accumulated_load);
			house_aggregate[slice.first].accumulated_load = 0;
		}
	} else {
		last_timestamp = boundary_ts;
		house_aggregate[x].accumulated_load += load;
	}
}

void processHouseHold(unsigned int boundary_ts, unsigned int x, float load, unsigned int household_id, bool flush = false) {

	static unsigned int last_timestamp = boundary_ts;
	if (flush || last_timestamp < boundary_ts) {
		for (auto& household:household_aggregate) {
			for (auto& slice:timeslice_lengths) {
				if (slice.first == TIMESLICE_30S) continue;
				if (flush)
					processHouse(last_timestamp, slice.first, household.second[slice.first].accumulated_load);
				household.second[slice.first].accumulated_load = 0;
			}
		}
		if (flush)
			processHouse(0,0,0,true);
	}
	if (!flush) {
		last_timestamp = boundary_ts;

		if (household_aggregate.find(household_id) == household_aggregate.end())
			household_aggregate[household_id] = initial_plug_state;
		household_aggregate[household_id][x].accumulated_load += load;
	}
	//for testing
/*	cout<<boundary_ts<< " slice " << x<<
			" load " << load<<
			" hhid " << household_id <<
			" agg " <<household_aggregate[household_id][x].accumulated_load <<endl;
*/
}

//for each measurement read do
//float doProcessing(measurement *input) {	//for testing
void doProcessing(measurement *input) {

	if (input->property == 0) //ignore if the measurement is a work value
		return ;
//		return 0; //for testing

	unsigned int modulo = input->timestamp % timeslice_lengths.at(TIMESLICE_30S);
	static unsigned int last_timestamp = input->timestamp -((modulo == 0)? timeslice_lengths.at(TIMESLICE_30S) : modulo);

	if ( state.find(input->household_id) == state.end() )
		state[input->household_id] = initial_household_state;
	if ( state[input->household_id].find(input->plug_id) ==
			state[input->household_id].end() ) {
		state[input->household_id][input->plug_id] = initial_plug_state;
		for (auto& x:timeslice_lengths) {
			state[input->household_id][input->plug_id][x.first].last_timestamp =
					input->timestamp -
					((input->timestamp % x.second == 0)? x.second : input->timestamp % x.second);
		}
	}

	unsigned int gap = input->timestamp - last_timestamp;
	if (gap > timeslice_lengths.at(TIMESLICE_30S)) {

		gap = last_timestamp;
		last_timestamp = input->timestamp -
				((modulo == 0)? timeslice_lengths.at(TIMESLICE_30S) : modulo);
		gap = last_timestamp - gap;

		for (auto& household:state)
		for (auto& plug:household.second) {
			if (plug.second[TIMESLICE_30S].last_timestamp == last_timestamp)
				continue;
			for (auto& x:plug.second) {
				if (x.first == TIMESLICE_30S)
					continue;
				if (last_timestamp - x.second.last_timestamp > timeslice_lengths.at(x.first)) {
					x.second.accumulated_load = 0;
					x.second.count_of_values = 0;
					x.second.last_timestamp = last_timestamp - last_timestamp % timeslice_lengths.at(x.first);
					continue;
				}
				x.second.accumulated_load += plug.second[TIMESLICE_30S].accumulated_load;
				x.second.count_of_values += plug.second[TIMESLICE_30S].count_of_values;
				float average_load = 0;
				if (x.second.count_of_values) {
					average_load = x.second.accumulated_load /  x.second.count_of_values;
					forcastPlugLoad(last_timestamp, household.first, plug.first, average_load, x.first);
				}

				if (last_timestamp - x.second.last_timestamp == timeslice_lengths.at(x.first)) {
					median_container[household.first][plug.first][x.first][x.second.last_timestamp % 86400].insert(average_load);
					x.second.accumulated_load = 0;
					x.second.count_of_values = 0;
					x.second.last_timestamp = last_timestamp;
				}

				processHouseHold(last_timestamp, x.first, average_load, household.first);
			}
			plug.second[TIMESLICE_30S].accumulated_load = 0;
			plug.second[TIMESLICE_30S].count_of_values = 0;
			plug.second[TIMESLICE_30S].last_timestamp = last_timestamp;

		}
		processHouseHold(0,0,0,0,true);
	}

	plug_state &p_state = state[input->household_id][input->plug_id];

	if (input->timestamp - p_state[TIMESLICE_30S].last_timestamp <= timeslice_lengths.at(TIMESLICE_30S)) {
		p_state[TIMESLICE_30S].accumulated_load += input->value;
		p_state[TIMESLICE_30S].count_of_values++;
	}

	if (input->timestamp - p_state[TIMESLICE_30S].last_timestamp == timeslice_lengths.at(TIMESLICE_30S)) {
		for (auto& x:timeslice_lengths) {
			if (x.first == TIMESLICE_30S)
				continue;
			p_state[x.first].accumulated_load += p_state[TIMESLICE_30S].accumulated_load;
			p_state[x.first].count_of_values += p_state[TIMESLICE_30S].count_of_values;
			float average_load = 0;
			if (p_state[x.first].count_of_values) {
				average_load = p_state[x.first].accumulated_load /  p_state[x.first].count_of_values;
				forcastPlugLoad(input->timestamp, input->household_id, input->plug_id, average_load, x.first);
			}

			if (input->timestamp - p_state[x.first].last_timestamp == timeslice_lengths.at(x.first)) {
				median_container[input->household_id][input->plug_id][x.first][p_state[x.first].last_timestamp % 86400].insert(average_load);
				p_state[x.first].accumulated_load = 0;
				p_state[x.first].count_of_values = 0;
				p_state[x.first].last_timestamp = input->timestamp;
			}
			processHouseHold(input->timestamp, x.first, average_load, input->household_id);
		}
		p_state[TIMESLICE_30S].accumulated_load = 0;
		p_state[TIMESLICE_30S].count_of_values = 0;
		p_state[TIMESLICE_30S].last_timestamp = input->timestamp;
	}

	if (input->timestamp - p_state[TIMESLICE_30S].last_timestamp > timeslice_lengths.at(TIMESLICE_30S)) {
		p_state[TIMESLICE_30S].accumulated_load = input->value;
		p_state[TIMESLICE_30S].count_of_values = 1;
		p_state[TIMESLICE_30S].last_timestamp = input->timestamp -
				((modulo == 0)? timeslice_lengths.at(TIMESLICE_30S) : modulo);
	}
}


// arg: house_id broker_ip sync_port self_ip data_port
int main(int argc, char const *argv[])
{
	string house_id_str, broker_ip, sync_port, self_ip, data_port;

	if(argc < 6)
	{
		cout<<"not enough arguments!"<<endl;
		exit(-1);
	} else
	{
		house_id_str  = string(argv[1]);
		house_id = atoi(house_id_str.c_str());
		broker_ip = string(argv[2]);
		sync_port = string(argv[3]);
		self_ip   = string(argv[4]);
		data_port = string(argv[5]);
	}
	cout<<"house running with id :"<<house_id_str<<endl;

	// creating a zmq context
	zmq::context_t context(NUM_THREADS);

	// listen on a pull socket
    zmq::socket_t subscriber(context, ZMQ_PULL);
    subscriber.bind((string("tcp://*:") + data_port).c_str());

    // synchronize with publisher
    zmq::socket_t syncclient(context, ZMQ_REQ);
    syncclient.connect((string("tcp://") + broker_ip + string(":") + sync_port).c_str());

    // send a synchronization request
    s_sendmore(syncclient, house_id_str);
    s_send(syncclient, self_ip+string(":")+data_port);

    // wait for synchronization reply
    s_recv(syncclient);

    measurement *m = new measurement();
    while(true) {
        // read message contents
        zmq_recv(subscriber, m, sizeof(measurement), 0);
        doProcessing(m);
    }
    delete m;

    return 0;
}
