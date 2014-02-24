#include <sys/socket.h>
#include <map>
#include <vector>
#include <tgmath.h>
#include "common.h"
#include "mc.h"
#include "slidingmc.h"

// global
house_state state;
unordered_map<unsigned int, plug_state> household_aggregate;
plug_state house_aggregate;

unordered_map<unsigned int,
	unordered_map<unsigned int,
		unordered_map<unsigned int,
			unordered_map<unsigned int, Mc>
		>
	>
> median_container;
unordered_map<unsigned int,
	unordered_map<unsigned int, Mc>
>house_median_container;

unordered_map<unsigned int,
	unordered_map<unsigned int, SlidingMc>
> sliding_median_container;

SlidingMc global_median_container;
unsigned int house_id = 1;

void doProcessing2(measurement *input)
{
	if(input->property !=1)
		return;

	static float last_percentage[2] = {0};
	float percentage, global_median;
	unsigned int total_plugs = 0, outlier = 0;

	sliding_median_container[input->household_id][input->plug_id].insert(input->timestamp,input->value);
	global_median_container.insert(input->timestamp,input->value);
	global_median = global_median_container.getMedian(input->timestamp, WINDOW_1HR);
	for(auto& household:sliding_median_container)
		for(auto& plug:household.second)
		{
			float median = plug.second.getMedian(input->timestamp, WINDOW_1HR);
			if(median < 0)
				continue;
			total_plugs++;
			if(median > global_median)
				outlier++;
		}

	percentage = (float)outlier * 100 / total_plugs;
	if(fabs(last_percentage[WINDOW_1HR] - percentage) > 0.5)
	{
		printf("SLIDING_WINDOW_1HR %u,%u,%u,%f",input->timestamp - 60*60, input->timestamp,input->house_id, percentage);
		last_percentage[WINDOW_1HR] = input->timestamp;
	}

	total_plugs = 0, outlier = 0;
	global_median = global_median_container.getMedian(input->timestamp, WINDOW_24HR);
	for(auto& household:sliding_median_container)
		for(auto& plug:household.second)
		{
			float median = plug.second.getMedian(input->timestamp, WINDOW_24HR);
			if(median < 0)
				continue;
			total_plugs++;
			if(median > global_median)
				outlier++;
		}

	percentage = (float)outlier * 100 / total_plugs;
	if(fabs(last_percentage[WINDOW_24HR] - percentage) > 0.5)
	{
		printf("SLIDING_WINDOW_24HR %u,%u,%u,%f", input->timestamp - 24*60*60, input->timestamp,input->house_id, percentage);
		last_percentage[WINDOW_24HR] = input->timestamp;
	}
}

void forcastPlugLoad(unsigned int ts, unsigned int hh_id, unsigned int plug_id, float average_load, unsigned int slice)
{
	unsigned int forcast_ts = ts - ts % timeslice_lengths.at(slice) +
			((ts % timeslice_lengths.at(slice))?2:1) * timeslice_lengths.at(slice);
	float median = median_container[hh_id][plug_id][slice][forcast_ts % 86400].getMedian(), forcast;
	if(median < 0)
		forcast = average_load;
	else
		forcast = (average_load + median)/2;
	printf("PLUG_FORCAST_%u_S %u %u,%u,%u,%u,%f\n", timeslice_lengths.at(slice), ts, forcast_ts, house_id, hh_id, plug_id, average_load);
}

void forcastHouseLoad(unsigned int ts, float average_load, unsigned int slice)
{
	unsigned int forcast_ts = ts - ts % timeslice_lengths.at(slice) +
			((ts % timeslice_lengths.at(slice))?2:1) * timeslice_lengths.at(slice);
	float median = house_median_container[slice][forcast_ts % 86400].getMedian(), forcast;
	if(median < 0)
		forcast = average_load;
	else
		forcast = (average_load + median)/2;
	printf("HOUSE_FORCAST_%u_S %u %u,%u,%f\n", timeslice_lengths.at(slice), ts, forcast_ts, house_id, average_load);
}

void processHouse(unsigned int boundary_ts, unsigned int x, float load, bool flush = false)
{
	static unsigned int last_timestamp = boundary_ts;
	if(flush)
	{
		for(auto& slice:timeslice_lengths)
		{
			if(slice.first == TIMESLICE_30S)
				continue;
			forcastHouseLoad(last_timestamp, house_aggregate[slice.first].accumulated_load, slice.first);
			if(last_timestamp % slice.second == 0)
				house_median_container[slice.first][(last_timestamp-slice.second)%86400].insert(house_aggregate[slice.first].accumulated_load);
			house_aggregate[slice.first].accumulated_load = 0;
		}
	} else
	{
		last_timestamp = boundary_ts;
		house_aggregate[x].accumulated_load += load;
	}
}

void processHouseHold(unsigned int boundary_ts, unsigned int x, float load, unsigned int household_id, bool flush = false)
{
	static unsigned int last_timestamp = boundary_ts;
	if(flush || last_timestamp < boundary_ts)
	{
		for(auto& household:household_aggregate)
		{
			for(auto& slice:timeslice_lengths)
			{
				if(slice.first == TIMESLICE_30S)
					continue;
				if(flush)
					processHouse(last_timestamp, slice.first, household.second[slice.first].accumulated_load);
				household.second[slice.first].accumulated_load = 0;
			}
		}
		if(flush)
			processHouse(0,0,0,true);
	}
	if(!flush)
	{
		last_timestamp = boundary_ts;

		if(household_aggregate.find(household_id) == household_aggregate.end())
			household_aggregate[household_id] = initial_plug_state;
		household_aggregate[household_id][x].accumulated_load += load;
	}

/*	cout<<boundary_ts<< " slice " << x<<
			" load " << load<<
			" hhid " << household_id <<
			" agg " <<household_aggregate[household_id][x].accumulated_load <<endl;
*/
}

// for each measurement read do
// float doProcessing(measurement *input) {
void doProcessing(measurement *input)
{
	//ignore if the measurement is a work value
	if(input->property == 0)
		return;

	unsigned int modulo = input->timestamp % timeslice_lengths.at(TIMESLICE_30S);
	static unsigned int last_timestamp = input->timestamp -((modulo == 0)? timeslice_lengths.at(TIMESLICE_30S) : modulo);

	if(state.find(input->household_id) == state.end())
		state[input->household_id] = initial_household_state;
	if(state[input->household_id].find(input->plug_id) == state[input->household_id].end())
	{
		state[input->household_id][input->plug_id] = initial_plug_state;
		for(auto& x:timeslice_lengths)
		{
			state[input->household_id][input->plug_id][x.first].last_timestamp =
					input->timestamp - ((input->timestamp % x.second == 0)? x.second : input->timestamp % x.second);
		}
	}

	unsigned int gap = input->timestamp - last_timestamp;
	if(gap > timeslice_lengths.at(TIMESLICE_30S))
	{
		gap = last_timestamp;
		last_timestamp = input->timestamp - ((modulo == 0)? timeslice_lengths.at(TIMESLICE_30S) : modulo);
		gap = last_timestamp - gap;

		for(auto& household:state)
			for(auto& plug:household.second)
			{
				if(plug.second[TIMESLICE_30S].last_timestamp == last_timestamp)
					continue;
				for(auto& x:plug.second)
				{
					if(x.first == TIMESLICE_30S)
						continue;
					if(last_timestamp - x.second.last_timestamp > timeslice_lengths.at(x.first))
					{
						x.second.accumulated_load = 0;
						x.second.count_of_values = 0;
						x.second.last_timestamp = last_timestamp - last_timestamp % timeslice_lengths.at(x.first);
						continue;
					}
					x.second.accumulated_load += plug.second[TIMESLICE_30S].accumulated_load;
					x.second.count_of_values += plug.second[TIMESLICE_30S].count_of_values;
					float average_load = 0;
					if(x.second.count_of_values)
					{
						average_load = x.second.accumulated_load /  x.second.count_of_values;
						forcastPlugLoad(last_timestamp, household.first, plug.first, average_load, x.first);
					}

					if(last_timestamp - x.second.last_timestamp == timeslice_lengths.at(x.first))
					{
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

	if(input->timestamp - p_state[TIMESLICE_30S].last_timestamp <= timeslice_lengths.at(TIMESLICE_30S))
	{
		p_state[TIMESLICE_30S].accumulated_load += input->value;
		p_state[TIMESLICE_30S].count_of_values++;
	}

	if(input->timestamp - p_state[TIMESLICE_30S].last_timestamp == timeslice_lengths.at(TIMESLICE_30S))
	{
		for(auto& x:timeslice_lengths)
		{
			if(x.first == TIMESLICE_30S)
				continue;
			p_state[x.first].accumulated_load += p_state[TIMESLICE_30S].accumulated_load;
			p_state[x.first].count_of_values += p_state[TIMESLICE_30S].count_of_values;
			float average_load = 0;
			if(p_state[x.first].count_of_values)
			{
				average_load = p_state[x.first].accumulated_load /  p_state[x.first].count_of_values;
				forcastPlugLoad(input->timestamp, input->household_id, input->plug_id, average_load, x.first);
			}

			if(input->timestamp - p_state[x.first].last_timestamp == timeslice_lengths.at(x.first))
			{
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

	if(input->timestamp - p_state[TIMESLICE_30S].last_timestamp > timeslice_lengths.at(TIMESLICE_30S)) {
		p_state[TIMESLICE_30S].accumulated_load = input->value;
		p_state[TIMESLICE_30S].count_of_values = 1;
		p_state[TIMESLICE_30S].last_timestamp = input->timestamp -
				((modulo == 0)? timeslice_lengths.at(TIMESLICE_30S) : modulo);
	}

	//return state[input->household_id][input->plug_id][TIMESLICE_1M].accumulated_load;
}

/*
int main(int argc, char *argv[])
{
  	unordered_map<unsigned int,Mc> cont;
	measurement line = {0,0,0,1,0,0,0};
	float value = 0, expect = 0;
	Mc a;
	cont[1] =a;
	for(unsigned int ts=1377986400; ts < 1377986700; ts++)
	{
		line.timestamp = ts;
		line.id++;
		line.value = value;
		if(!(ts % 60)) expect = 0;
		float ret = -1;
		if((ts)% 30)
			doProcessing(&line);

		cont[1].insert(ts -1377986400);
		if((ts)% 30 == 0)
			cout << ts <<" returned " <<cont[1].getMedian() << " expected " << expect<<endl;
		value += 1;
		expect += value;
	}

	return 0;
}
*/
