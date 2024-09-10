#include <cassert>
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
  elements.push_back(val); //NOT SURE ABT THIS ONE, IDK WHAT IT MEANS
  isNegative = negative;
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
  if (isNegative == rhs.is_negative()) {
    BigInt obj = add_magnitudes(*this, rhs);
    obj.isNegative = isNegative;
    return obj;
  } else if (compare_magnitudes(*this, rhs) > 0) {
    BigInt obj = subtract_magnitudes(*this, rhs);
    obj.isNegative = is_negative();
    return obj;
  } else {
    BigInt obj = subtract_magnitudes(rhs, *this);
    obj.isNegative = rhs.is_negative();
    return obj;
  }
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
  // TODO: implement
}

BigInt BigInt::operator*(const BigInt &rhs) const
{
  // TODO: implement
}

BigInt BigInt::operator/(const BigInt &rhs) const
{
  // TODO: implement
}

int BigInt::compare(const BigInt &rhs) const
{
  if (this->is_zero()) {
    if (rhs.is_zero()) {
      return 0;
    }
    return -1;
  }
  if (rhs.is_zero()) {
    return 1;
  }

  if (isNegative == rhs.is_negative()) {
    return compare_magnitudes(*this, rhs);
  } else if (isNegative == false && rhs.is_negative() == true) {
    return 1;
  } else {
    return -1;
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
  for (auto it = current.elements.rbegin(); it != current.elements.rend(); ++it) {
    while (*it != 0) {
      uint64_t mod = *it - ((*it / 10) * 10);
      dec << mod;
      *it = *it / 10;
    }
  }
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
    while (itl != lhs.elements.end()) {
      if (overflow == true) {
        if (*itl < 0xFFFFFFFFFFFFFFFFUL) {
          result_vector.push_back(*itl+1);
          overflow = false;
        } else {
          result_vector.push_back(0UL);
        }
      }
      result_vector.push_back(*itl);
      ++itl;
    }
    while (itr != rhs.elements.end()) {
      if (overflow == true) {
        if (*itr < 0xFFFFFFFFFFFFFFFFUL) {
          result_vector.push_back(*itr+1);
          overflow = false;
        } else {
          result_vector.push_back(0UL);
        }
      }
      result_vector.push_back(*itr);
      ++itr;
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

