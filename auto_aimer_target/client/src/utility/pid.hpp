#pragma once

#include <cassert>

namespace utility {
template <typename Input, typename Output>
class Pid {
public:
    Pid()
        : is_initialized_(false)
    {
    }

    Pid(double p, double i, double d)
        : is_initialized_(true)
        , p_(p)
        , i_(i)
        , d_(d)

    {
    }

    void set(double p, double i, double d)
    {
        p_ = p;
        i_ = i;
        d_ = d;

        is_initialized_ = true;
    }

    void reset()
    {
        error_ = 0;
        error_last_ = 0;
        error_integral_ = 0;
    }

    bool is_initialized()
    {
        return is_initialized_;
    }

    Output update(Input real, Input target)
    {
        assert(is_initialized_);

        error_ = target - real;

        Output output
            = p_ * error_
            + i_ * error_integral_
            + d_ * (error_ - error_last_);

        error_last_ = error_;
        error_integral_ += error_;

        return output;
    }

private:
    bool is_initialized_;

    double p_;
    double i_;
    double d_;

    Input error_;
    Input error_last_;
    Input error_integral_;
};
}