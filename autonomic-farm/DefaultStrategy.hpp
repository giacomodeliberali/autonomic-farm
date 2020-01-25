#ifndef DEFAULT_STRATEGY_HPP
#define DEFAULT_STRATEGY_HPP

#include "IStrategy.hpp"
#include "Constants.hpp"
#include "Flags.hpp"

// The default strategy for the monitor
class DefaultStrategy: public IStrategy
{

private:
    float expected_throughput_;
    vector<float> last_window_;
    vector<float> *current_window_;
    

    
public:

    DefaultStrategy(float expected_throughput): expected_throughput_(expected_throughput){

    }
    
    Flags get(float actual_throughput) override 
    {



        return Flags::NONE;
    };
};

#endif