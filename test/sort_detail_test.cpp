//  Boost Sort library sort_detail_test.cpp file  ----------------------------//

//  Copyright Steven Ross 2014. Use, modification and
//  distribution is subject to the Boost Software License, Version
//  1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org/libs/sort for library home page.

#include <boost/cstdint.hpp>
#include <boost/sort/detail/spread_sort.hpp>
#include <boost/sort/float_sort.hpp>
// Include unit test framework
#include <boost/test/included/test_exec_monitor.hpp>
#include <boost/test/test_tools.hpp>
#include <vector>

#include <iostream>


using namespace std;
using namespace boost;
using namespace boost::detail;

namespace {

struct int_right_shift {
  int operator()(const int x, const unsigned offset) const { 
    return x >> offset; 
  }
};

struct float_right_shift {
  int operator()(const float x, const unsigned offset) const { 
    return float_mem_cast<float, int>(x) >> offset; 
  }
};

struct bracket {
  unsigned char operator()(const string &x, size_t offset) const {
    return x[offset];
  }
};

struct getsize {
  size_t operator()(const string &x) const{ return x.size(); }
};

const int max_int_bits = sizeof(boost::uintmax_t) * 8;
const int max_size_bits = sizeof(size_t) * 8;
const boost::uintmax_t one = 1;

// spreadsort won't recurse for inputs smaller than min_count.
const int int_min_log_count = 
  (std::min)((int)LOG_FINISHING_COUNT, 
             (int)LOG_MEAN_BIN_SIZE + LOG_MIN_SPLIT_COUNT);
const int float_min_log_count = 
  (std::min)((int)FLOAT_LOG_FINISHING_COUNT,
             (int)FLOAT_LOG_MEAN_BIN_SIZE + FLOAT_LOG_MIN_SPLIT_COUNT);
const unsigned absolute_min_count = (std::min)(1 << int_min_log_count, 
                                               1 << float_min_log_count);

// Verify that roughlog2 is floor(log base 2) + 1.
void roughlog2_test()
{
  for (boost::uintmax_t i = 0; i < max_int_bits; ++i) {
    BOOST_CHECK(detail::rough_log_2_size(one << i) == i + 1);
    BOOST_CHECK(detail::rough_log_2_size((one << i) - 1) == i);
  }
}

// Test the worst-case performance handling, and assure that is using the
// correct formula for the worst-case number of radix iterations.
template<unsigned log_mean_bin_size, unsigned log_min_split_count,
         unsigned log_finishing_count>
void get_min_count_test()
{
  const unsigned min_log_size = log_mean_bin_size + log_min_split_count;
  size_t prev_min_count = absolute_min_count;
  for (int log_range = 0; log_range <= max_int_bits; ++log_range) {
    size_t min_count = get_min_count<log_mean_bin_size, log_min_split_count, 
                                     log_finishing_count>(log_range);
    BOOST_CHECK(min_count >= prev_min_count);
    prev_min_count = min_count;
    // When the range is really small, the radix sort will complete in one
    // iteration and worst-case handling doesn't apply.  The code below 
    // guarantees the worst-case number of radix sorting iteration.
    if (log_range > min_log_size) {
      BOOST_CHECK(min_count >= (1 << min_log_size));
      int iterations = rough_log_2_size(min_count) - min_log_size;
      BOOST_CHECK(iterations >= 1);
      int base_iterations = MAX_SPLITS - log_min_split_count;
      int covered_log_range = 0;
      if (iterations > base_iterations) {
        covered_log_range += MAX_SPLITS * (iterations - base_iterations);
      } else {
        base_iterations = iterations;
      }
      //sum of n to n + x = ((x + 1) * (n + (n + x)))/2 + log_mean_bin_size
      covered_log_range += (base_iterations * (log_min_split_count * 2 + base_iterations - 1))/2 + log_mean_bin_size;
      BOOST_CHECK(covered_log_range >= log_range);
      BOOST_CHECK(covered_log_range - MAX_SPLITS < log_range);
    }
  }
}

// Test the decision of how many pieces to split up the radix sort into
// (roughly 2^(log_range - log_divisor)) to make sure the results are logical.
void get_log_divisor_test()
{
  for (int log_range = 0; log_range <= max_int_bits; ++log_range) {
    int prev_log_divisor = max_int_bits +
      (std::max)((int)LOG_MEAN_BIN_SIZE, (int)FLOAT_LOG_MEAN_BIN_SIZE);
    for (int log_count = 0; log_count < max_size_bits; ++log_count) {
      size_t count = (one << log_count) - 1;
      BOOST_CHECK(rough_log_2_size(count) == (unsigned)log_count);
      int log_divisor = get_log_divisor<LOG_MEAN_BIN_SIZE>(count, log_range);
      // We will only process counts >= LOG_FINISHING_COUNT in this function.
      if (count >= absolute_min_count)
        BOOST_CHECK(log_divisor <= log_range);
      // More pieces should be used the larger count is.
      BOOST_CHECK(log_divisor <= prev_log_divisor);
      prev_log_divisor = log_divisor;
      BOOST_CHECK(log_divisor >= 0);
      if (log_range > log_count) {
        BOOST_CHECK(log_range - log_divisor <= MAX_SPLITS);
      } else if (log_range <= MAX_FINISHING_SPLITS) {
        BOOST_CHECK(log_divisor == 0);
      }
    }
  }
}

// Verify that is_sorted_or_find_extremes returns true if the data is sorted,
// and otherwise returns the actual min and max.
void is_sorted_or_find_extremes_test()
{
  vector<int> input;
  input.push_back(3);
  input.push_back(5);
  input.push_back(1);
  // Test a sorted input.
  vector<int> sorted_input(input);
  std::sort(sorted_input.begin(), sorted_input.end());
  vector<int>::iterator max, min;
  BOOST_CHECK(detail::is_sorted_or_find_extremes(sorted_input.begin(), 
                                                 sorted_input.end(), max, min));
  // Test an unsorted input.
  BOOST_CHECK(!detail::is_sorted_or_find_extremes(input.begin(), input.end(), 
                                                  max, min));
  BOOST_CHECK(*min == 1);
  BOOST_CHECK(*max == 5);
  // Test the comparison function version.
  BOOST_CHECK(detail::is_sorted_or_find_extremes(sorted_input.begin(), 
                                                 sorted_input.end(), max, min, 
                                                 std::less<int>()));
  BOOST_CHECK(!detail::is_sorted_or_find_extremes(sorted_input.begin(), 
                                                  sorted_input.end(), 
                                                  max, min, 
                                                  std::greater<int>()));
  BOOST_CHECK(*min == 5);
  BOOST_CHECK(*max == 1);

  // Test with floats
  vector<float> float_input;
  float_input.push_back(.3f);
  float_input.push_back(4.0f);
  float_input.push_back(.1f);
  vector<float> sorted_float_input(float_input);
  std::sort(sorted_float_input.begin(), sorted_float_input.end());
  // Test cast_float_iter
  int cast_min = detail::cast_float_iter<int, vector<float>::iterator>(
                     sorted_float_input.begin());
  int cast_max = detail::cast_float_iter<int, vector<float>::iterator>(
                     sorted_float_input.end() - 1);
  BOOST_CHECK(cast_min == float_right_shift()(.1f, 0));
  BOOST_CHECK(cast_max == float_right_shift()(4.0f, 0));
  // Test a sorted input
  int div_max, div_min;
  BOOST_CHECK(detail::is_sorted_or_find_extremes(sorted_float_input.begin(), 
                                                 sorted_float_input.end(), 
                                                 div_max, div_min));
  // Test an unsorted input.
  BOOST_CHECK(!detail::is_sorted_or_find_extremes(float_input.begin(), 
                                                  float_input.end(), 
                                                  div_max, div_min));
  BOOST_CHECK(div_min == cast_min);
  BOOST_CHECK(div_max == cast_max);

  // Test with a right_shift functor.
  BOOST_CHECK(detail::is_sorted_or_find_extremes(sorted_float_input.begin(), 
                                                 sorted_float_input.end(), 
                                                 div_max, div_min, 
                                                 float_right_shift()));
  // Test an unsorted input.
  BOOST_CHECK(!detail::is_sorted_or_find_extremes(float_input.begin(), 
                                                  float_input.end(), div_max, 
                                                  div_min, 
                                                  float_right_shift()));
  BOOST_CHECK(div_min == float_right_shift()(.1f, 0));
  BOOST_CHECK(div_max == float_right_shift()(4.0f, 0));
}

// Make sure bins are created correctly.
void size_bins_test() {
  std::vector<size_t> bin_sizes;
  bin_sizes.push_back(1);
  const int old_bin_value = 7;
  std::vector<int> old_bins;
  old_bins.push_back(old_bin_value);
  std::vector<vector<int>::iterator> bin_cache;
  bin_cache.push_back(old_bins.begin());
  unsigned cache_offset = 1;
  unsigned cache_end;
  const unsigned bin_count = 2;
  std::vector<int>::iterator *new_cache_start = 
    size_bins(bin_sizes, bin_cache, cache_offset, cache_end, bin_count);
  BOOST_CHECK((new_cache_start - &bin_cache[0]) == cache_offset);
  BOOST_CHECK(bin_sizes.size() == bin_count);
  BOOST_CHECK(bin_sizes[0] == 0);
  BOOST_CHECK(bin_sizes[1] == 0);
  BOOST_CHECK(cache_end == 3);
  BOOST_CHECK(bin_cache.size() == cache_end);
  BOOST_CHECK(old_bins[0] == old_bin_value);
}

// Test the specialized 3-way swap loops.
void swap_loop_test() {
  vector<size_t> bin_sizes;
  bin_sizes.push_back(2);
  bin_sizes.push_back(2);
  bin_sizes.push_back(1);

  // test integer swap loop
  vector<int> ints;
  const int int_div_min = 3;
  const int int_log_divisor = 1;
  const unsigned int_offset = int_div_min << int_log_divisor;
  ints.push_back(2 + int_offset);
  ints.push_back(1 + int_offset); // stays in place
  ints.push_back(4 + int_offset);
  ints.push_back(3 + int_offset);
  ints.push_back(0 + int_offset);
  vector<vector<int>::iterator> int_bin_vector;
  int_bin_vector.push_back(ints.begin());
  int_bin_vector.push_back(int_bin_vector[0] + bin_sizes[0]);
  int_bin_vector.push_back(int_bin_vector[1] + bin_sizes[1]);
  vector<int>::iterator next_int_bin_start = int_bin_vector[0];
  vector<int>::iterator *int_bins = &int_bin_vector[0];
  int_right_shift integer_right_shift;
  swap_loop(int_bins, next_int_bin_start, 0, integer_right_shift, bin_sizes, 
            int_log_divisor, int_div_min);
  for (unsigned i = 0; i < ints.size(); ++i) {
    BOOST_CHECK(ints[i] == int(int_offset + i));
  }
  BOOST_CHECK(next_int_bin_start == ints.begin() + bin_sizes[0]);

  // test float swap loop
  vector<float> floats;
  const int float_four_as_int = float_mem_cast<float, int>(4.0f);
  const int float_log_divisor = 
    rough_log_2_size(float_mem_cast<float, int>(5.0f) - float_four_as_int);
  const int float_div_min = float_four_as_int >> float_log_divisor;
  floats.push_back(6.0f);
  floats.push_back(5.0f); // stays in place
  floats.push_back(8.0f);
  floats.push_back(7.0f);
  floats.push_back(4.0f);
  vector<vector<float>::iterator> float_bin_vector;
  float_bin_vector.push_back(floats.begin());
  float_bin_vector.push_back(float_bin_vector[0] + bin_sizes[0]);
  float_bin_vector.push_back(float_bin_vector[1] + bin_sizes[1]);
  vector<float>::iterator next_float_bin_start = float_bin_vector[0];
  vector<float>::iterator *float_bins = &float_bin_vector[0];
  float_swap_loop(float_bins, next_float_bin_start, 0, bin_sizes,
                  float_log_divisor, float_div_min);
  for (unsigned i = 0; i < floats.size(); ++i) {
    BOOST_CHECK(floats[i] == 4.0f + i);
  }
  BOOST_CHECK(next_float_bin_start == floats.begin() + bin_sizes[0]);
}

// Test that update_offset finds the first character with a difference.
void update_offset_test() {
  vector<string> input;
  input.push_back("test1");
  input.push_back("test2");
  size_t char_offset = 1;
  update_offset(input.begin(), input.end(), char_offset);
  BOOST_CHECK(char_offset == 4);

  // Functor version
  char_offset = 1;
  update_offset(input.begin(), input.end(), char_offset, bracket(), getsize());
  BOOST_CHECK(char_offset == 4);
}

// Test that offset comparison operators only look after the offset.
void offset_comparison_test() {
  string input1 = "ab";
  string input2 = "ba";
  string input3 = "aba";
  offset_less_than<string, unsigned char> less_than(0);
  offset_greater_than<string, unsigned char> greater_than(0);
  BOOST_CHECK(less_than(input1, input2));
  BOOST_CHECK(less_than(input1, input3));
  BOOST_CHECK(!less_than(input2, input1));
  BOOST_CHECK(!less_than(input1, input1));
  BOOST_CHECK(!greater_than(input1, input2));
  BOOST_CHECK(!greater_than(input1, input3));
  BOOST_CHECK(greater_than(input2, input1));
  BOOST_CHECK(!greater_than(input1, input1));

  // Offset comparisons only check after the specified offset.
  offset_less_than<string, unsigned char> offset_less(1);
  offset_greater_than<string, unsigned char> offset_greater(1);
  BOOST_CHECK(!offset_less(input1, input2));
  BOOST_CHECK(offset_less(input1, input3));
  BOOST_CHECK(offset_less(input2, input1));
  BOOST_CHECK(!offset_less(input1, input1));
  BOOST_CHECK(offset_greater(input1, input2));
  BOOST_CHECK(!offset_greater(input1, input3));
  BOOST_CHECK(!offset_greater(input2, input1));
  BOOST_CHECK(!offset_greater(input1, input1));
}

} // end anonymous namespace

// test main 
int test_main( int, char*[] )
{
  roughlog2_test();
  get_min_count_test<LOG_MEAN_BIN_SIZE, LOG_MIN_SPLIT_COUNT,
                    LOG_FINISHING_COUNT>();
  get_min_count_test<FLOAT_LOG_MEAN_BIN_SIZE, FLOAT_LOG_MIN_SPLIT_COUNT,
                    FLOAT_LOG_FINISHING_COUNT>();
  get_log_divisor_test();
  is_sorted_or_find_extremes_test();
  size_bins_test();
  swap_loop_test();
  update_offset_test();
  offset_comparison_test();
  return 0;
}
