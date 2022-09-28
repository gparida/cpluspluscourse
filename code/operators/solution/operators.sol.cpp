#include <cassert>
#include <iomanip>
#include <iostream>
#include <numeric>

class Fraction {
 public:
  explicit Fraction(int i) : m_num(i), m_denom(1) {}
  Fraction(int num, int denom) : m_num(num), m_denom(denom) {
    normalize();
  }

  friend std::ostream& operator<<(std::ostream& os, Fraction const & f) {
    os << f.m_num << "/" << f.m_denom;
    return os;
  }

  Fraction & operator*=(int i) {
    m_num *= i;
    normalize();
    return *this;
  }

  Fraction & operator*=(Fraction const & o) {
    m_num *= o.m_num;
    m_denom *= o.m_denom;
    normalize();
    return *this;
  }

  friend Fraction operator*(Fraction f, int i) { return f *= i; }
  friend Fraction operator*(int i, Fraction const & f) { return f * i; }
  friend Fraction operator*(Fraction a, Fraction const & b) { return a *= b; }

  Fraction & operator+=(int i) {
    m_num += i * m_denom;
    return *this;
  }

  Fraction & operator+=(Fraction o) {
    m_num *= o.m_denom;
    m_num += o.m_num * m_denom;
    m_denom *= o.m_denom;
    normalize();
    return *this;
  }

  friend Fraction operator+(Fraction r, int i) { return r += i; }
  friend Fraction operator+(int i, Fraction const & r) { return r + i; }
  friend Fraction operator+(Fraction a, Fraction const & b) { return a += b; }

  friend bool operator==(Fraction const & a, Fraction const & b) {
    return a.m_num == b.m_num && a.m_denom == b.m_denom;
  }

  friend bool operator<(Fraction const & a, Fraction const & b) {
    return a.m_num * b.m_denom < b.m_num * a.m_denom;
  }

  friend bool operator!=(Fraction const & a, Fraction const & b) { return !(a == b); }
  friend bool operator>(Fraction const & a, Fraction const & b) { return b < a; }
  friend bool operator<=(Fraction const & a, Fraction const & b) { return !(a > b); }
  friend bool operator>=(Fraction const & a, Fraction const & b) { return !(a < b); }

 private:
  void normalize() {
    const int gcd = std::gcd(m_num, m_denom);
    m_num /= gcd;
    m_denom /= gcd;
  }

  int m_num, m_denom;
};

void printAndCheck(std::string const & what, Fraction const & result, Fraction const & expected) {
  const bool passed = result == expected;
  std::cout << std::left << std::setw(40) << what << ": " << (passed ? "PASS" : "** FAIL **") << "    " << result << "\n";
}
void printAndCheck(std::string const & what, bool result, bool expected) {
  const bool passed = result == expected;
  std::cout << std::left << std::setw(40) << what << ": " << (passed ? "PASS" : "** FAIL **") << "    " << result << "\n";
}

int main() {
  // create a fraction with values 3 (which is 3/1) and 1/3
  const Fraction three{3};
  const Fraction athird{1, 3};

  // print the fractions
  std::cout << "Three: " << three << '\n';
  std::cout << "One third: " << athird << '\n';

  // multiply fraction with an int
  // the printAndCheck function requires operator<< and operator==:
  printAndCheck("One third times two", athird * 2, Fraction{2, 3});
  // ensure symmetry
  printAndCheck("Two times one third", 2 * athird, Fraction{2, 3});

  // multiply two fractions
  printAndCheck("Three times one third", three * athird, Fraction{1, 1});
  // normalize the fraction after multiplication so the above statement
  // prints 1/1 instead of e.g. 3/3
  // you might need to compute the greatest common divisor, for which you can
  // use the function std::gcd(a, b) from the <numeric> header
  printAndCheck("Three times one third", 3 * athird, Fraction{1, 1});

  // multiply in place
  Fraction f = athird;
  f *= 2;
  printAndCheck("One third times two", f, Fraction{2, 3});

  f = athird;
  f *= athird;
  printAndCheck("One third times one third", f, Fraction{1, 9});

  f = athird;
  f *= f *= f;
  printAndCheck("One third times itself twice", f, Fraction{1, 81});

  // you might have some redundancy between the implementation of operator* and
  // operator*=. Can you refactor your code and implement operator* in terms of
  // operator*=?

  // add an int to a fraction
  printAndCheck("One third plus 2", athird + 2, Fraction{7, 3});
  // ensure symmetry
  printAndCheck("2 plus one third", 2 + athird, Fraction{7, 3});

  // add two fractions
  printAndCheck("One third plus two sixth", athird + Fraction{2, 6}, Fraction{2, 3});
  // normalize the fraction after addition too so the above statement
  // prints 2/3 instead of 12/18
  // make sure the normalization between addition and multiplication is not
  // duplicated, e.g. by putting it into a separate method

  // add in place
  f = athird;
  f += f += 1;
  printAndCheck("One third plus one and added to itself", f, Fraction{8, 3});
  // again, try to refactor and implement operator+ in terms of operator+=

  std::cout << std::boolalpha;  // print bools as 'true' or 'false' from now on

  // more equality comparisons
  printAndCheck("One third == one third", (athird == Fraction{1, 3}), true);
  printAndCheck("One third != one forth", (athird != Fraction{1, 4}), true);
  printAndCheck("One third == two sixth", (athird == Fraction{2, 6}), true);
  printAndCheck("One third != three sixth", (athird != Fraction{3, 6}), true);
  // try to implement operator!= in terms of operator==

  // more comparisons
  const Fraction afourth{1, 4};
  printAndCheck("athird < athird", (athird < athird), false);
  printAndCheck("afourth < athird", (afourth < athird), true);
  printAndCheck("athird  <= athird", (athird <= athird), true);
  printAndCheck("athird  <= afourth", (athird <= afourth), false);
  printAndCheck("athird  > athird", (athird > athird), false);
  printAndCheck("afourth > athird", (afourth > athird), false);
  printAndCheck("athird >= athird", (athird >= athird), true);
  printAndCheck("athird >= afourth", (athird >= afourth), true);
  // the operators <=, >= and > can typically be implemented just in terms of
  // operator<. Can you do this as well? ;)

  // take aways on operators:
  // * we can very often implement an arithemtic operator@ in terms of
  // operator@=
  // * it usually suffices to implement operator< and operator== and derive the
  // other relational operators from them. C++20 will do part of this automatically.
}
