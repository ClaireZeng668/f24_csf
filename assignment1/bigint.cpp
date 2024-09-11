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


BigInt::BigInt()
{
  isNegative = false;
}

BigInt::BigInt(std::initializer_list<uint64_t> vals, bool negative)
{
  isNegative = negative;
  elements = vals;
}

BigInt::BigInt(uint64_t val, bool negative)
{
  elements.push_back(val); // push the value to the vector
  isNegative = negative;   // set whether the value is negative or not
}

BigInt::BigInt(const BigInt &other)
{
  isNegative = other.isNegative;
  elements = other.elements;
}

BigInt::~BigInt()
{
}

BigInt &BigInt::operator=(const BigInt &rhs)
{
  if (this != &rhs) {
    isNegative = rhs.isNegative;
    elements = rhs.elements;
  }
  return *this;
}

bool BigInt::is_negative() const
{
  return isNegative;
}

uint64_t BigInt::get_bits(unsigned index) const
{
  if (index >= elements.size()) {
    return 0;
  }
  return elements.at(index);
}

const std::vector<uint64_t> &BigInt::get_bit_vector() const {
  return elements;
}

BigInt BigInt::operator+(const BigInt &rhs) const
{
  BigInt obj;
  if (isNegative == rhs.is_negative()) {
    obj = add_magnitudes(*this, rhs);
    if (!obj.is_zero()) {
      obj.isNegative = isNegative;
    }
  } else if (compare_magnitudes(*this, rhs) > 0) {     
    obj = subtract_magnitudes(*this, rhs);
    if (!obj.is_zero()) {
      obj.isNegative = is_negative();
    }
  } else {
    obj = subtract_magnitudes(rhs, *this);
    if (!obj.is_zero()) {
      obj.isNegative = rhs.is_negative();
    }
  }
  return obj;
}

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

BigInt BigInt::operator-() const
{
  BigInt obj = BigInt(*this);
  if(!is_zero()) {
    obj.isNegative = !this->isNegative;
  }
  return obj;
}

bool BigInt::is_bit_set(unsigned n) const
{
  if (is_zero()) {
    return false;
  }
  int pos = 0;
  if (n >= 64) {
    pos = n/64;
  }
  int bit = n%64;
  if (bit == 0) {
    return elements[pos] % 2 != 0;
  }
  if (elements[pos] > (1UL << (bit))) {
    int section = elements[pos] / (1UL << (bit));
    if (section % 2 == 0) {
      return false;
    }
    return true;
  }
  return false;
}


BigInt BigInt::operator<<(unsigned n) const
{

    if (isNegative) {
        throw std::invalid_argument("Cannot left shift a negative value");
    }

    if (n == 0 || this->is_zero()) {
        return *this;
    }

    BigInt result;
    unsigned shift_units = n / 64;
    unsigned shift_bits = n % 64;

    result.elements.resize(shift_units, 0);
    result.elements.insert(result.elements.end(), elements.begin(), elements.end());

    if (shift_bits > 0) {
        uint64_t carry = 0;
        for (std::size_t i = 0; i < result.elements.size(); ++i) {
            uint64_t temp = result.elements[i];
            result.elements[i] = (temp << shift_bits) | carry;
            carry = (temp >> (64 - shift_bits));
        }
        if (carry > 0) {
            result.elements.push_back(carry);
        }
    }

    result.isNegative = isNegative;
    return result;
}



BigInt BigInt::operator*(const BigInt &rhs) const
{
  BigInt result;
  result.elements.resize(this->elements.size() + rhs.elements.size(), 0);

  for (std::size_t i = 0; i < this->elements.size(); ++i) {
      uint64_t carry = 0;
      for (std::size_t j = 0; j < rhs.elements.size(); ++j) {
          __uint128_t product = (__uint128_t)this->elements[i] * rhs.elements[j] + result.elements[i + j] + carry;
          result.elements[i + j] = (uint64_t)product;
          carry = (uint64_t)(product >> 64);
      }
      result.elements[i + rhs.elements.size()] += carry;
  }

  result.isNegative = this->isNegative != rhs.isNegative;
  return result;
}


BigInt BigInt::operator/(const BigInt &rhs) const {
  if (rhs.is_zero()) {
      throw std::runtime_error("Division by zero");
  }
  
  BigInt dividend = *this;
  BigInt divisor = rhs;
  
  dividend.isNegative = false;  // work with absolute values
  divisor.isNegative = false;

  if (dividend < divisor) {
      return BigInt(0);
  }
  
  BigInt quotient = BigInt(0);
  BigInt one = BigInt(1);
  
  while (dividend >= divisor) {
      BigInt temp = divisor;
      BigInt multiple = one;
      
      while (dividend >= (temp << 1)) {
          temp = temp << 1;
          multiple = multiple << 1;
      }
      
      dividend = dividend - temp;
      quotient = quotient + multiple;
  }
  
  quotient.isNegative = (this->isNegative != rhs.isNegative);
  return quotient;
}

BigInt BigInt::div_by_2() const {
    BigInt result;
    uint64_t carry = 0;

    for (auto it = elements.rbegin(); it != elements.rend(); ++it) {
        uint64_t value = *it;
        result.elements.push_back((value >> 1) | carry);
        carry = (value & 1) ? 0x8000000000000000 : 0;
    }

    std::reverse(result.elements.begin(), result.elements.end());
    result.isNegative = isNegative;
    return result;
}


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

std::string BigInt::to_hex() const
{
  if (is_zero()) {
    return "0";
  }
  std::stringstream hex;
  if (isNegative) {
    hex << "-";
  }
  for (auto it = elements.rbegin(); it != elements.rend(); ++it) {
    if (it == elements.rbegin()) {
      hex << std::hex << *it;
    } else {
      hex << std::hex << std::setw(16) << std::setfill('0') << *it;
    }
  }
  return hex.str();
}

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
  while (!current.is_zero()) {
    BigInt last = current - ((current/10) * 10);
    current = current / 10;
    uint64_t digit = last.get_bits(0);
    dec << digit;
  }
  // for (auto it = current.elements.begin(); it != current.elements.end(); ++it) {
  //   while (*it != 0) {
  //     uint64_t mod = *it - ((*it / 10) * 10);
  //     dec << mod;
  //     *it = *it / 10;
  //   }
  // }
  std::string reversed = dec.str();
  reverse(reversed.begin(), reversed.end());
  return dec.str();

}


bool BigInt::is_zero () const {
  for (auto it = elements.begin(); it != elements.end(); ++it) {
    if (*it != 0) {
      return false;
    }
  }
  return true;
}

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
      result_vector.push_back(1UL);
    }
    result.elements = result_vector;
    return result;
  }
}

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
  // while (itr != rhs.get_bit_vector().end()) {
  //   result_vector.push_back(*itr);
  //   ++itr;
  // }
  result.elements = result_vector;
  return result;
}


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

