#include <cassert>
#include <cstddef>
#include "bigint.h"
#include <cstdint>
#include <initializer_list>
#include <sstream>
#include <vector>
#include <iostream>
#include <iomanip>
#include <algorithm>

// Default constructor that initializes the BigInt value to 0, non-negative
BigInt::BigInt()
{
  isNegative = false;
}

// Constructor from an `std::initializer_list` of `uint64_t` values
BigInt::BigInt(std::initializer_list<uint64_t> vals, bool negative)
{
  isNegative = negative;
  elements = vals;
}

// Constructor from a `uint64_t` value
BigInt::BigInt(uint64_t val, bool negative)
{
  elements.push_back(val); // push the value to the vector
  isNegative = negative;   // set whether the value is negative or not
}

// Copy constructor
BigInt::BigInt(const BigInt &other)
{
  isNegative = other.isNegative;
  elements = other.elements;
}

// Destructor
BigInt::~BigInt()
{
}

// Assignment operator
BigInt &BigInt::operator=(const BigInt &rhs)
{
  if (this != &rhs) {
    isNegative = rhs.isNegative;
    elements = rhs.elements;
  }
  return *this;
}

// Check whether value is negative
bool BigInt::is_negative() const
{
  return isNegative;
}

// Gets the value of the bit at the specified index
uint64_t BigInt::get_bits(unsigned index) const
{
  if (index >= elements.size()) {
    return 0;
  }
  return elements.at(index);
}

// Returns the vector containing the bit string values
const std::vector<uint64_t> &BigInt::get_bit_vector() const {
  return elements;
}

// Addition operator
BigInt BigInt::operator+(const BigInt &rhs) const
{
  BigInt obj;
  if (isNegative == rhs.is_negative()) {  //if both are negative or positive
    obj = add_magnitudes(*this, rhs);
    if (!obj.is_zero()) {
      obj.isNegative = isNegative;  //result has the same sign as the operands
    }
  } else if (compare_magnitudes(*this, rhs) > 0) { 
    //operand is positive, subtract the smaller from the larger   
    obj = subtract_magnitudes(*this, rhs);
    if (!obj.is_zero()) {
      obj.isNegative = is_negative();
    }
  } else {
    // operand is negative, subtract positive
    obj = subtract_magnitudes(rhs, *this);
    if (!obj.is_zero()) {
      obj.isNegative = rhs.is_negative();
    }
  }
  return obj;
}

// Subtraction operator
BigInt BigInt::operator-(const BigInt &rhs) const
{
  // Hint: a - b could be computed as a + -b
  if (this->is_zero()) {
    BigInt obj = -rhs;
    return obj;
  } else if (rhs.is_zero()) {
    BigInt obj = *this;
    return obj;
  }
  if (isNegative == true && rhs.is_negative() == true) {
    if (compare_magnitudes(*this, rhs) > 0) {
      BigInt obj = subtract_magnitudes(*this, rhs);
      obj.isNegative = true;
      return obj;
    } else {
      BigInt obj = subtract_magnitudes(rhs, *this);
      obj.isNegative = false;
      return obj;
    }
  }
  if (isNegative == false && rhs.is_negative() == false) {
    if (compare_magnitudes(*this, rhs) > 0) {
      BigInt obj = subtract_magnitudes(*this, rhs);
      obj.isNegative = false;;
      return obj;
    } else {
      BigInt obj = subtract_magnitudes(rhs, *this);
      obj.isNegative = true;
      return obj;
    }
  } else {
    BigInt obj = add_magnitudes(*this, rhs);
    if (isNegative == false && rhs.is_negative() == true) {
      obj.isNegative = false;
    } else if (isNegative == true && rhs.is_negative() == false) {
      obj.isNegative = true;
    }
    return obj;
  }
}

// Unary negation operator
BigInt BigInt::operator-() const
{
  BigInt obj = BigInt(*this);
  if(!is_zero()) {
    obj.isNegative = !this->isNegative;
  }
  return obj;
}

// Test whether a specific bit in the bit string is set
bool BigInt::is_bit_set(unsigned n) const
{
  if (is_zero()) {
    return false; //if the value is zero, no bit is set
  }
  int pos = 0;
  if (n >= 64) {
    pos = n/64;  //position of the element in the vector
  }
  int bit = n%64;  //position of the bit in the element
  if (bit == 0) {
    return elements[pos] % 2 != 0;  //check if the least signficant bit is set
  }
  if (elements[pos] > (1UL << (bit))) {
    int section = elements[pos] / (1UL << (bit));
    if (section % 2 == 0) {
      return section % 2 != 0;  //check if the specific bit is set
    }
  }
  return false;
}

// Left shift ooerator
BigInt BigInt::operator<<(unsigned n) const
{
  if (isNegative) {
      throw std::invalid_argument("Cannot left shift a negative value");
  }
  if (n == 0 || this->is_zero()) {
      return *this;  //if n is 0 or the value is zero, no shift is needed
  }

  BigInt result;
  unsigned shift_units = n / 64;  //number of units to shift
  unsigned shift_bits = n % 64;   //number of bits to shift

  result.elements.resize(shift_units, 0);
  result.elements.insert(result.elements.end(), elements.begin(), elements.end());

  if (shift_bits > 0) {
      uint64_t carry = 0;
      for (std::size_t i = 0; i < result.elements.size(); ++i) {
          uint64_t temp = result.elements[i];
          result.elements[i] = (temp << shift_bits) | carry;   //shift and add carry
          carry = (temp >> (64 - shift_bits));  // update carry
      }
      if (carry > 0) {
          result.elements.push_back(carry);  //add the carry to the end
      }
  }

  result.isNegative = isNegative;
  return result;
}

// Multiplication operator
BigInt BigInt::operator*(const BigInt &rhs) const
{
  BigInt result;
  result.elements.resize(this->elements.size() + rhs.elements.size(), 0);

  for (std::size_t i = 0; i < this->elements.size(); ++i) {
      uint64_t carry = 0;
      for (std::size_t j = 0; j < rhs.elements.size(); ++j) {
          __uint128_t product = (__uint128_t)this->elements[i] * rhs.elements[j] + result.elements[i + j] + carry;
          result.elements[i + j] = (uint64_t)product;
          carry = (uint64_t)(product >> 64);  //update carry
      }
      result.elements[i + rhs.elements.size()] += carry;  //add the carry to the next element
  }

  result.isNegative = this->isNegative != rhs.isNegative;  //determine sign of the result
  return result;
}


BigInt BigInt::operator/(const BigInt &rhs) const {
  if (rhs.is_zero()) {
      throw std::invalid_argument("Division by zero");
  }

  //handle zero dividend
  if (this->is_zero()) {
      return BigInt(0);
  }

  //handle division by 1 or -1
  if (rhs == BigInt(1, false)) {
      return *this;
  }
  if (rhs == BigInt(1, true)) {
      return -(*this);
  }
  if (rhs == BigInt(-1, false)) {
      return -(*this);
  }
  if (rhs == BigInt(-1, true)) {
      return *this;
  }

  BigInt dividend = *this;
  BigInt divisor = rhs;
  dividend.isNegative = false;
  divisor.isNegative = false;

  if (dividend < divisor) {
      return BigInt(0);  //if divisor is greater, quotient is 0
  }

  //binary search
  BigInt low(0);             
  BigInt high = dividend;    
  BigInt mid;                
  BigInt quotient;

  while (low < high) {
      mid = (low + high + BigInt(1)).div_by_2();  //midpoint

      BigInt product = mid * divisor;
      if (product == dividend) {
          quotient = mid;
          break; 
      } else if (product < dividend) {
          low = mid;         //search higher
          quotient = mid;    
      } else {
          high = mid - BigInt(1); //search lower
      }
  }

  quotient.isNegative = (this->isNegative != rhs.isNegative);  //handle sign of quotient
  return quotient;
}


// Division by 2 helper method
BigInt BigInt::div_by_2() const {
  BigInt result;
  uint64_t carry = 0;

  for (auto it = elements.rbegin(); it != elements.rend(); ++it) {
      uint64_t value = *it;
      result.elements.push_back((value >> 1) | carry);  //shift right and add carry
      carry = (value & 1) ? 0x8000000000000000 : 0;  //ternary operators
  }

  std::reverse(result.elements.begin(), result.elements.end());  //reverse the vector
  result.isNegative = isNegative;
  return result;
}

// Convert BigInt to decimal string representation
std::string BigInt::to_dec() const
{
  if (is_zero()) {
    return "0";
  }

  std::stringstream dec;
  if (isNegative) {
    dec << "-";
  }

  BigInt current = *this;
  std::vector<char> digits;

  while (!current.is_zero()) {
    BigInt last = current - ((current/10) * 10);  //get the last digit
    current = current / 10;  //reduce the number
    uint64_t digit = last.get_bits(0);
    dec << digit;  //append
  }

  std::string reversed = dec.str();
  reverse(reversed.begin(), reversed.end());  //reverse the string to get the correct order
  return dec.str();

}

// Compare two BigInt values
int BigInt::compare(const BigInt &rhs) const
{
  if (isNegative != rhs.is_negative()) {
    if (isNegative) {
      return -1;
    } else {
        return 1;
    }
  }

  int comp = compare_magnitudes(*this, rhs);

  if (isNegative) {
    return -comp;
  } else {
    return comp;
  }
}

// Convert BigInt to hexadecimal string representation
std::string BigInt::to_hex() const
{
 if (is_zero()) {
    return "0";
  }

  std::stringstream dec;
  if (isNegative) {
    dec << "-";
  }

  BigInt current = *this;
  std::vector<char> digits;

  while (!current.is_zero()) {
    BigInt last = current - ((current/10) * 10);  //get the last digit
    current = current / 10;  //reduce the number
    uint64_t digit = last.get_bits(0);
    dec << digit;  //append
  }

  std::string reversed = dec.str();
  reverse(reversed.begin(), reversed.end());  //reverse the string to get the correct order
  return dec.str();

}

// Check if the value is zero
bool BigInt::is_zero () const {
  for (auto it = elements.begin(); it != elements.end(); ++it) {
    if (*it != 0) {
      return false;  //return false if any element is not zero
    }
  }
  return true;  //otherwise return true
}

// Add magnitudes of two BigInt numbers helper method
BigInt BigInt::add_magnitudes(const BigInt &lhs, const BigInt &rhs) {
  BigInt result;
  std::vector<std::uint64_t> result_vector;
  bool overflow = false;
  auto itl = lhs.elements.begin();
  auto itr = rhs.elements.begin();
  
  if (lhs.is_zero()) {
    result.elements = rhs.elements;
    return result;
  } else if (rhs.is_zero()) {
    result.elements = lhs.elements;
    return result;
  } else {
    for (; itl != lhs.elements.end() && itr != rhs.elements.end(); ++itl, ++itr) {
      uint64_t l_elemnt = *itl;
      uint64_t r_elemnt = *itr;
      uint64_t result = (l_elemnt + r_elemnt);
      if (overflow == true) {
        result++;
        overflow = false;
      }
      if (result < l_elemnt) {
        overflow = true;
      }
      result_vector.push_back(result);
    }
     while (itl != lhs.elements.end() || itr != rhs.elements.end() || overflow) {
        uint64_t l_elemnt = (itl != lhs.elements.end()) ? *itl : 0;    // ternary operators replacing if else statements
        uint64_t r_elemnt = (itr != rhs.elements.end()) ? *itr : 0;
        uint64_t sum = l_elemnt + r_elemnt + overflow;
        overflow = (sum < l_elemnt) || (sum < r_elemnt);
        result_vector.push_back(sum);
        
        if (itl != lhs.elements.end()) ++itl;
        if (itr != rhs.elements.end()) ++itr;
    }
    if (overflow == true) {
      result_vector.push_back(1UL);  //add final carry if overflow
    }
    result.elements = result_vector;
    return result;
  }
}

// Subtract magnitudes of two BigInt numbers helper method
BigInt BigInt::subtract_magnitudes(const BigInt &lhs, const BigInt &rhs) { //lhs > rhs
  BigInt result;
  std::vector<std::uint64_t> result_vector;
  bool borrow = false;
  auto itl = lhs.elements.begin();
  auto itr = rhs.elements.begin();
  for (; itl != lhs.elements.end() && itr != rhs.elements.end(); ++itl, ++itr) {
    uint64_t l_elemnt = *itl;
    uint64_t r_elemnt = *itr;
    if (borrow == true) {
      if (l_elemnt > 0) {
        l_elemnt--;
        borrow = false;
      } else {
        l_elemnt = 0xFFFFFFFFFFFFFFFFUL;
      }
    }
    uint64_t result = (l_elemnt - r_elemnt);
    if (l_elemnt < r_elemnt) {
      result = 0xFFFFFFFFFFFFFFFFUL - r_elemnt + 1 + l_elemnt;
      borrow = true;
    }
    result_vector.push_back(result);
  }
  while (itl != lhs.elements.end()) {
    if (borrow == true) {
      if (*itl > 0) {
        result_vector.push_back(*itl -1);
        borrow = false;
      } else {
        result_vector.push_back(0xFFFFFFFFFFFFFFFFUL);
      }
      ++itl;
    } else {
      result_vector.push_back(*itl);
      ++itl;
    }
  }
  result.elements = result_vector;
  return result;
}

// Compare magnitudes of two BigInt numbers helper method
int BigInt::compare_magnitudes(const BigInt &lhs, const BigInt &rhs) {
  int left = lhs.elements.size();
  int right = rhs.elements.size();
  if (left > right) {
      return 1;
  } else if (right > left) {
      return -1;
  } else if (left == 0 && right == 0) {
      return 0;
  } else {
      auto itl = lhs.elements.rbegin();
      auto itr = rhs.elements.rbegin();
      for (; itl != lhs.elements.rend() && itr != rhs.elements.rend(); ++itl, ++itr) {
          uint64_t l_elemnt = *itl;
          uint64_t r_elemnt = *itr;
          if (l_elemnt > r_elemnt) {
              return 1;
          } else if (l_elemnt < r_elemnt) {
              return -1;
          }
      }
    return 0;
  }
}