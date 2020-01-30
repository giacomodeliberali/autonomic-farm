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
    float slope_threshold_;
    float avg_threshold_;
    vector<float> current_window_;

    bool add_to_window(float actual_throughput)
    {
        if (current_window_.size() < STRATEGY_WINDOW_SIZE)
            current_window_.push_back(actual_throughput);

        return current_window_.size() >= STRATEGY_WINDOW_SIZE;
    }

    float get_slope()
    {
        float slope = 0;
        for (int i = 1; i < current_window_.size(); i++)
            slope += current_window_[i] - current_window_[i - 1];

        return slope;
    }

    float get_average()
    {
        float sum = 0;
        for (int i = 0; i < current_window_.size(); i++)
            sum += current_window_[i];

        return sum / current_window_.size();
    }

    bool is_in_average(float avg)
    {
        return avg >= expected_throughput_ - avg_threshold_ &&
               avg <= expected_throughput_ + avg_threshold_;
    }

    bool is_above_average(float avg)
    {
        return avg > expected_throughput_ + avg_threshold_;
    }

    bool is_under_average(float avg)
    {
        return avg < expected_throughput_ - avg_threshold_;
    }

    bool is_costant_slope(float slope)
    {
        return slope >= -slope_threshold_ && slope <= slope_threshold_;
    }

    bool is_positive_slope(float slope)
    {
        return slope > slope_threshold_;
    }

    bool is_negative_slope(float slope)
    {
        return slope < slope_threshold_;
    }

public:
    DefaultStrategy(float expected_throughput) : expected_throughput_(expected_throughput)
    {
        this->slope_threshold_ = SLOPE_THRESHOLD;
        this->avg_threshold_ = AVERAGE_THROUGHPUT_THRESHOLD * expected_throughput_;
    }

    int get(float actual_throughput, int actual_workers_number) override
    {
        auto cmd = NONE;
        if (add_to_window(actual_throughput))
        {
            auto slope = get_slope();
            auto average = get_average();

            if (!is_in_average(average))
            {
                if (is_costant_slope(slope))
                {
                    if (is_under_average(average))
                        cmd = ADD_WORKER;
                    else if (is_above_average(average))
                        cmd = REMOVE_WORKER;
                }
                else if (is_negative_slope(slope))
                {
                    if (is_under_average(average))
                        cmd = ADD_WORKER;
                }
                else if (is_positive_slope(slope))
                {
                    if (is_above_average(average))
                        cmd = REMOVE_WORKER;
                }
            }

            cmd = FlagUtils::combine(cmd, WINDOW_FULL);

            current_window_.clear();
        }

        return cmd;
    };
};

#endif