#ifndef FILTER_H
#define FILTER_H

#include <array>

template <typename CoefficientType>
struct Coefficients
{
    std::array<CoefficientType, 3> a = { { 1, 0, 0 } };
    std::array<CoefficientType, 3> b = { { 0, 0, 0 } };
};

template <typename StateType>
class DirectFormI
{
public:
    void reset()
    {
        x.fill(0);
        y.fill(0);
    }

    template <typename SampleType, class DenormalPrevention>
    SampleType operator()(SampleType aIn, const Coefficients<StateType>& aCoefficients, const DenormalPrevention& aDenormalPrevention)
    {
        double out = aDenormalPrevention(aCoefficients.b[0] * aIn + aCoefficients.b[1] * x[0] + aCoefficients.b[2] * x[1]
                                                                  - aCoefficients.a[1] * y[0] - aCoefficients.a[2] * y[1]);

        x[1] = x[0];
        y[1] = y[0];
        x[0] = aIn;
        y[0] = out;

        return static_cast<SampleType>(out);
    }

private:
    std::array<StateType, 2> x = { 0, 0 };
    std::array<StateType, 2> y = { 0, 0 };
};

template <typename StateType>
class DirectFormII
{
public:
    void reset()
    {
        v.fill(0);
    }

    template <typename SampleType, class DenormalPrevention>
    SampleType operator()(SampleType aIn, const Coefficients<StateType>& aCoefficients, const DenormalPrevention& aDenormalPrevention)
    {
        double w =  aDenormalPrevention(aIn - aCoefficients.a[1] * v[0] - aCoefficients.a[2] * v[1]);
        double out = aCoefficients.b[0] * w + aCoefficients.b[1] * v[0] + aCoefficients.b[2] * v[1];

        v[1] = v[0];
        v[0] = w;

        return static_cast<SampleType>(out);
    }

private:
    std::array<StateType, 2> v = { 0, 0 };
};

template <std::size_t Channels, class FormState>
class MultiChannel
{
    static_assert(Channels > 0, "Channel count should be at least 1");

public:
    std::size_t channelCount() const
    {
        return Channels;
    }

    void reset()
    {
        for (auto& s: state)
        {
            s.reset();
        }
    }

private:
    std::array<FormState, Channels> state;
};

class Biquad
{
private:
    Coefficients<double> coefficients;
};

#endif // FILTER_H
