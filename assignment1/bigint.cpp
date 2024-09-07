#include <cassert>
#include "bigint.h"
#include <cstdint>
#include <sstream>
#include <vector>
#include <iostream>
#include <iomanip>


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
  // TODO: implement
}

BigInt BigInt::operator-(const BigInt &rhs) const
{
  // TODO: implement
  // Hint: a - b could be computed as a + -b
  
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
  // TODO: implement
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
  // TODO: implement
}

std::string BigInt::to_hex() const
{
  // TODO: implement

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
  // TODO: implement
}

bool BigInt::is_zero () const {
  for (auto it = elements.begin(); it != elements.end(); ++it) {
    if (*it != 0) {
      return false;
    }
  }
  return true;
}

BigInt add_magnitudes(const std::vector<std::uint64_t> lhs, const std::vector<std::uint64_t> rhs) {
  BigInt obj;
  std::vector<std::uint64_t> result_vector;
  bool overflow = false;
  auto itl = lhs.begin();
  auto itr = rhs.begin();
  for (itl, itr; itl != lhs.end() && itr != lhs.end(); ++itl, ++itr) {
    uint64_t l_elemnt = *itl;
    uint64_t r_elemnt = *itr;
    uint64_t result = (l_elemnt + r_elemnt);
    if (overflow == true) {
      result++;
      overflow = false;
    }
    if (result < l_elemnt) {
      result = (l_elemnt + r_elemnt) % (1UL << 63);
      overflow = true;
    }
    result_vector.push_back(result);
  }
  while (itl != lhs.end()) {
    result_vector.push_back(*itl);
    ++itl;
  }
  while (itr != lhs.end()) {
    result_vector.push_back(*itr);
    ++itr;
  }
}

