//Templated Spreadsort-based implementation of integer_sort

//          Copyright Steven J. Ross 2001 - 2014.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

// See http://www.boost.org/libs/sort/ for library home page.
      
/*
Some improvements suggested by:
Phil Endecott and Frank Gennari
*/

#ifndef BOOST_INTEGER_SORT_HPP
#define BOOST_INTEGER_SORT_HPP
#include <algorithm>
#include <vector>
#include <cstring>
#include <limits>
#include <boost/static_assert.hpp>
#include <boost/sort/detail/constants.hpp>
#include <boost/sort/detail/spread_sort.hpp>

namespace boost {
  //Top-level sorting call for integers
  template <class RandomAccessIter>
  inline void integer_sort(RandomAccessIter first, RandomAccessIter last) 
  {
    //Don't sort if it's too small to optimize
    if (last - first < detail::min_sort_size)
      std::sort(first, last);
    else
      detail::integer_sort(first, last, *first >> 0);
  }

  //integer_sort with functors
  template <class RandomAccessIter, class Right_shift, class Compare>
  inline void integer_sort(RandomAccessIter first, RandomAccessIter last,
                           Right_shift shift, Compare comp) {
    if (last - first < detail::min_sort_size)
      std::sort(first, last, comp);
    else
      detail::integer_sort(first, last, shift(*first, 0), shift, comp);
  }

  //integer_sort with Right_shift functor
  template <class RandomAccessIter, class Right_shift>
  inline void integer_sort(RandomAccessIter first, RandomAccessIter last,
                           Right_shift shift) {
    if (last - first < detail::min_sort_size)
      std::sort(first, last);
    else
      detail::integer_sort(first, last, shift(*first, 0), shift);
  }
}

#endif
