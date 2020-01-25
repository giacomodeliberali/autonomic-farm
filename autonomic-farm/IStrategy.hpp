#ifndef I_STRATEGY_HPP
#define I_STRATEGY_HPP

#include "Flags.hpp"

// The strategy for the monitor
class IStrategy
{
public:
    
    virtual Flags get(float actual_throughput) = 0;
};

#endif