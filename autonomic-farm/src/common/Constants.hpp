#ifndef CONSTANTS_HPP
#define CONSTANTS_HPP

// The strategy window size
#define STRATEGY_WINDOW_SIZE 50

// The slope threshold
#define SLOPE_THRESHOLD 0.02

// If the slope is above this threshold, the number of workers added/removed is 4 instaed of 1
#define DOUBLE_COMMAND_SLOPE_THRESHOLD 0.15

// If the slope is above this threshold, the number of workers added/removed is 2 instaed of 1
#define QUAD_COMMAND_SLOPE_THRESHOLD 0.30

// The average throughput treshold wrt to the given throughput (10% of given throughput)
#define AVERAGE_THROUGHPUT_THRESHOLD 0.1

// The size of the chunks of the input arrays
#define CHUNK_SIZE 30000


#endif
