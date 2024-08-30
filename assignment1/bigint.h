#ifndef BIGINT_H
#define BIGINT_H

#include <initializer_list>
#include <vector>
#include <string>
#include <cstdint>

//! @file
//! Arbitrary-precision integer data type.

//! Class representing an arbitrary-precision integer represented as a bit string
//! (implemented using a vector of `uint64_t` elements) and a boolean flag
//! to record whether or not the value is negative.
class BigInt {
private:
  // TODO: add fields

public:
  //! Default constructor.
  //! The initialized BigInt value should be equal to 0.
  BigInt();

  //! Constructor from an `std::initializer_list` of `uint64_t` values
  //! to initialize the BigInt object's bit string, and (optionally)
  //! a boolean value indicating whether the value is negative.
  //!
  //! @param vals `std::initializer_list` of `uint64_t` values,
  //!             in order from less-significant to more-significant
  //! @param negative if true, the value is negative
  BigInt(std::initializer_list<uint64_t> vals, bool negative = false);

  //! Constructor from a `uint64_t` value and (optionally) a boolean
  //! indicating whether the value is negative.
  //!
  //! @param val a `uint64_t` value indicating the magnitude of the
  //!            BigInt value
  //! @param negative if true, the value is negative
  BigInt(uint64_t val, bool negative = false);

  //! Copy constructor.
  //!
  //! @param other another BigInt object that this object should be made
  //!              identical to
  BigInt(const BigInt &other);

  //! Destructor.
  ~BigInt();

  //! Assignment operator.
  //!
  //! @param rhs another BigInt object that this object should be made
  //!            identical to
  BigInt &operator=(const BigInt &rhs);

  //! Check whether value is negative.
  //!
  //! @return true if the value is negative, false otherwise
  bool is_negative() const;

  //! Get one `uint64_t` chunk of the overall bit string.
  //! Note that this function should work correctly regardless of the
  //! index passed in. If the index passed in is greater than the index
  //! of any explicit element in the vector containing the bit string,
  //! it should return 0.
  //!
  //! @param index the index of the `uint64_t` value to retrieve (0 indicates
  //!              the least-significant 64 bits, etc.)
  //! @return the `uint64_t` value containing the requested bits (0 if
  //!         the index is outside the bounds of the internal vector
  //!         containing the bit string)
  uint64_t get_bits(unsigned index) const;

  //! Return a const reference to the underlying vector of
  //! `uint64_t` values representing the bits of the magnitude of the
  //! overall BigInt value. Note that the values should be in
  //! "little endian" order: element 0 is the lowest 64 bits,
  //! eleemnt 1 is the next-lowest 64 bits, etc. In theory,
  //! all you should need to do is return a reference to the
  //! internal vector the BigInt object keeps its magnitude bits in.
  //!
  //! @return const reference to the vector containing the bit string values
  //!         (element at index has the least-significant 64 bits, etc.)
  const std::vector<uint64_t> &get_bit_vector() const;

  //! Addition operator.
  //!
  //! @param rhs the right-hand side BigInt value (the left hand value
  //!            is the implicit receiver object, i.e., `*this`)
  //! @return the BigInt value representing the sum of the operands
  BigInt operator+(const BigInt &rhs) const;

  //! Subtraction operator.
  //!
  //! @param rhs the right-hand side BigInt value (the left hand value
  //!            is the implicit receiver object, i.e., `*this`)
  //! @return the BigInt value representing the difference of the operands
  BigInt operator-(const BigInt &rhs) const;

  //! Unary negation operator.
  //!
  //! @return the BigInt value representing the negation of this
  //!         BigInt value
  BigInt operator-() const;

  //! Test whether a specific bit in the bit string is set to 1.
  //!
  //! @param n the bit to test (0 for the least significant bit, etc.)
  //! @return true if bit `n` is set to 1, false if it is set to 0
  bool is_bit_set(unsigned n) const;

  //! Left shift by n bits. Note that it is only allowed
  //! to use this operation on non-negative values.
  //! An `std::invalid_argument` exception is thrown if
  //! this BigInt is negative.
  //!
  //! @param n number of bits to shift left by
  //! @return BigInt value representing the result of shifting this]
  //!         value left by `n` bits
  //! @throw std::invalid_argument if this object represents a negative value
  BigInt operator<<(unsigned n) const;

  //! Multiplication operator.
  //!
  //! @param rhs the right-hand side BigInt value (the left hand value
  //!            is the implicit receiver object, i.e., `*this`)
  //! @return the BigInt value representing the product of the operands
  BigInt operator*(const BigInt &rhs) const;

  //! Division operator.
  //! Note that since BigInt objects represent integers, this
  //! operator should return a quotient value with the largest
  //! magnitude such that the magnitude of the product of the right-hand
  //! value and the quotient is less than or equal to the
  //! dividend (the left-hand object.) In simpler terms, the
  //! magnitude of the quotient should be truncated (not rounded).
  //!
  //! Some examples to illustrate:
  //! - `5 / 2 = 2`
  //! - `-5 / 2 = -2`
  //! - `5 / -2 = -2`
  //! - `-5 / -2 = 2`
  //!
  //! @param rhs the right-hand side BigInt value (the left hand value
  //!            is the implicit receiver object, i.e., `*this`)
  //! @return the quotient resulting from dividing the left hand
  //!         BigInt by the right-hand BigInt
  //! @throw std::invalid_argument if the right hand object is
  //!        equal to 0
  BigInt operator/(const BigInt &rhs) const;

  //! Compare two BigInt values, returning
  //!   - negative if lhs < rhs
  //!   - 0 if lhs = rhs
  //!   - positive if lhs > rhs
  //!
  //! @param rhs the right-hand side BigInt value (the left hand value
  //!            is the implicit receiver object, i.e., `*this`)
  //! @return the result of the comparison (negative means less,
  //!         0 means equal, positive means greater)
  int compare(const BigInt &rhs) const;

  // comparison operators: you won't need to modify these,
  // since they're all implemented using compare
  bool operator==(const BigInt &rhs) const { return compare(rhs) == 0; }
  bool operator!=(const BigInt &rhs) const { return compare(rhs) != 0; }
  bool operator<(const BigInt &rhs) const  { return compare(rhs) < 0; }
  bool operator<=(const BigInt &rhs) const { return compare(rhs) <= 0; }
  bool operator>(const BigInt &rhs) const  { return compare(rhs) > 0; }
  bool operator>=(const BigInt &rhs) const { return compare(rhs) >= 0; }

  //! Return a string representing the value of this BigInt, in
  //! lower-case hexadecimal (base-16). Note that there should be a leading
  //! minus sign (`-`) if this value is negative.
  //!
  //! @return the value of this BigInt object in hexadecimal
  std::string to_hex() const;

  //! Return a string representing the value of this BigInt, in
  //! decimal (base-10). Note that there should be a leading
  //! minus sign (`-`) if this value is negative.
  //!
  //! @return the value of this BigInt object in decimal (base-10)
  std::string to_dec() const;

private:
  // TODO: add helper functions
};

#endif // BIGINT_H
