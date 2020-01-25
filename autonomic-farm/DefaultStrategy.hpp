#ifndef DEFAULT_STRATEGY_HPP
#define DEFAULT_STRATEGY_HPP

#include "IStrategy.hpp"
#include "Constants.hpp"
#include "Flags.hpp"
#include <vector>

// The default strategy for the monitor
class DefaultStrategy : public IStrategy
{

private:
    float expected_throughput_;
    vector<float> current_window_;
    Flags last_command;

    bool add_to_window(float actual_throughput)
    {
        if (current_window_.size() < STRATEGY_WINDOW_SIZE)
            current_window_.push_back(actual_throughput);

        return current_window_.size() >= STRATEGY_WINDOW_SIZE;
    }

    float get_trend()
    {
        float trend = 0;
        for (int i = 1; i < current_window_.size(); i++)
            trend += current_window_[i] - current_window_[i - 1];

        return trend;
    }

    float get_average()
    {
        float sum = 0;
        for (int i = 0; i < current_window_.size(); i++)
            sum += current_window_[i];

        return sum / current_window_.size();
    }

    bool is_costant_average(float avg)
    {
        return avg >= expected_throughput_ - AVERAGE_THRESHOLD &&
               avg <= expected_throughput_ + AVERAGE_THRESHOLD;
    }

    bool is_above_average(float avg)
    {
        return avg > expected_throughput_ + AVERAGE_THRESHOLD;
    }

    bool is_under_average(float avg)
    {
        return avg < expected_throughput_ - AVERAGE_THRESHOLD;
    }

    bool is_costant_trend(float trend)
    {
        return trend >= -TREND_THRESHOLD && trend <= TREND_THRESHOLD;
    }

    bool is_positive_trend(float trend)
    {
        return trend > TREND_THRESHOLD;
    }

    bool is_negative_trend(float trend)
    {
        return trend < TREND_THRESHOLD;
    }

public:
    DefaultStrategy(float expected_throughput) : expected_throughput_(expected_throughput)
    {
    }

    Flags get(float actual_throughput) override
    {
        auto cmd = Flags::NONE;
        if (add_to_window(actual_throughput))
        {
            auto trend = get_trend();
            auto average = get_average();

            if (is_costant_trend(trend))
            {
                if (is_under_average(average))
                    cmd = Flags::ADD_WORKER;
                else if (is_above_average(average))
                    cmd = Flags::REMOVE_WORKER;
            }
            else if (is_negative_trend(trend))
            {
                if (is_under_average(average))
                    cmd = Flags::ADD_WORKER;
            }
            else if (is_positive_trend(trend))
            {
                if (is_above_average(average))
                    cmd = Flags::REMOVE_WORKER;
            }

            current_window_.clear();
        }

        // avoid consecutive opposite commands
        if(last_command == Flags::ADD_WORKER && cmd == Flags::REMOVE_WORKER)
            cmd = Flags::NONE;

        if(last_command == Flags::REMOVE_WORKER && cmd == Flags::ADD_WORKER)
            cmd = Flags::NONE;

        last_command = cmd;

        return cmd;
    };
};

#endif