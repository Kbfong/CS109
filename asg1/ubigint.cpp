// $Id: ubigint.cpp,v 1.14 2016-06-23 17:21:26-07 - - $

#include <cctype>
#include <cstdlib>
#include <exception>
#include <stack>
#include <stdexcept>
using namespace std;

#include "ubigint.h"
#include "debug.h"

ubigint::ubigint (unsigned long that) {
   while(that > 0)
   {
      ubig_value.push_back(that % 10);
      that /= 10;
   }
}

ubigint::ubigint (const string& that) {
   for (char digit: that) {
      if (not isdigit (digit)) {
         throw invalid_argument ("ubigint::ubigint(" + that + ")");
      }
      ubig_value.insert(ubig_value.begin(), digit - '0');
   }
   while(ubig_value.size() > 0 && ubig_value.back() == 0) //eliminate extra 0s
      ubig_value.pop_back();
}

ubigint ubigint::operator+ (const ubigint& that) const {
   ubigint result {0};
   unsigned int i {0}; //keeps track of place in numbers
   bool carry {0}; //keeps track of carry over value
   int total {0}; //contains the sum for every addition of number pairs
   unsigned int length = (ubig_value.size() < that.ubig_value.size() 
                        ? ubig_value.size() : that.ubig_value.size()); //determine which is larger
   while(i < length || carry > 0)
   {
      if(i < ubig_value.size())
         total += ubig_value[i];
      if(i < that.ubig_value.size())
         total += that.ubig_value[i];
      total += carry;
      carry = 0;
      if(total > 9)
      {
         carry = 1;
         total -= 10;
      }
      result.ubig_value.push_back(total);
      total = 0;
      i++;
   }
   if(ubig_value.size() > that.ubig_value.size())
   {
      while(i < ubig_value.size())
      {
         result.ubig_value.push_back(ubig_value[i]);
         i++;
      }
   }
   else if(ubig_value.size() < that.ubig_value.size())
   {
      while(i < that.ubig_value.size())
      {
         result.ubig_value.push_back(that.ubig_value[i]);
         i++;
      }
   }
   return result;
}

ubigint ubigint::operator- (const ubigint& that) const {
   if (*this < that) throw domain_error ("ubigint::operator-(a<b)");
   ubigint result {0};
   unsigned int i {0}; //keeps track of carry over value
   bool borrow {0}; //keeps track of borrowing between numbers
   int total {0}; //contains the difference for each subtraction of number pairs
   while(i < that.ubig_value.size())
   {
      total = ubig_value[i] - borrow;
      borrow = 0;
      if(total < that.ubig_value[i])
      {
         total += 10;
         borrow = 1;
      }
      result.ubig_value.push_back(total - that.ubig_value[i]);
      i++;
   }
   while(i < ubig_value.size())
   {
      if(borrow > 0)
      {
         result.ubig_value.push_back(ubig_value[i] - 1);
         borrow = 0;
      }
      else
         result.ubig_value.push_back(ubig_value[i]);
      i++;
   }
   while(result.ubig_value.size() > 0 && result.ubig_value.back() == 0) //eliminate extra 0s
      result.ubig_value.pop_back();
   return result;
}

ubigint ubigint::operator* (const ubigint& that) const {
   ubigint result {0};
   for(unsigned int i = 0; i < ubig_value.size(); i++)
   {
      ubigint current {0};
      int product {0};
      int carry {0};
      for(unsigned int j = 0; j < that.ubig_value.size(); j++)
      {
         product = (ubig_value[i] * that.ubig_value[j]) + carry;
         carry = 0;
         if(product > 9)
         {
            carry = product/10;
            product = product%10;
         }
         current.ubig_value.push_back(product);
      }
      if(carry > 0)
      {
         current.ubig_value.push_back(carry);
         carry = 0;
      }
      for(unsigned int k = 0; k < i; k++) //offset
         current.ubig_value.insert(current.ubig_value.begin(), 0);
      result = result + current;
   }
   return result;
}

void ubigint::multiply_by_2() {
   (*this) = (*this) + (*this);
}

void ubigint::divide_by_2() {
   unsigned int remainder {0};
   for(unsigned int i = 0; i < ubig_value.size(); i++)
   {
      if(i != ubig_value.size() - 1 && ubig_value[i+1] % 2 != 0)
         remainder = 5;
      unsigned int current = ubig_value[i]/2;
      cout << "current = " << current << endl; 
      current += remainder;
      remainder = 0;
      cout << "current + remainder = " << current << endl; 
      ubig_value[i] = current;
   }
   while(ubig_value.size() > 0 && ubig_value.back() == 0) //eliminate extra 0s
      ubig_value.pop_back();
}


struct quo_rem { ubigint quotient; ubigint remainder; };
quo_rem udivide (const ubigint& dividend, ubigint divisor) {
   // Note: divisor is modified so pass by value (copy).
   ubigint zero {0};
   if (divisor == zero) throw domain_error ("udivide by zero");
   ubigint power_of_2 {1};
   ubigint quotient {0};
   ubigint remainder {dividend}; // left operand, dividend
   while (divisor < remainder) {
      divisor.multiply_by_2();
      power_of_2.multiply_by_2();
   }
   while (power_of_2 > zero) {
      if (divisor <= remainder) {
         remainder = remainder - divisor;
         quotient = quotient + power_of_2;
      }
      divisor.divide_by_2();
      power_of_2.divide_by_2();
   }
   return {.quotient = quotient, .remainder = remainder};
}

ubigint ubigint::operator/ (const ubigint& that) const {
   return udivide (*this, that).quotient;
}

ubigint ubigint::operator% (const ubigint& that) const {
   return udivide (*this, that).remainder;
}

bool ubigint::operator== (const ubigint& that) const {
   if(ubig_value.size() == that.ubig_value.size())
   {
      for(unsigned int i = 0; i < ubig_value.size(); i++)
      {
         if(ubig_value[i] != that.ubig_value[i])
            return false;
      }
      return true;
   }
   return false;
}

bool ubigint::operator< (const ubigint& that) const {
   if(*this == that)
      return false;
   if(ubig_value.size() < that.ubig_value.size())
      return true;
   else if (ubig_value.size() == that.ubig_value.size())
   {
      for(int i = ubig_value.size() - 1; i >= 0; i--)
      {
         if(ubig_value[i] > that.ubig_value[i])
            return false;
      }
      return true;
   }
   return false;
}

ostream& operator<< (ostream& out, const ubigint& that) {
   int line =  70; //length of line to wrap around
   if(that.ubig_value.size() > 0)
   {
      int count = 0; //keeps track of how many characters there are
      for(int i = that.ubig_value.size()-1; i >= 0; i--)
      {
         if(count%(line - 1) == 0 && count != 0) //if the count reaches the line limit
            out << "\\" << endl;
         out << static_cast<char>(that.ubig_value.at(i) + '0');
         count++;
      }
   }
   else
      out << '0';
   return out;
}

