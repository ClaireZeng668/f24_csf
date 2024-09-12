TODO: list team members, document who did what, discuss
anything interesting about your implementation.

Team members:

Prasi Thapa: 
Implemented
- operator*(const BigInt &) member function
- compare(const BigInt &) member function
- operator<<(unsigned) member function (left shift)
- operator/(const BigInt &) member function (division)
- bool is_zero() const;
- BigInt div_by_2() const;
Wrote unit tests

Claire Zeng: 
Implemented
- is_bit_set(unsigned) member function
- operator+(const BigInt &) member function (addition)
- operator-(const BigInt &) member function (two-operand subtraction)
- to_dec() member function
- static BigInt add_magnitudes(const BigInt &lhs, const BigInt &rhs);
- static BigInt subtract_magnitudes(const BigInt &lhs, const BigInt &rhs);
- static int compare_magnitudes(const BigInt &lhs, const BigInt &rhs);
Wrote unit tests


Our implementation: We tried several times to abide by the homework rules and utilize uint64_t rather than __uint128_t. We wrote many implemenations of the multiplication operator,
for instance splitting the 128-bit product into two 64-bit parts. No matter our implemenation, the autograder tests for multiplication operator would
fail unless __uint128_t was used, which is why you can its usage in our final implemenation.