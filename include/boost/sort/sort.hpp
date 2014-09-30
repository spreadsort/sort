// Templated generic hybrid sorting

//          Copyright Steven J. Ross 2001 - 2009.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

// See http://www.boost.org/libs/sort/ for library home page.
      
/*
Some improvements suggested by:
Phil Endecott and Frank Gennari
float_mem_cast fix provided by:
Scott McMurray
*/

#ifndef BOOST_SORT_HPP
#define BOOST_SORT_HPP
#include <algorithm>
#include <vector>
#include <cstring>
#include <string>
#include <limits>
#include <boost/static_assert.hpp>
#include <boost/type_traits.hpp>
#include <boost/sort/detail/constants.hpp>
#include <boost/sort/integer_sort.hpp>
#include <boost/sort/float_sort.hpp>
#include <boost/sort/string_sort.hpp>

namespace boost {
  //Generic spread_sort call to integer_sort
  template <class RandomAccessIter>
  inline typename boost::enable_if_c< std::numeric_limits< 
    typename std::iterator_traits<RandomAccessIter>::value_type >::is_integer, 
    void >::type
  sort(RandomAccessIter first, RandomAccessIter last) 
  {
    integer_sort(first, last);
  }

  //Generic spread_sort call to float_sort
  template <class RandomAccessIter>
  inline typename boost::enable_if_c< !std::numeric_limits< 
    typename std::iterator_traits<RandomAccessIter>::value_type >::is_integer
    && std::numeric_limits< 
    typename std::iterator_traits<RandomAccessIter>::value_type >::is_iec559, 
    void >::type
  sort(RandomAccessIter first, RandomAccessIter last) 
  {
    float_sort(first, last);
  }

  //Generic spread_sort call to string_sort for strings and wstrings
  template <class RandomAccessIter>
  inline typename boost::enable_if_c< 
    is_same<typename std::iterator_traits<RandomAccessIter>::value_type, 
            typename std::string>::value || 
    is_same<typename std::iterator_traits<RandomAccessIter>::value_type, 
            typename std::wstring>::value, void >::type
  sort(RandomAccessIter first, RandomAccessIter last)
  {
    string_sort(first, last);
  }
}

#endif
