# Colossal Numbers Stack Calculator

A stack-based arbitrary-precision calculator implemented in **C23** that performs arithmetic operations on **Colossal Numbers** (a recursive, hereditary base-2 representation considered by Donald Knuth).

## Overview

In the colossal number representation, every non-negative integer is represented as a list of digits, where each digit is itself a colossal number. The value of a colossal number is the sum of powers of two whose exponents are the values of its digits:

$$N = \sum_{i=0}^{m} 2^{d_i}$$

A colossal number is **normalized** if all of its digits are normalized and strictly ordered in descending order by value. Every non-negative integer has a unique normalized representation. This non-positional recursive structure allows the program to compactly represent and compute values such as $2^{2^{2^{65536}}}$, whose standard binary positional representation would exceed the number of atoms in the observable universe.

### Text Encoding (EBNF)

Numbers are encoded as strings of `1`s and `0`s according to the following grammar:

```ebnf
<colossal> ::= { "1" <digit> } "0"
<digit>    ::= <colossal>
