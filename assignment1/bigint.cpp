#include <cassert>
#include "bigint.h"
#include <cstdint>
#include <vector>

BigInt::BigInt()
  // TODO: initialize member variables
{
  isNegative = false;
}

BigInt::BigInt(std::initializer_list<uint64_t> vals, bool negative)
  // TODO: initialize member variables
{
  isNegative = negative;
  elements = vals;
}

BigInt::BigInt(uint64_t val, bool negative)
  // TODO: initialize member variables
{
  elements[0] = val; //NOT SURE ABT THIS ONE, IDK WHAT IT MEANS
  isNegative = negative;
}

BigInt::BigInt(const BigInt &other)
  // TODO: initialize member variables
{
  isNegative = other.isNegative;
  elements = other.elements;
}

BigInt::~BigInt()
{
}

BigInt &BigInt::operator=(const BigInt &rhs)
{
  // TODO: implement
  if (this != &rhs) {
    isNegative = rhs.isNegative;
    elements = rhs.elements;
  }
  return *this;
}

bool BigInt::is_negative() const
{
  // TODO: implement
  return isNegative;
}

uint64_t BigInt::get_bits(unsigned index) const
{
  // TODO: implement
  return elements[index];
}

const std::vector<uint64_t> &BigInt::get_bit_vector() const {
  // TODO: implement
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
  // TODO: implement
}

bool BigInt::is_bit_set(unsigned n) const
{
  // TODO: implement
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
}

std::string BigInt::to_dec() const
{
  // TODO: implement
}

