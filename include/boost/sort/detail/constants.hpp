//constant definitions for the Boost Sort library

//          Copyright Steven J. Ross 2001 - 2014
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org/libs/sort for library home page.
#ifndef BOOST_SORT_DETAIL_CONSTANTS
#define BOOST_SORT_DETAIL_CONSTANTS
namespace boost {
namespace detail {
//Tuning constants
//This should be tuned to your processor cache;
//if you go too large you get cache misses on bins
//The smaller this number, the less worst-case memory usage.
//If too small, too many recursions slow down spreadsort
enum { MAX_SPLITS = 11,
//It's better to have a few cache misses and finish sorting
//than to run another iteration
MAX_FINISHING_SPLITS = MAX_SPLITS + 1,
//Sets the minimum number of items per bin.
LOG_MEAN_BIN_SIZE = 2,
//Used to force a comparison-based sorting for small bins, if it's faster.
//Minimum value 1
LOG_MIN_SPLIT_COUNT = 9,
//This is the minimum split count to use spreadsort when it will finish in one
//iteration.  Make this larger the faster std::sort is relative to integer_sort.
LOG_FINISHING_COUNT = 31,
//Sets the minimum number of items per bin for floating point.
FLOAT_LOG_MEAN_BIN_SIZE = 2,
//Used to force a comparison-based sorting for small bins, if it's faster.
//Minimum value 1
FLOAT_LOG_MIN_SPLIT_COUNT = 8,
//This is the minimum split count to use spreadsort when it will finish in one
//iteration.  Make this larger the faster std::sort is relative to float_sort.
FLOAT_LOG_FINISHING_COUNT = 4,
//There is a minimum size below which it is not worth using spreadsort
MIN_SORT_SIZE = 3000 };
}
}
#endif