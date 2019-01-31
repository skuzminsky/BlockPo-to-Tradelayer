#include "omnicore/externfns.h"
#include "omnicore/tradelayer_matrices.h"
#include "omnicore/log.h"
#include "omnicore/parse_string.h"
#include "omnicore/mdex.h"

#include <unordered_set>
#include <boost/lexical_cast.hpp>
#include <stdint.h>
#include <algorithm>
#include <string>
#include <boost/math/constants/constants.hpp>
#include <boost/multiprecision/cpp_dec_float.hpp>
#include <iostream>
#include <limits>

using mastercore::StrToInt64;
typedef boost::multiprecision::cpp_dec_float_100 dec_float;

using namespace std;

void printing_matrix(MatrixTLS &gdata)
{
  int M = size(gdata, 0);
  int N = size(gdata, 1);

  for (int i = 0; i < M; ++i)
    {
      for (int j = 0; j < N; ++j)
	{
	  PrintToLog("%s\t", gdata[i][j]);
	}
      PrintToLog("\n");
    }
}

void printing_vector(VectorTLS &vdata)
{
  int n = length(vdata);
  for (int i = 0; i < n; ++i) PrintToLog("%s\t", vdata[i]);
}

bool finding(std::string &s, VectorTLS &v)
{
  int n = length(v);
  bool findout = false;

  for (int i = 0; i < n; ++i)
    {
      if ( v[i] == s )
	{
	  findout = true;
	  break;
	}
    }
  return findout;
}

void sub_row(VectorTLS &jrow_databe, MatrixTLS &databe, int i)
{
  for (int j = 0; j < size(databe, 1); ++j) jrow_databe[j] = databe[i][j];
}

bool is_number(const std::string& s)
{
  std::string::const_iterator it = s.begin();
  while (it != s.end() && std::isdigit(*it)) ++it;
  return !s.empty() && it == s.end();
}
bool finding_string(std::string sub_word, std::string word_target)
{
  bool word_bool = false;

  std::size_t found = word_target.find(sub_word);
  if (found != std::string::npos)
    word_bool = true;

  return word_bool;
}

bool find_string_strv(std::string s, std::vector<std::string> v)
{
  bool find_bool = false;
  for (std::vector<std::string>::iterator it = v.begin(); it != v.end(); ++it)
    {
      if ( finding_string(s, *it) )
	{
	  find_bool = true;
	  break;
	}
    }
  return find_bool;
}

bool find_uint64_t(uint64_t m, std::vector<uint64_t> v)
{
  bool find_bool = false;
  for (std::vector<uint64_t>::iterator it = v.begin(); it != v.end(); ++it)
    {
      if ( *it == m )
	{
	  find_bool = true;
	  break;
	}
    }
  return find_bool;
}

bool find_string_set(std::string s, std::unordered_set<std::string> addrs_set)
{
  bool find_bool = false;
  for (const std::string &x : addrs_set)
    {
      if ( finding_string(s, x) )
	{
	  find_bool = true;
	  break;
	}
    }
  return find_bool;
}

bool findTrueValue(bool a, bool b) { return a || b; }
bool findTrueValue(bool a, bool b, bool c) { return ( a || b ) || c; }
bool findTrueValue(bool a, bool b, bool c, bool d) { return ( a || b ) || ( c || d ); }

namespace mastercore
{
  int64_t DoubleToInt64(double d) { return mastercore::StrToInt64(boost::lexical_cast<std::string>(d), true); }
  int64_t RationalToInt64(rational_t r) { return mastercore::StrToInt64(xToString(r), true); }
}
