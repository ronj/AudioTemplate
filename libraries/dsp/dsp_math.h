#ifndef JAAP_COMMON_H
#define JAAP_COMMON_H

#include <cmath>
#include <limits>

static const double PI     = std::atan(1.0f) * 4.0f;
static const double TWO_PI = std::atan(1.0f) * 8.0f;

template <typename T>
T clamp(T value, T min, T max)
{
  return std::max(min, std::min(value, max));
}

template <typename T>
T map(T value, T inputMin, T inputMax, T outputMin, T outputMax, bool clamp = false)
{
  if (std::abs(inputMin - inputMax) < std::numeric_limits<float>::epsilon())
  {
    //std::cout << "ofMap(): avoiding possible divide by zero, check inputMin and inputMax: " << inputMin << " " << inputMax << std::endl;
    return outputMin;
  }
  else
  {
    float outVal = ((value - inputMin) / (inputMax - inputMin) * (outputMax - outputMin) + outputMin);

    if (clamp)
    {
      if(outputMax < outputMin)
      {
        if( outVal < outputMax )outVal = outputMax;
        else if( outVal > outputMin )outVal = outputMin;
      }
      else
      {
        if( outVal > outputMax )outVal = outputMax;
        else if( outVal < outputMin )outVal = outputMin;
      }
    }

    return outVal;
  }
}

#endif // JAAP_COMMON_H
