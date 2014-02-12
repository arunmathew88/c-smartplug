#include <sys/socket.h>
#include <map>
#include <vector>
#include <common.h>

int main(int argc, char *argv[]) {

	//for each measurement read do
	struct measurement *measurement;
	house_state_ptr state = new house_state;

	if ( state->find(measurement->household_id) == state->end() )
		(*state)[measurement->household_id] = new household_state;

	if ( (*state)[measurement->household_id]->find(measurement->plug_id) ==
			(*state)[measurement->household_id]->end() ) {

		plug_state_ptr p_state 		= new plug_state;
		(*p_state)[TIMESLICE_1M] 	= new timeslice_state;
		(*p_state)[TIMESLICE_5M] 	= new timeslice_state;
		(*p_state)[TIMESLICE_15M] 	= new timeslice_state;
		(*p_state)[TIMESLICE_60M] 	= new timeslice_state;
		(*p_state)[TIMESLICE_120M] = new timeslice_state;
		(*(*state)[measurement->household_id])[measurement->plug_id] = p_state;
	}

	plug_state_ptr p_state = (*(*state)[measurement->household_id])[measurement->plug_id];

/*	for (plug_state::iterator x = p_state.begin(); x != p_state.end(); x++) {
		if ( measurement->timestamp -­ x[last_ts] > x )
			p_state[x][last_ts] = (line[ts] / x) * x
			p_state[x+1][load]  += p_state[x][load]
			p_state[x+1][count] += p_state[x][count]
			p_state[x][load]    /= p_state[x][count]
			median[plug_id][x][ (line[ts] / x) * x % 86400].insert(p_state[x][load])
			forcast_load( (line[ts] / x) * x + 2 * x, x )
			// q1 result
			h_hh(  (line[ts] / x) * x, x, p_state[x][load] , plug_id ) // for query 1
			p_state[x][load]    = 0
			p_state[x][count] = 0
		else
			break
	}

	p_state[60][load] += line[load]
	p_state[60][count] ++
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


