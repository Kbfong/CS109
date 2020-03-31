// $Id: bigint.cpp,v 1.76 2016-06-14 16:34:24-07 - - $

#include <cstdlib>
#include <exception>
#include <stack>
#include <stdexcept>
using namespace std;

#include "bigint.h"
#include "debug.h"
#include "relops.h"

bigint::bigint (long that): uvalue (that), is_negative (that < 0) {
   DEBUGF ('~', this << " -> " << uvalue)
}

bigint::bigint (const ubigint& uvalue, bool is_negative):
                uvalue(uvalue), is_negative(is_negative) {
}

bigint::bigint (const string& that) {
   is_negative = that.size() > 0 and that[0] == '_';
   uvalue = ubigint (that.substr (is_negative ? 1 : 0));
}

bigint bigint::operator+ () const {
   return *this;
}

bigint bigint::operator- () const {
   return {uvalue, !is_negative};
}

bigint bigint::operator+ (const bigint& that) const {
  if(is_negative == that.is_negative)
    return {uvalue + that.uvalue, is_negative};
  else
  {
    if(uvalue < that.uvalue)
      return {that.uvalue - uvalue, that.is_negative};
    else if (uvalue > that.uvalue)
      return {uvalue - that.uvalue, is_negative};
  }
  return {ubigint {0}, false};
}

bigint bigint::operator- (const bigint& that) const {
  if(is_negative == that.is_negative)
  {
    if(uvalue < that.uvalue)
      return {that.uvalue - uvalue, !is_negative};
    else if (uvalue > that.uvalue)
      return {uvalue - that.uvalue, is_negative};
    return {ubigint {0}, false};
  }
  return {uvalue + that.uvalue, is_negative};
}

bigint bigint::operator* (const bigint& that) const {
  if(is_negative == that.is_negative)
    return {uvalue * that.uvalue, false};
  return {uvalue * that.uvalue, true};
}

bigint bigint::operator/ (const bigint& that) const {
  if(is_negative == that.is_negative)
    return {uvalue / that.uvalue, false};
  return {uvalue / that.uvalue, true};
}

bigint bigint::operator% (const bigint& that) const {
  if(is_negative == that.is_negative)
    return {uvalue % that.uvalue, false};
  return {uvalue % that.uvalue, true};
}

bool bigint::operator== (const bigint& that) const {
  return is_negative == that.is_negative and uvalue == that.uvalue;
}

bool bigint::operator< (const bigint& that) const {
   if (is_negative != that.is_negative) return is_negative;
   return is_negative ? that.uvalue < uvalue
                      : uvalue < that.uvalue;
}

ostream& operator<< (ostream& out, const bigint& that) {
   return out << (that.is_negative ? "-" : "") << that.uvalue;
}

