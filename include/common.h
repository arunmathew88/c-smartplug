/*
 * common.h
 *
 *  Created on: 12-Feb-2014
 *      Author: arunmathew
 */

#ifndef COMMON_H_
#define COMMON_H_

#include <unordered_map>

struct measurement {
	unsigned long	id;
	unsigned int	timestamp;
	float			value;
	unsigned char	property;
	unsigned int	plug_id;
	unsigned int	household_id;
	unsigned int	house_id;
};

struct timeslice_state {
	unsigned int	last_timestamp;
	float			accumulated_load;
	unsigned int	count_of_values;
};

typedef struct timeslice_state* timeslice_state_ptr;

typedef std::unordered_map<unsigned int, timeslice_state_ptr> 	plug_state;
typedef plug_state* 											plug_state_ptr;
typedef std::unordered_map<unsigned int, plug_state_ptr>	 	household_state;
typedef household_state*										household_state_ptr;
typedef std::unordered_map<unsigned int, household_state_ptr> 	house_state;
typedef house_state*								 			house_state_ptr;

/*
#define TIMESLICE_1M	60
#define TIMESLICE_5M	300
#define TIMESLICE_15M	900
#define TIMESLICE_60M	3600
#define TIMESLICE_120M	7200
*/

typedef enum {TIMESLICE_1M = 0, TIMESLICE_5M, TIMESLICE_15M, TIMESLICE_60M, TIMESLICE_120M} timeslice;


const int timeslice_length[5] = {60, 300, 900, 3600, 7200};


#endif /* COMMON_H_ */
