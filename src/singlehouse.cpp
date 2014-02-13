#include <sys/socket.h>
#include <map>
#include <vector>
#include <common.h>


//global
house_state state;
std::unordered_map<unsigned int, plug_state> household_aggregate;
plug_state house_aggregate;


void processHouse(unsigned int boundary_ts, unsigned int x, float load) {

	static unsigned int last_timestamp = boundary_ts;
	if (last_timestamp < boundary_ts) {
		for (auto& slice:timeslice_lengths)
		if (last_timestamp % slice.second == 0) {
//			process(last_timestamp, slice.first, house_aggregate[slice.first].accumulated_load);
			house_aggregate[slice.first].accumulated_load = 0;
		}
		else
			break;
		last_timestamp = boundary_ts;
	}
	house_aggregate[x].accumulated_load += load;
}

void processHouseHold(unsigned int boundary_ts, unsigned int x, float load, unsigned int household_id) {

	static unsigned int last_timestamp = boundary_ts;
	if (last_timestamp < boundary_ts) {
		for (auto& household:household_aggregate) {
			for (auto& slice:timeslice_lengths)
			if (last_timestamp % slice.second == 0) {
				processHouse(last_timestamp, slice.first, household.second[slice.first].accumulated_load);
				household.second[slice.first].accumulated_load = 0;
			}
			else
				break;
			household_aggregate.erase(household.first);
		}
		last_timestamp = boundary_ts;
	}
	if (household_aggregate.find(household_id) == household_aggregate.end())
		household_aggregate[household_id] = initial_plug_state;
	household_aggregate[household_id][x].accumulated_load += load;
}


//for each measurement read do
void doProcessing(measurement *input) {

	if (input->property == 0) //ignore if the measurement is a work value
		return ;

	if ( state.find(input->household_id) == state.end() )
		state[input->household_id] = initial_household_state;
	if ( state[input->household_id].find(input->plug_id) ==
			state[input->household_id].end() ) {
		state[input->household_id][input->plug_id] = initial_plug_state;
		for (auto& x:timeslice_lengths) {
			state[input->household_id][input->plug_id][x.first].last_timestamp =
					input->timestamp -
					(input->timestamp % x.second == 0)? x.second : input->timestamp % x.second;
		}
	}

	plug_state &p_state = state[input->household_id][input->plug_id];

	if (input->timestamp - p_state[TIMESLICE_1M].last_timestamp <= timeslice_lengths.at(TIMESLICE_1M)) {
		p_state[TIMESLICE_1M].accumulated_load += input->value;
		p_state[TIMESLICE_1M].count_of_values++;
	}

	for (auto& x:timeslice_lengths) {
		if (input->timestamp - p_state[x.first].last_timestamp >= x.second) {
			p_state[x.first].last_timestamp += x.second;
			p_state[x.first+1].accumulated_load += p_state[x.first].accumulated_load;
			p_state[x.first+1].count_of_values += p_state[x.first].count_of_values;
			float average_load = p_state[x.first].accumulated_load /  p_state[x.first].count_of_values;
//			median[plug_id][x][ (line[ts] / x) * x % 86400].insert(p_state[x][load])
//			forcast_load( (line[ts] / x) * x + 2 * x, x )
			// q1 result
//			h_hh(  (line[ts] / x) * x, x, p_state[x][load] , plug_id ) // for query 1
			if (x.first != TIMESLICE_30S) {
				p_state[x.first].accumulated_load = 0;
				p_state[x.first].count_of_values = 0;
			}
		}
		else
			break;
	}

	if (input->timestamp - p_state[TIMESLICE_1M].last_timestamp != 0) {
		p_state[TIMESLICE_1M].accumulated_load += input->value;
		p_state[TIMESLICE_1M].count_of_values++;
	}

/*
	sliding_median[plug_id].insert(line[ts], line[load])
	if ( last_ts < line[ts] )
	h(last_ts, total_load)
	//for query 2
	h(last_ts, sliding_median[all_plugs][1H], 1H)
	h(last_ts, sliding_median[all_plugs][24H], 24H)
	last_ts = line[ts]
	total_load = line[load]
	else
	total_load += line[load]


*/

}


int main(int argc, char *argv[]) {

	return 0;
}
