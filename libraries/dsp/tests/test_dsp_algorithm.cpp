#include "yaffut.h"

#include <array>
#include <vector>

#include <dsp/common/dsp_algorithm.h>

struct algorithm_test
{
  std::array<int, 10> data_a = { { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 } };
  std::vector<int> data_b = { { 1000, 1005, 1010 } };
  std::vector<double> data_c = { { 123.4, 567.8 } };
  std::vector<double> data_d = { { -2.0, -3.0, 3, 1.0, 2.0, 0, -1.0 } };
};

TEST(algorithm_test, should_calculate_mean)
{
  EQUAL(5, mean(std::begin(data_a), std::end(data_a)));
  EQUAL(1005, mean(data_b.begin(), data_b.end()));
  EQUAL(345.6, mean(data_c.begin(), data_c.end()));
  EQUAL(0, mean(data_d.begin(), data_d.end()));
}

TEST(algorithm_test, should_determine_max_amplitude)
{
  EQUAL(10, max_amplitude(std::begin(data_a), std::end(data_a)));
  EQUAL(1010, max_amplitude(data_b.begin(), data_b.end()));
  EQUAL(567.8, max_amplitude(data_c.begin(), data_c.end()));
  EQUAL(3, max_amplitude(data_d.begin(), data_d.end()));
}

TEST(algorithm_test, should_calculate_rms)
{
  EQUAL(6, rms(std::begin(data_a), std::end(data_a)));
  EQUAL(1005, rms(data_b.begin(), data_b.end()));
  //EQUAL(410.868, rms(data_c.begin(), data_c.end()));
  EQUAL(2, rms(data_d.begin(), data_d.end()));
}

TEST(algorithm_test, should_calculate_std_dev)
{
  EQUAL(2, std_dev(std::begin(data_a), std::end(data_a)));
  EQUAL(4, std_dev(data_b.begin(), data_b.end()));
  EQUAL(222.2, std_dev(data_c.begin(), data_c.end()));
  EQUAL(2, std_dev(data_d.begin(), data_d.end()));
}
