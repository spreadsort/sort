//  Boost sorting_algo library string_sort_test.cpp file  ---------------------------//

//  Copyright Steven Ross 2009. Use, modification and
//  distribution is subject to the Boost Software License, Version
//  1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org for updates, documentation, and revision history.

#include <boost/sort/string_sort.hpp>
#include <boost/sort/sort.hpp>
// Include unit test framework
#include <boost/test/included/test_exec_monitor.hpp>
#include <boost/test/test_tools.hpp>
#include <vector>
#include <string>


using namespace std;
using namespace boost;

struct bracket {
  unsigned char operator()(const string &x, size_t offset) const { return x[offset]; }
};

struct getsize {
  size_t operator()(const string &x) const{ return x.size(); }
};

void string_test()
{
    // Prepare inputs
    vector<string> base_vec;
  const unsigned count = 10000;
  const unsigned max_length = 32;
  srand(1);
  //Generating semirandom numbers
  for(unsigned u = 0; u < count; ++u) {
    unsigned length = rand() % max_length;
    string result;
    for(unsigned u = 0; u < length; ++u)
      result.push_back(rand() % 256);
    base_vec.push_back(result);
  }
  vector<string> sorted_vec = base_vec;
  vector<string> test_vec = base_vec;
  std::sort(sorted_vec.begin(), sorted_vec.end());
  //Testing basic call
  string_sort(test_vec.begin(), test_vec.end());
  BOOST_CHECK(test_vec == sorted_vec);
  //Testing boost::sort wrapper
  boost::sort(test_vec.begin(), test_vec.end());
  BOOST_CHECK(test_vec == sorted_vec);
  //Character functors
  test_vec = base_vec;
  string_sort(test_vec.begin(), test_vec.end(), bracket(), getsize());
  BOOST_CHECK(test_vec == sorted_vec);
  //All functors
  test_vec = base_vec;
  string_sort(test_vec.begin(), test_vec.end(), bracket(), getsize(), less<string>());
  BOOST_CHECK(test_vec == sorted_vec);
  //reverse order
  std::sort(sorted_vec.begin(), sorted_vec.end(), greater<string>());
  reverse_string_sort(test_vec.begin(), test_vec.end(), greater<string>());
  BOOST_CHECK(test_vec == sorted_vec);
  //reverse order with functors
  test_vec = base_vec;
  reverse_string_sort(test_vec.begin(), test_vec.end(), bracket(), getsize(), greater<string>());
  BOOST_CHECK(test_vec == sorted_vec);
  
}

// test main 
int test_main( int, char*[] )
{
    string_test();
    
    return 0;
}
